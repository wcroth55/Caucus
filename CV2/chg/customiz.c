
/*** CUSTOMIZE.   Allow Organizer to customize conference-specific things.
/
/    Returns 1 on nominal success (action may not have succeeded, but
/    nothing broke!), or 0 on strange (unexpected) failure. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:34 Source code master starting point */
/*: CR  9/30/88 16:16 Replace unit_copy() with direct copying code. */
/*: CR 11/08/88 15:45 Left out unit_make(out) in direct copy code. */
/*: CR 12/06/88 22:23 Strlower 2nd word in COMLINE. */
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: CR  9/06/89 14:17 Add CUSTOMIZE VISIBLE. */
/*: CR 12/02/89 18:33 Output cus_Hnoarg to XWTX. */
/*: CR  7/13/90 14:27 Use thisuser.is_organizer. */
/*: CR  7/17/90 12:25 If editing files, unlock XITX. */
/*: CR  7/24/90 12:12 Parse XCUS after edit. */
/*: CR  7/26/90 22:50 Use new copyfile(). */
/*: CR  8/12/90 19:10 Cast editfile() arg to make lint happy. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  7/28/91 16:28 Use cus_dtext() and cusxtext(). */
/*: CR  7/30/91  0:29 Call chixfile with XITX. */
/*: CR  7/30/91 13:51 Replace license check with is_distributed(). */
/*: CR 10/29/91 12:53 Add confnum arg to allowed_on() call. */
/*: CC 11/05/91 16:23 Temporary hack to allowed_on() call. */
/*: JV 11/13/91 22:56 Add dest_chn arg to cusxtext(). */
/*: CR 10/30/91 22:37 Remove XCUS stuff to cus_user(). */
/*: CR 11/19/91 17:41 Use equal_id() to check for primary org. */
/*: UL 12/11/91 18:41 Make unit_lock args 3-6 int4.*/
/*: CR  1/02/92 14:46 Handle CUS SUBJECT. */
/*: DE  3/16/92 16:57 Chixified - success->pass, ok->success (For done.h)*/
/*: JX  5/19/92 20:35 Fix Chix. */
/*: JX  8/03/92 10:10 CHN's are chix. */
/*: CR  7/13/92 17:43 Allow change, add, subject to take ANSWER arg. */
/*: CI 10/05/92 12:43 Chix Integration. */
/*: CR 10/12/92 13:48 Fix Dan-isms, remove tempchix. */
/*: CR  1/13/93 12:50 Add chxwrap() after chixfile(). */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"

extern Userreg                thisuser;
extern union  null_t          null;
extern short                  confnum;

FUNCTION  customize (comline)
   Chix   comline;
{
   static Chix custab;
   int    what, which, out, pass, success, error1, error2;
   Chix   str, answer, newstr, format;
   Chix   ctext;

   ENTRY ("customize", "");

   str      = chxalloc (L(202), THIN, "customiz str");
   answer   = chxalloc (L(40),  THIN, "customiz answer");
   ctext    = chxalloc (L(500), THIN, "customiz ctext");
   newstr   = chxalloc (L(80),  THIN, "customiz newstr");
   format   = chxalloc (L(80),  THIN, "customiz format");
   success  = 0;

   mdtable ("cus_Atab", &custab);

   if (NOT thisuser->is_organizer) {
      mdwrite (XWER, "cus_Tonlyorg", null.md);
      SUCCEED;
   }

   unit_write (XWTX, NEWCPAGE);
   if (chxtoken (str, null.chx, 2, comline) < 0) {
      mdwrite (XWTX, "cus_Hnoarg", null.md);
      SUCCEED;
   }
   jixreduce (str);

   chxtoken (answer, null.chx, 3, comline);

   what = tablematch (custab, str);
   switch (what) {

      case ( 0):
      case (-1):
         mdstr      (format, "cus_Fbadcom", null.md);
         chxformat  (newstr, format, L(0), L(0), str, null.chx);
         unit_write (XWER, newstr);
         SUCCEED;

      case (-2):  ambiguous (str);   SUCCEED;

      case ( 1):  out = XCGR;   which = CUS_GREET;    break;
      case ( 2):  out = XCIN;   which = CUS_INTRO;    break;
      case ( 3):  cus_user();                         SUCCEED;
      case ( 4):  neworganizer();                     SUCCEED;
      case ( 5):  cus_change  (answer);               SUCCEED;
      case ( 6):  cus_enter   (answer);               SUCCEED;
      case ( 7):  cus_visible (answer);               SUCCEED;
      case ( 8):  cus_subject (answer);               SUCCEED;
      default:                                        FAIL;
   }

   /*** Copy the designated FILE into a temporary, editable file. */
   if (NOT copyfile (out, 0,0,0,null.chx,  XITX, 0,0,0,null.chx)) FAIL;

   /*** Edit the temporary file. */
   if (NOT unit_lock (XITX, WRITE, L(0), L(0), L(0), L(0), null.chx))  FAIL;
   pass = editfile (XITX, thisuser->editor, (int) thisuser->truncate);
   if (NOT pass)             { unit_unlk (XITX);   SUCCEED; }

   /*** Conference Greeting or Introduction changes must be made
   /    via cus_dtext() and cusxtext(). */
   success = chixfile (XITX, ctext, null.str);
   chxwrap (ctext, 77);
   unit_unlk (XITX);

   if (success)  success = cus_dtext (confnum, which, ctext, &error1);
   if (success  &&  is_distributed (confnum))
      success = cusxtext (confnum, which, ctext, nullchix, &error2);

 done:
   chxfree (str);
   chxfree (answer);
   chxfree (ctext);
   chxfree (newstr);
   chxfree (format);

   RETURN  (success);
}
