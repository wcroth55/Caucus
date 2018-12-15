
/*** CHOOSE_NAME.   Ask the user to choose a single name from a pattern.
/
/    ok = choose_name (pattern, result, error);
/
/    Parameters:
/       int       ok;       (success?)
/       Address  *pattern;  (original pattern of names)
/       Address  *result;   (final result)
/       int      *error;    (returned error code)
/
/    Purpose:
/       There are many places in Caucus where we have a name pattern 
/       that may describe several people, and need to resolve this down
/       to one single person. 
/
/       Choose_name() attempts to map PATTERN into a single person,
/       whose address is placed in RESULT.  It does this automatically
/       if it can, otherwise it asks the user to choose among the
/       the possibilities.
/
/       Note that in the case of a PATTERN that refers to a user on
/       another host, choose_name() knows that its knowledge about
/       a foreign host may be imperfect.  It offers the user the
/       choice of the unchanged PATTERN (aint4 with any other matches
/       it may know of).
/
/    How it works:
/       Patterns that begin with the CMI escape (e.g., "=roth@thor")
/       are passed through unchanged. 
/
/       Patterns that begin with the form "(userid)" have 'userid' 
/       extracted and returned in RESULT->id.  (If 'userid' doesn't
/       exist, choose_name() tries 'userid_'.  If that doesn't work,
/       we just return 'userid'.)
/
/       All other patterns are passed to name_to_netid() to do the 
/       dirty work of looking up the matches.
/
/       If more than one match is found, including the special case
/       of a name and host but no id (e.g., "john@ringo"), the user
/       is asked to select one by number.
/
/   WARNING: this code has many kludges in anticipation of being completely
/       revamped for CaucusLink 1.2.  Be Warned.
/
/    Returns: 1 if successful.
/             0 if no matches made or selected, or other error occurred.
/
/    Error Conditions: sets ERROR to:
/       0          no errors (success, or user explicitly chose no one.)
/       NOPERSON   no person matched user's request.
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus (sendwho()...)
/
/    Operating system dependencies:
/
/    Known bugs:
/       The "(userid)" checking should really use two dictionary strings
/       for "(" and ")".
/
/    Home:  people/choosena.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/11/91 13:15 New function. */
/*: CR  7/16/91 13:06 Apply sysvalid() to cmi addresses, fix "special" bug. */
/*: CR  7/16/91 15:25 Correct "special" bug.*/
/*: JV 12/03/91 16:16 Remove special case && no matches. */
/*: CR 12/20/91 12:48 Handle "name@ambiguous_host"; add ERROR argument. */
/*: CR  2/03/92 16:24 Multiple matches; clear id if NOT NULLSTR(host). */
/*: DE  4/09/92 17:50 Chixified */
/*: JX  5/15/92 16:13 Fix DE code. */
/*: JX 10/02/92 13:59 More fixification. */
/*: CR 10/11/92 13:23 Check return value from chxnum(). */
/*: CR 12/10/92 15:31 chxindex 2nd arg is int4. */
/*: CR  2/05/93 15:04 chxfree newstr. */
/*: CW  7/04/93 14:15 Recognize (userid), for Wigwam support. */
/*: CP  7/26/93 12:53 CHXSUB must use int4s. */
/*: CR 10/12/01 16:40 Ignore @'s!  Replace with "^" for now. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;
extern Chix         ss_lparen, ss_rparen;
extern Chix         ss_luid, ss_ruid;

FUNCTION  choose_name (pattern, result, error)
   Address pattern, result;
   int     *error;
{
   Chix     cmiesc, localhost, answer, thisid, thishost, tempchx, newstr;
   Namelist list, p;
   Namelist nlnode();
   int      errcode, matches, i, special, specnum, ok;
   int      success;
   int4     person;

   ENTRY ("choose_name", "");

   cmiesc      = chxalloc (L(40),  THIN, "choosena cmiesc");
   localhost   = chxalloc (L(100), THIN, "choosena localhost");
   answer      = chxalloc (L(20),  THIN, "choosena answer");
   thisid      = chxalloc (L(80),  THIN, "choosena thisid");
   thishost    = chxalloc (L(80),  THIN, "choosena thishost");
   newstr      = chxalloc (L(100), THIN, "choosena newstr");
   success     = 0;

   *error = NOPERSON;

   /*** CMI addresses (e.g. "=user@host") are passed through unchanged. */
   mdstr (cmiesc, "ss_cmiesc", null.md);
   if (chxindex (pattern->name, L(0), cmiesc) == 0) {
      if (NOT sysvalid (tempchx = CHXSUB(pattern->name, chxlen(cmiesc)))) {
         chxfree (tempchx);
         FAIL;
      }
      chxfree (tempchx);
      chxcpy (result->name, pattern->name);
      chxcpy (result->id,   pattern->id);
      chxcpy (result->host, pattern->host);
/*    *result = *pattern; */
      *error  = 0;
      SUCCEED;
   }

   /*** Specific caucus id address (e.g., "(xyz)") extract the id 
   /    directly.   (If 'xyz' doesn't exist, try 'xyz_'.) */
   if (chxindex (pattern->name, L(0), CQ("(")) == 0) {
      tempchx = CHXSUB (pattern->name, L(1));
      chxbreak  (tempchx, result->id, answer, CQ(")"));
      chxclear  (result->name);
      if (NOT choose_exists (result->id)) {
         chxconc (tempchx, result->id, CQ("_"));
         if  (choose_exists (tempchx))   chxcpy (result->id, tempchx);
      }
      chxfree   (tempchx);
      SUCCEED;
   }

   /*** Take note of the "special" case of name@foreign_host without an id. */
   getmyhost (localhost);
   special = (EMPTYCHX(pattern->id)  &&  NOT EMPTYCHX(pattern->host)  &&
              NOT chxeq (pattern->host, localhost));

   /*** Find the LIST of matches to PATTERN. */
   chxclear(result->name); 
   chxclear(result->id);
   chxclear(result->host);
   list = nlnode(1);
   name_to_netid (pattern->name, pattern->id, pattern->host, 0, list, &errcode);
   if (errcode != NLE_NOERR) { nlfree (list);   FAIL; }
 
   /*** If only 1 match, we're done, go home! */
   matches = nlsize (list);
   if (matches == 1) {
      if (NOT special)
         chxbreak (list->next->str, result->id,   result->host, CQ("^"));
      else 
         chxbreak (list->next->str, result->name, result->host, CQ("^"));
      nlfree   (list);
      *error = 0;
      SUCCEED;
   }

   /*** If no matches, but we have a special case, just return the
   /    special case. 
   if (matches == 0  &&  special) {
      nlfree (list);
      *result = *pattern;
      *error  = 0;
      SUCCEED;
   }
*/
   /*** No matches at all. */
   if (matches == 0) {
      nlfree (list);
      FAIL;
   }

   /*** The rest of the code handles the case of multiple matches.  Display
   /    them to the user. */
   mdwrite   (XWTX, "ppl_Tchoose", null.md);
   sysbrkclear();
   for (p=list->next, i=0;   p!=null.nl;   p = p->next) {
      if (sysbrktest())  break;
      chxbreak (p->str, thisid, thishost, CQ("^"));

      /*** Matches to people on this host. */
      if (EMPTYCHX(thishost)  ||  chxeq (thishost, localhost))
         person_is (XWTX, thisid, 0, null.par, ++i);

      /*** Matches to people on another host.  */
      /*** This code section will eventually load whatever full name
      /    information we have for remote people.  For now, it just
      /    displays their userid and host name. */
      else {
/*       sprintf (mtu, "%s%2d%s   %s %s%s^%s%s\n", ss_lparen, ++i, ss_rparen, 
                        pattern->name, ss_luid, thisid, thishost, ss_ruid); */
         chxclear(newstr);
         chxformat (newstr, CQ("%s%2d%s   "), L(++i), L(0), ss_lparen, 
                    ss_rparen);
         chxformat (newstr, CQ("%s^%s\n"), L(0), L(0), pattern->name, thishost);
         unit_write (XWTX, newstr);
      }
   }

   /*** In the SPECIAL case of "name@host" with no userid information,
   /    also display that possibility as a numbered choice. */
   /*** This needs to be fixed when usernames are on remote hosts. */
   if (special) {
/* if (NULLSTR (pattern->id)) { */
/*    specnum = i + 1;
      sprintf (mtu, "%s%2d%s   %s^%s\n", ss_lparen, specnum, ss_rparen,
                                        pattern->name, pattern->host);
      unit_write (XWTX, mtu);
      mdwrite (XWTX, "ppl_Tspecial", null.md);
*/ }
   else  specnum = -1;

   unit_write (XWTX, CQ("\n"));

   /*** Finally, ask the user to choose one of the numbered selections. */
   while (1) {
      ok = mdprompter ("fnd_Plist", null.chx, 1, "", 5, answer, CQ("0")) >= -1;
      if (NOT ok  ||  NOT chxnum (answer, &person)  ||  person <= 0) {
         nlfree (list);
         *error = 0;
         FAIL;
      }
      
      if (person <= matches) {
         for (p=list->next, i=1;    i != person;   p=p->next, i++) ;
         if (special) chxbreak (p->str, result->name, result->host, CQ("^"));
         else         chxbreak (p->str, result->id,   result->host, CQ("^"));
         chxcpy (result->name, pattern->name);
         if (NOT EMPTYCHX (result->host)) chxcpy (result->id, null.chx);
         break;
      }

      if (person == specnum) {
         *result = *pattern;
         break;
      }

      mdwrite (XWTX, "fnd_Enumber", null.md);
   }

   nlfree (list);
   *error = 0;
   SUCCEED;

 done:

   chxfree ( cmiesc );
   chxfree ( localhost );
   chxfree ( answer );
   chxfree ( thisid );
   chxfree ( thishost );
   chxfree ( newstr );

   RETURN ( success );
}


/*** Does 'userid' exist as a Caucus user? */
FUNCTION  choose_exists (userid)
   Chix   userid;
{
   int    exists = 0;

   if (unit_lock (XUMD, READ, L(0), L(0), L(0), L(0), userid)) {
      exists = unit_check (XUMD);
      unit_unlk (XUMD);
   }
   return (exists);
}
