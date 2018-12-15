
/*** CHG_INTRO.   Change user's brief introduction.
/
/    Returns 1 on success, 0 on failure (in which case the
/    introduction is left unchanged. 
/
/    Known bugs:
/       Unclear as to who should really close unit IN.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:33 Source code master starting point */
/*: JV  9/06/89 10:45 Added number parameter to person_is call. */
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: CW  7/21/89 15:58 Echoinput() input text line. */
/*: CR  3/19/90 14:19 Add code for Edit/Delete/Replace introduction. */
/*: CR  3/24/90 13:54 Iseditreq() needs table of edit synonymns. */
/*: CR  4/24/90 20:13 Remove unused 'c' variable. */
/*: CR  4/24/90 21:18 Declare strtoken(). */
/*: CR  8/12/90 20:21 Cast textenter/textedit arg to make Lint happy. */
/*: CR  4/08/91 18:48 Add new unit_lock argument. */
/*: CX 11/10/91 17:39 Chixify. */
/*: UL 12/11/91 18:41 Make unit_lock args 3-6 int4.*/
/*: JX  5/19/92 12:09 Fix Chix. */
/*: CR  8/07/92 13:01 Allow CHANGE INTRO <file. */
/*: CI 10/05/92 10:52 Chix Integration. */
/*: JV 12/04/92 13:38 Add sitevar arg to storereg(). */
/*: CK  8/20/93 14:15 Change arg 'in' to redirection arg 'red'. */

#include <stdio.h>
#include "caucus.h"
#include "redirect.h"

extern Userreg               thisuser;
extern union  null_t         null;
extern Chix   ss_inprom,
              sitevar[];

FUNCTION  chg_intro (red)
   Redirect *red;
{
   struct  namlist_t *intro, *b, *nlnode();
   static  Chix       itemtab;
   Chix    def, answer, editor, str;
   int     in;
   short   select, ok;

   ENTRY ("chgintro", "");

   def    = chxalloc (L(10), THIN, "chg_intro def");
   str    = chxalloc (L(90), THIN, "chg_intro str");
   answer = chxalloc (L(10), THIN, "chg_intro answer");
   editor = chxalloc (L(90), THIN, "chg_intro editor");

   if (red->unit != XKEY) {
      intro = nlnode (4);
      ok    = 1;
      apply_redirect (red);
      in    = red->unit;
   }
   else {
      /*** Display the old introduction... */
      unit_write (XWTX, NEWCPAGE);
      mdwrite    (XWTX, "chg_Tbrief0",  null.md);
      person_is  (XWTX, thisuser->id, 1, null.par, 0);
      unit_write (XWTX, NEWLINE);
   
      mdtable ("chg_Aitem", &itemtab);
   
      /*** Find out what the user wants to do to the brief introduction. */
      chxtoken (def, nullchix, 1, itemtab);
      select = mdprompter ("chg_Pedr", itemtab, 2, "chg_Hintro", 60, answer, def);
   
      /*** Failed, <cancel>, or STOP goes home. */
      ok = 0;
      if (select<0  ||  select==3)  goto done;
   
      /*** Prepare to Delete, Edit, or Replace. */
      if (NOT unit_lock (XITX, WRITE, L(0), L(0), L(0), L(0), nullchix))  goto done;
      unit_kill (XITX);
      intro = nlnode (4);
      ok    = 1;
   
      /*** Delete introduction. */
      if      (select == 2)  ;
   
      /*** Edit introduction.  Put text in XITX and edit it. */
      else if (iseditreq (answer, def, thisuser->editor, editor)) {
   
         /*** Copy the text of the item into XITX. */
         if (ok = unit_make (XITX)) {
            for (b=thisuser->briefs->next;   b!=null.nl;   b = b->next) {
               unit_write (XITX, b->str);
               unit_write (XITX, NEWLINE);
            }
            unit_close (XITX);
      
            /*** Edit the text in XITX. */
            ok = textedit (XITX, editor, (int) thisuser->truncate);
         }
      }
   
      /*** If REPLACE, just ENTER the text again. */
      else {
         mdwrite (XWTX, "chg_Treplace", null.md);
         ok = textenter (XITX, (int) thisuser->truncate, 0, 0, 0);
      }
      in = XITX;
   }
   
   /*** Copy XITX back into the introduction. */
   if (ok  &&  unit_view (in)) {
      while (unit_read (in, str, L(0)))  nlcat (intro, str, 0);
      unit_close (in);
   }

   if (in == XITX) {
      unit_kill (XITX);
      unit_unlk (XITX);
   }

   /*** Rewrite the user registration file with the new introduction... */
   if (ok) {
      unit_write (XWTX, NEWCPAGE);
      nlfree (thisuser->briefs);
      thisuser->briefs = intro;
      storereg (thisuser, 1, sitevar);
   } else nlfree (intro);

done:
   chxfree (str);
   chxfree (def);
   chxfree (answer);
   chxfree (editor);
   RETURN  (ok);
}
