/*** A_CHG_NAMEX.   Change list of item/resp's in a name index.
/
/    ok = a_chg_namex (namex, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Namex     namex;    (object to containing new name index info)
/       int4      mask;     (mask of properties to change)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Find a uniquely specified name index, and replace the
/       contents of its Rlist.
/
/    How it works:
/       Currently, MASK can only be P_SUBJECT.
/
/       Caller must fill in members NAMEPART, CNUM, and OWNER (the latter
/       only for psubjects) to select the particular named index.  The
/       contents of member RLIST replace the list of items in that index.
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
/       It is not clear who should be responsible for modifying the
/       "uncategorized items" category.  Should the API do it, or
/       should the UI do it?  Currently a_chg_namex() does not
/       touch "uncategorized items".  Does a_add_resp() do anything
/       to that subject when a new item is added?  And what about Naomi?
/
/    Home:  api/achgname.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/15/93 21:31 New function. */
/*: CR  2/23/93 22:59 Include handicap.h. */
/*: CR  2/28/93 22:03 Use cache'd tbuf. */
/*: JV  6/02/93 11:00 Clear cache on exit. */
/*: CN  5/15/93 17:11 Use new item selection codes. */
/*: CP  7/14/93 15:52 Really, really, clear the tbuf cache on exit. */
/*: CR  5/14/04 Use co_priv_str() to check access levels. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "unitcode.h"
#include "maxparms.h"
#include "textbuf.h"
#include "handicap.h"
#include "done.h"

#define  S_IGNORE -1
#define  S_ADD     0
#define  S_REM     2

FUNCTION  a_chg_namex (Namex namex, int4 mask, int wait)
{
   Rlist    rthis;
   int      usd, usf, utd, utf, i, subject, error;
   int      access, additem, chgresp, chgsubj, success, is_psub, lno;
   char     sn[20];
   short   *select;
   Chix     cname, userid, addsub, remsub;
   Chix     line, getline2();
   Textbuf  tbuf, a_cch_tbuf();

   if (disk_failure (0, &error, NULL))                    return (A_DBERR);

   /*** Check argument validity. */
   if (namex == (Namex) NULL  ||  namex->tag != T_NAMEX)  return (A_BADARG);
   if (EMPTYCHX(namex->namepart))                         return (A_BADARG);
   if (mask != P_SUBJECT)                                 return (A_BADARG);
   if (namex->cnum <= 0)                                  return (A_NOCONF);

   cname  = chxalloc (L(30), THIN, "achgsu cname");
   userid = chxalloc (L(20), THIN, "achgsu userid");
   addsub = chxalloc (L(80), THIN, "achgnamex addsub");
   remsub = chxalloc (L(80), THIN, "achgnamex remsub");

   /*** This user can only work with their own psubjects. */
   is_psub = (NOT EMPTYCHX (namex->owner));
   sysuserid (userid);
   if (is_psub  &&  NOT chxeq (userid, namex->owner))  DONE (A_NOTOWNER);

   /*** Make sure this user can read this conference. */
   if (NOT a_cache (V_GET, namex->cnum, &access, &additem, &chgresp,
                    &chgsubj, cname))                  DONE (A_NOCONF);
   if (access < 1)                                     DONE (A_NOREAD);
/* if (NOT is_psub  &&  NOT chgsubj  &&  access < 3)   DONE (A_NOWRITE); */
   if (NOT is_psub  &&  NOT chgsubj  &&  
                 access < co_priv_str("instructor"))   DONE (A_NOWRITE);

   /*** Select unit numbers of namelist partfiles to scan, depending
   /    on OWNER member. */
   if (is_psub)  { usd=XUND;  usf=XUNN;  utd=XUSD;  utf=XUSF; }
   else          { usd=XCND;  usf=XCNN;  utd=XCSD;  utf=XCSF; }

   /*** Find the number of the subject that uniquely matches NAME. */
   subject = submatch (usd,usf, utd,utf, namex->owner, namex->cnum,
                       cname, namex->namepart);
   if (subject  < 0) DONE (A_NOTUNIQ);
   if (subject == 0) DONE (A_NONE);

   /*** The UI application expects to call a_chg_namex() with a
   /    completely new Rlist of items that will replace the old
   /    Rlist for this subject.  However, for historical (and
   /    CaucusLink related) reasons, changes to the database for
   /    a subject index are applied as lists of *changes*, i.e. items
   /    to add or items to remove.  (Instead of simply replacing an
   /    old list with a new list, which might seem more logical.)
   /
   /    This means that we have to compare the new Rlist against the
   /    old list of items for this subject, figure the changes, and
   /    then apply those changes.  */

   /*** Transform the old subject item list into an array of flags "select",
   /    0=item selected, -1=otherwise. */
   tbuf   = a_cch_tbuf (V_GET, utd, utf, namex->cnum, 0, subject, namex->owner);
   select = (short *) sysmem ( sizeof(short) * L(MAXITEMS+1), "select");
   for (lno=2;   (line = getline2 (utd, utf, namex->cnum, namex->owner,
                     0, subject, lno, tbuf)) != nullchix;
        lno++) {
      parsenum (line, select, MAXITEMS, 0);
   }

   /*** Now compare the new RLIST against FLAGS, and set each array
   /    element to S_ADD (for add this item) or S_REM (for remove
   /    this item. */
   /*** (This version only handles item #s, and ignores response #s.) */
   for (rthis=namex->rlist->next;   rthis!=(Rlist) NULL;   rthis=rthis->next) {
      for (i=rthis->i0;   i<=rthis->i1;   ++i) {
         if      (select[i]== -1)  select[i] = S_REM;
         else if (select[i]==  0)  select[i] = S_IGNORE;
      }
   }

   /*** Build the strings of items to add or remove. */
   for (i=1;   i<MAXITEMS;   ++i) {
      if (select[i]==S_ADD) { sprintf(sn, " %d", i);  chxcat(addsub, CQ(sn)); }
      if (select[i]==S_REM) { sprintf(sn, " %d", i);  chxcat(remsub, CQ(sn)); }
   }
   sysfree ( (char *) select);
  
   /*** Add or remove the items as appropriate. */
   if (NOT EMPTYCHX(addsub))
      subchange (namex->namepart, namex->cnum, cname, namex->owner, 0, addsub);

   if (NOT EMPTYCHX(remsub))
      subchange (namex->namepart, namex->cnum, cname, namex->owner, 1, remsub);

/* subuncat (1, range, subject); */

   tbuf->type   = 0;
   namex->ready = 1;
   DONE (A_OK);

done:
   a_cch_tbuf (V_CLR, utd, utf, namex->cnum, 0, subject, namex->owner);
   chxfree (cname);
   chxfree (userid);
   chxfree (addsub);
   chxfree (remsub);
   return  (success);
}
