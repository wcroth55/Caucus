/*** GROUPMATCH.  Is USERID in this group file?
/
/    ok = groupmatch (file, groups, userid, wildcard, permnames, searchall);
/
/    Parameters:
/       int      ok;        (Success?)
/       Chix     file;      (name of group file)
/       Namelist groups;    (history of previous matches against GROUP files)
/       Chix     userid;    (looking for match against this userid)
/       Chix     wildcard;  (string used for wildcard matching)
/       Chix     permnames; (permission names & bit values)
/       int      searchall; (1=>search all entries, 0=>just find first match)
/
/    Purpose:
/       Does the "groups file" FILE contain a match for USERID?
/       If so, return the (optional) permission bits for that match.
/
/    How it works:
/       Groupmatch() first verifies that FILE actually resides on
/       this host.  (FILE may contain an "@host" after the file
/       name proper.  If it's not on this host, return failure.)
/
/       Groupmatch then scans the namelist GROUPS to see if this
/       FILE has already been scanned for this userid.  If so, the
/       previous result is used.
/
/       Otherwise, groupmatch() proceeds to read FILE and scan it for a
/       match for USERID.  WILDCARD contains the string used for wildcard 
/       matching, i.e. if WILDCARD contains "*", then userid "ABCD" matches
/       FILE entry "AB*".  The result of this test is saved in GROUPS.
/
/       If a 2nd word appears on the matched line, it is interpreted
/       as a number (either directly or via the PERMNAMES list), and the
/       value is used as the result of the match.  If there is no 2nd word,
/       the value is 1.
/
/       FILE may contain references to other files, e.g. "<subfile".
/       GROUPMATCH calls itself recursively to handle these cases.
/       If the file references itself, GROUPMATCH knows to avoid
/       infinite recursion.
/
/    Returns: 
/       Value of match found (1 or numeric value if 2nd word appears as above)
/       0 if no match.
/
/       If SEARCHALL is 1, returns logical "or" of *all* matches.
/       Otherwise only the value of the first match found is returned.
/
/    Error Conditions:
/  
/    Known bugs:      none
/
/    Home:  conf/groupmat.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/17/90  9:45 New function. */
/*: CR  7/24/90 11:59 Avoid infinite recursion. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  4/25/91  1:11 Lower-case and trim trailing blanks from XSUG entries.*/
/*: CR  6/20/91 17:01 Use new equal_id() function. */
/*: CR  7/24/91 21:46 Remove unused variables. */
/*: CR 12/09/91 16:58 Handle FILE = "name@host"; expand header. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: JX  3/05/92 11:28 Chixify. */
/*: DE  3/26/92 16:40 Patched chxindex w/ 0, */
/*: DE  5/26/92 13:33 Fix Chixification */
/*: JX  8/24/92 18:56 Fixify. */
/*: JV 10/09/92 15:09 Robustify. */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CR 05/20/00 19:54 Sub-files retain original capitalization! */
/*: CR 10/16/01 13:50 Add permnames, searchall args & functionality. */
/*: CR 03/12/04 Match with no 2nd word = 1; otherwise use value only. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "done.h"

extern union null_t null;
extern Chix         ss_inred;

FUNCTION  groupmatch (file, groups, userid, wildcard, permnames, searchall)
   Chix   file, userid, wildcard, permnames;
   struct namlist_t  *groups;
   int    searchall;
{
   struct namlist_t *searchlist, *f, *g, *nlnode();
   Chix   name, filename, host, thishost, format, message, temp, origname,
          perm, groupfile;
   int    match, lock, open, success, permdex, found, value;
   Flag   nullgroups;

   ENTRY ("groupmatch", "");

   host      = chxalloc (L(100), THIN, "host");
   name      = chxalloc (L(200), THIN, "name");
   format    = chxalloc (L(100), THIN, "format");
   message   = chxalloc (L(200), THIN, "message");
   filename  = chxalloc (L(100), THIN, "filename");
   temp      = chxalloc (L(40),  THIN, "temp");
   perm      = chxalloc (L(40),  THIN, "perm");
   origname  = chxalloc (L(40),  THIN, "origname");
   groupfile = chxalloc (L(40),  THIN, "groupfile");
   thishost  = chxalloc (L(100), THIN, "thishost");

   if (groups == null.nl) {      /* Some ungrateful ingrate forgot to  */
      groups = nlnode (1);       /* pass us a namelist.  Make our own! */
      nullgroups = 1;
   } else nullgroups = 0;

   /*** If FILE is "filename@host", is "host" *this* host? */
   if (chxbreak (file, filename, host, CQ("@"))) {
      getmyhost (thishost);
      if (NOT chxeq (host, thishost))  FAIL;
   }

   /*** If this FILE is already in the GROUPS history, return
   /    the match value stored with it. */
   for (f=groups->next;   f!=null.nl;   f = f->next) {
      if (chxeq (f->str, filename))  {success = f->num; goto done;}
   }

   /*** If we can neither lock nor read the FILE, then complain, and
   /    add it to the GROUPS history with a match value of FALSE. */
   if (lock = unit_lock (XSUG, READ, L(0), L(0), L(0), L(0), filename)) 
         open = unit_view(XSUG);
   else  open = 0;

   if (NOT lock  ||  NOT open) {
      nlcat  (groups, file, 0);
/*    mdstr  (format, "mai_Fnogroup", null.md);
      chxformat  (message, format, L(0), L(0), file, null.chx);
      unit_write (XWER, message);
*/
      if (lock)  unit_unlk (XSUG);
      FAIL;
   }

   /*** Since FILE is not in GROUPS, we have to do the dirty work of
   /    actually looking through it.  SEARCHLIST is a list of subfiles
   /    that appear in FILE.  We don't actually search any of them until
   /    we're finished with FILE. */
   searchlist = nlnode (1);
   match      = 0;
   while (unit_read (XSUG, temp, L(0))  &&  (NOT match  ||  searchall)) {
      if (chxvalue (temp, L(0)) == L('#'))  continue;
      chxcpy    (origname, temp);
      jixreduce (temp);
      chxtrim   (temp);
      chxtoken  (name, nullchix, 1, temp);
      chxtoken  (nullchix, perm, 2, temp);

      /*** If "<subfile" redirection, add "subfile" to the search list. */
      if (chxindex (name, L(0), ss_inred) == 0) {
         chxtoken (groupfile, nullchix, 1, origname);
         chxclear (temp);
         chxcatsub(temp, groupfile, chxlen(ss_inred), L(200));
         nlcat (searchlist, temp, groupmatch_perm (perm, permnames));
         continue;
      }

      /*** Was this line a MATCH?  Allow for a wildcard at the end, e.g.
      /   "roth" matches "ro*" and everyone matches "*". */
      if (equal_id (name, userid, wildcard)) {

         /*** If there's a 2nd (etc) word, look for permission matches. */
         value = groupmatch_perm (perm, permnames);
         match = match | (value == 0 ? 1 : value);
      }
   }

   unit_close (XSUG);
   unit_unlk  (XSUG);

   /*** If we hit a match (and not searchall), we need to look no further. */
   if (match  &&  NOT searchall) {
      nlcat  (groups, filename, 1);
      nlfree (searchlist);
      DONE (match);
   }

   /*** Temporarily add this file to the groups history list with a 
   /    result of 0.  This avoids infinite recursion if the file 
   /    somehow references itself.   Save the pointer to this entry
   /    in G, we'll just correct the result later. */
   nlcat (groups, filename, 0);
   for (g=groups->next;   g!=null.nl;   g = g->next)
      if (chxeq (g->str, filename))  break;

   /*** Otherwise, recursively look at the files added to this layer of the
   /    searchlist.   Included groups with a specified perm overrides
   /    whatever perm the group itself contained. */
   for (f=searchlist->next;   f!=null.nl;   f=f->next) {
      value = groupmatch (f->str, groups, userid, wildcard, permnames, 
                          searchall);
      if (f->num > 0  &&  value > 0)  match = match | f->num;
      else                            match = match | value;
      if (match  &&  NOT searchall)  break;
   }
      
   /*** Success or no, add this file to the GROUPS history list. */
   nlfree (searchlist);
   g->num  = match;
   success = match;

done:
   chxfree (host);
   chxfree (name);
   chxfree (format);
   chxfree (message);
   chxfree (filename);
   chxfree (temp);
   chxfree (perm);
   chxfree (origname);
   chxfree (groupfile);
   chxfree (thishost);
   if (nullgroups) nlfree (groups);
   RETURN (success);
}

FUNCTION int groupmatch_perm (Chix perms, Chix permnames) {
   int   match, found;
   int4  pos, value;
   Chix  temp;

   if (EMPTYCHX (perms))   return (0);

   temp  = chxalloc (L(40),  THIN, "groupmatch_perm temp");
   match = 0;

   for (pos=0;   chxnextword (temp, perms, &pos) >= 0;   ) {
      if      (chxnum (temp, &value))  match = match | value;
      else if ( (found = tablefind (permnames, temp)) >= 0) {
         chxtoken (temp, nullchix, found+2, permnames);
         if (chxnum (temp, &value))    match = match | value;
      }
   }

   chxfree (temp);
   return  (match);
}
