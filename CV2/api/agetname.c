/*** A_GET_NAMEX.   Fetch list of item/resp's indexed by names.
/
/    ok = a_get_namex (namex, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Namex     namex;    (object to fill with name index info)
/       int4      mask;     (mask of properties to get)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Given the partial name of a name index, fetch the list of
/       item/resp numbers that are indexed under that name.
/
/    How it works:
/       MASK selects what kind of name index to scan:
/          P_TITLE   for item titles
/          P_NAME    for item authors (userids)
/          P_SUBJECT for subject categories
/          P_ISINCE  for items added since the specified date
/          P_RSINCE  for responses added since the specified date
/
/       Caller must fill in members NAMEPART, CNUM, and OWNER (the latter
/       only for psubjects) to select the particular named index.  If 
/       there is more than one title/author/subject/etc match, a_get_namex()
/       fills the RLIST member with the item/resp list for all matches.
/
/       Normally a_get_namex() empties namex->rlist, and just places
/       the results in it.  If, however, MASK contains the property
/       bit P_AND, a_get_namex() "and"s the contents of namex->rlist
/       with the result. 
/
/       The P_AND property bit may be used with multiple calls to
/       a_get_namex() to select items by multiple properties (for 
/       example: in the Caucus command interface syntax, 
/       LIST ITEM TITLE "XYZ" AUTHOR "SMITH").
/
/       The exact meaning of the contents of NAMEPART vary according to MASK:
/           P_TITLE   any part of an item title
/           P_SUBJECT any part of a subject category name
/           P_NAME    an initial substring of any *userid* of a participant.
/           P_ISINCE  a date ("-N" days, or "mm/dd/yy" or "dd-mmm-yyy")
/           P_RSINCE  a date (ditto)
/
/    Returns: A_OK       on success
/             A_BADARG   bad arguments
/             A_NOCONF   no such conference
/             A_NOREAD   user not allowed in this conf
/             A_NOTOWNER trying to access other user's psubjects
/             A_NONE     no name indices matched
/             A_DBERR    database error
/
/    Error Conditions:
/
/    Side effects: 
/
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/agetname.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/15/93 21:31 New function. */
/*: CR  2/23/93 22:23 Clear PREV at the end of each rlist block. */
/*: CR  2/23/93 22:36 Clarify comments about NAMEPART. */
/*: CR  2/28/93 22:39 Get tbuf from a_cch_tbuf(). */
/*: CK  3/01/93 16:51 sysmem requires int4 arg. */
/*: CN  5/15/93 17:11 Use new item selection codes. */
/*: CP  5/17/93 21:31 Add P_ISINCE, P_RSINCE. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "unitcode.h"
#include "maxparms.h"
#include "textbuf.h"
#include "handicap.h"
#include "done.h"

#define  RLNULL   ( (Rlist) NULL)

FUNCTION  a_get_namex (Namex namex, int4 mask, int wait)
{
   Namelist thisname, matches, this, nlnames(), nlfind();
   Rlist    rthis, ptr;
   int      usd, usf, utd, utf;
   int      ok, access, additem, chgresp, chgsubj, success, is_psub, lno;
   int      item, maxitems, i, and;
   int4     tag, subtract, date, match;
   short   *select = (short *) NULL;
   short    parms[6];
   Chix     cname, temp, userid;
   Chix     line, getline2();
   Textbuf  tbuf, a_cch_tbuf();
   int4     systime(), time_of_date();

   /*** Check argument validity, clear list of matching namexect names. */
   if (namex == (Namex) NULL  ||  namex->tag != T_NAMEX)    return (A_BADARG);
   if (EMPTYCHX(namex->namepart))                           return (A_BADARG);
   if (namex->cnum <= 0)                                    return (A_NOCONF);

   /*** And'ing with an empty Rlist returns an empty Rlist! */
   if ( (mask & P_AND)  &&  (namex->rlist->next == RLNULL)) return (A_OK);

   cname  = chxalloc (L(30), THIN, "agetsu cname");
   temp   = chxalloc (L(20), THIN, "agetsu temp");
   userid = chxalloc (L(20), THIN, "agetnamex userid");
   maxitems = MAXITEMS;

   /*** This user can only work with their own psubjects. */
   is_psub = (NOT EMPTYCHX (namex->owner))  &&  (mask & P_SUBJECT);
   sysuserid (userid);
   if (is_psub  &&  NOT chxeq (userid, namex->owner))  DONE (A_NOTOWNER);

   /*** Make sure this user can read this conference. */
   if (NOT a_cache (V_GET, namex->cnum, &access, &additem, &chgresp, 
                    &chgsubj, cname))        DONE (A_NOCONF);
   if (access < 1)                           DONE (A_NOREAD);

   /*** Select unit numbers of namelist partfiles to scan, depending
   /    on MASK and the OWNER member. */
   if      (is_psub)          { usd=XUND;  usf=XUNN;  utd=XUSD;  utf=XUSF; }
   else if (mask & P_SUBJECT) { usd=XCND;  usf=XCNN;  utd=XCSD;  utf=XCSF; }
   else if (mask & P_TITLE)   { usd=XCTD;  usf=XCTN;  }
   else if (mask & P_NAME)    { usd=XCAD;  usf=XCAN;  }
   else if (mask & P_ISINCE)  ;
   else if (mask & P_RSINCE)  ;
   else                       DONE (A_BADARG);

   /*** Prepare an array of "items selected" for temporary work. */
   select = (short *) sysmem (sizeof(short) * L(MAXITEMS+1), "select");
   for (item=0;   item<=MAXITEMS;   ++item)  select[item] = -1;

   /*** Named ranges call 'matchnames()' to find the item list. */
   if (mask & (P_SUBJECT | P_TITLE | P_NAME)) {

      /*** Actually scan the relevant namelist partfiles for matches. */
      thisname = nlnames (namex->namepart, nullchix);
      and = (mask & P_NAME ? 0 : 1);
      ok = matchnames (usd, usf, namex->cnum, namex->owner, thisname, 
                       and, &matches, NOPUNCT);
      if (NOT ok)                { nlfree (matches);   DONE (A_DBERR);  }
      if (nlsize (matches) == 0) { nlfree (matches);   DONE (A_NONE); }
   
      /*** Scan the matches.  Add the appropriate items to SELECT. */
      tbuf  = (Textbuf) NULL;
      for (this=matches->next;   this!=(Namelist) NULL;   this=this->next) {
   
         /*** For P_NAME, we want *exact* matches against the userids. */
         if (mask & P_NAME) {
            chxtoken (temp, nullchix, 1, this->str);
            if (nlfind (thisname, temp) == NLNULL)  continue;
         }

         /*** Find the tag for each match. */
         chxtoken (temp, nullchix, 2, this->str);
         chxnum   (temp, &tag);
   
         /*** For author and title indices, the tag *is* the item number. */
         if ( (mask & P_NAME)  ||  (mask & P_TITLE) ) {
            select[tag] = 0;
            continue;
         }
   
         /*** The remainder of the loop handles subjects, where the TAG is
         /    the subject number. */
   
         /*** See if that subject is in this conference. */
         if (tbuf == (Textbuf) NULL)
            tbuf = a_cch_tbuf (V_GET, utd, utf, namex->cnum, 0, (int) tag, 
                                      namex->owner);
         line = getline2 (utd, utf, namex->cnum, namex->owner, 0, 
                             (int) tag, 0, tbuf);
         if (line == nullchix)         continue;
         chxtoken (temp, nullchix, 2, line);
         if (NOT chxeq (temp, cname))  continue;
   
         for (lno=2;   (line = getline2 (utd, utf, namex->cnum, namex->owner,
                           0, (int) tag, lno, tbuf)) != nullchix;
              lno++) {
            parsenum (line, select, MAXITEMS, 0);
         }
      }
      nlfree (matches);
      nlfree (thisname);
   }

   /*** For both P_ISINCE and P_RSINCE, parse the DATE, and get the
   /    number of items in this conference. */
   if (mask & (P_ISINCE | P_RSINCE)) {

      /*** Look for "-N" days syntax; otherwise parse full mm/dd/yy or
      /    dd-mmm-yyy date. */
      chxtoken (temp, nullchix, 1, namex->namepart);
      if (chxvalue (temp, L(0)) == '-') {
         chxnum (temp, &subtract);
         date = systime() + subtract * L(24) * L(3600);
      }
      else date = time_of_date (temp, nullchix);
   
      /*** Get the # of items in the conf directly; read the 1st two
      /    lines of the XCMA file for this conference. */
      if (unit_lock (XCMA, READ, L(namex->cnum), L(0), L(0), L(0), nullchix)) {
         if (unit_view (XCMA)) {
            unit_read  (XCMA, temp, L(0));
            unit_short (XCMA, parms, 4);
            unit_close (XCMA);
            maxitems = parms[0];
         }
         unit_unlk (XCMA);
      }
   }

   /*** For P_SINCE, add to SELECT all those items after (including)
   /    the matched date. */
   if (mask & P_ISINCE) {
      ok = matchdate (XCED, XCEN, namex->cnum, L(0), date, &match);
      if  (NOT ok  ||  match == -1)  match = maxitems + 1;
      for (i=match;   i<=maxitems;   ++i)  select[i] = 0;
   }

   /*** P_RSINCE means items with responses entered after the date,
   /    and only those responses.  Note MATCH == -1 means no matches. */
   if (mask & P_RSINCE) {

      /*** Set SELECT to the list of items we're going to examine for
      /    a response since DATE.  (If the caller specified P_AND,
      /    use the information in namex->rlist *now* to make that list
      /    of items as small as possible.) */
      if (mask & P_AND) {
         for (ptr=namex->rlist->next;   ptr!=RLNULL;   ptr=ptr->next)
            for (i=ptr->i0;   i<=ptr->i1;    ++i)  select[i] = 0;
      }
      else  for (i=1;         i<=maxitems;   ++i)  select[i] = 0;

      /*** Find the response-since MATCH for each selected item. */
      for (i=1;   i<=maxitems;   ++i)  if (select[i] >= 0) {
         ok = matchdate (XCFD, XCFN, namex->cnum, L(i), date, &match);
         if      (NOT ok  ||  match == -1)  select[i] = -1;
         else if (match > select[i] )        select[i] = match;
      }
   }

   /*** Convert "select" marked list of items into rlist RTHIS. */
   rthis = rl_of_select (select, maxitems);

   /*** If caller requested it, "and" namex->rlist and RTHIS. */
   if (mask & P_AND) {
      Rlist  and;
      and = and_rlist (namex->rlist, rthis);
      a_fre_rlist (rthis);
      rthis = and;
   }

   a_fre_rlist (namex->rlist);
   namex->rlist = rthis;

   namex->ready = 1;
   DONE (A_OK);

done:
   if (select != (short *) NULL)  sysfree ( (char *) select);
   chxfree (cname);
   chxfree (temp);
   chxfree (userid);
   return  (success);
}
