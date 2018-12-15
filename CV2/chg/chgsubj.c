
/*** CHG_SUBJ.   Handle CHANGE SUBJECT command.
/
/    The syntax for this command is:
/      CHANGE SUBJECT [name]
/ or   CHANGE SUBJECT "name"  [+/-  [item-range] ]
/
/    where + means add item-range to the subject, and - means remove.
/    CHG_SUBJ prompts for any missing parts of the command. 
/
/    ARGS is the list of arguments on the command line, starting
/         immediately after CHANGE SUBJECT.
/
/    MINE is 1 for MYSUBJECT, 0 for SUBJECT.  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:34 Source code master starting point */
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: CR  7/13/90 14:25 Use thisuser->is_organizer. */
/*: CR  7/23/91 16:19 Use subchange(), which in turn calls chg_dsubj(). */
/*: CX 11/10/91 19:16 Chixify. */
/*: CR 10/29/91 11:45 Add confnum arg to getline2() call. */
/*: CR 11/07/91 13:55 Add confname to subchange() call. */
/*: CR  1/02/92 14:41 Add master.chgsubj. */
/*: JX  5/19/92 14:51 Fix Chix. */
/*: CI 10/05/92 12:03 Chix Integration. */
/*: CR 10/11/92 21:50 Remove CQ from (md)prompter() call. */
/*: CL 12/10/92 15:22 Long chxindex args. */

#include <stdio.h>
#include "caucus.h"

extern struct master_template master;
extern Userreg                thisuser;
extern union  null_t          null;
extern short                  confnum;
extern Chix                   confname;

FUNCTION  chg_subj (args, mine)
   Chix   args;
   int    mine;
{
   static Chix s_plus  = nullchix;
   static Chix s_minus = nullchix;
   Chix   keep[2], plus, minus, name, sign, range, owner, empty;
   short  quoted, pplus, pminus, subject, is_plus, is_minus, i;
   short  usd, usf, utd, utf;
   int    success;
   Chix   line;
   Chix   getline2();

   ENTRY ("chg_subj", "");

   empty   = chxalloc (L(0), THIN, "chgsubj empty");
   keep[0] = chxalloc (L(0), THIN, "chgsubj keep1");
   keep[1] = chxalloc (L(0), THIN, "chgsubj keep2");
   sign    = chxalloc (L(0), THIN, "chgsubj sign");
   range   = chxalloc (L(0), THIN, "chgsubj range");
   owner   = chxalloc (L(0), THIN, "chgsubj owner");
   plus    = chxalloc (L(0), THIN, "chgsubj plus");
   minus   = chxalloc (L(0), THIN, "chgsubj minus");
   name    = chxalloc (L(0), THIN, "chgsubj name");
   success = 0;
   mdstr (plus,  "chg_Kplus",  &s_plus);
   mdstr (minus, "chg_Kminus", &s_minus);

   /*** Setup: unit numbers, strings, userid. */
   if (mine) {
      usd = XUND;   usf = XUNN;          utd = XUSD;   utf = XUSF;
      sysuserid (owner);
   }
   else {
      usd = XCND;   usf = XCNN;          utd = XCSD;   utf = XCSF;
   }

   if (NOT mine  &&  NOT thisuser->is_organizer && NOT master.chgsubj) {
      mdwrite (XWER, "cus_Tonlyorg", null.md);
      goto done;
   }

   /*** If no ARGS at all, use blank line. */
   if (args == null.chx)  args = empty;

   /*** The parsing section of this code plucks out the subject NAME, the
   /    SIGN (add or remove) and the item RANGE.  Any parts not found are
   /    set to an empty string, and will be prompted for later. */

   quoted = chxquote (args, 0, keep);

   /*** Finding a quoted string implies CHANGE SUBJECT "name" [+/- itemrange].
   /    Parse the rest of the line. */
   if (quoted) {
      if ( (pplus  = chxindex (args, L(0), plus) ) < 0)  pplus  = 999;
      if ( (pminus = chxindex (args, L(0), minus)) < 0)  pminus = 999;

      /*** If + or - found, break ARGS into NAME and RANGE parts. */
      if (pplus < pminus) {
         chxbreak (args, name, range, plus); 
         chxcpy   (sign, plus);
      }
      else if (pminus < pplus) {
         chxbreak (args, name, range, minus); 
         chxcpy   (sign, minus);
      }

      /*** Otherwise, take the whole line as NAME. */
      else   chxcpy (name, args);

      /*** Finally, replace the quoted string, and strip away the quotes. */
      chxquote (name, 1, keep);
      chxstrip (name);
      chxsimplify (sign);
      chxsimplify (range);
   }

   /*** If no quoted string, just use the whole line as the NAME. */
   else  chxcpy (name, args);


   /*** The parsing is complete.  Now look at NAME, SIGN, and RANGE.
   /    Prompt for each if empty. */
   if (EMPTYCHX(name)) {
      if (mdprompter ("chg_Psubnam", null.chx, 0, "chg_Hsubnam", 
                        80, name, null.chx) < -1)  goto done;
      chxstrip (name);
   }

   /*** Find the number of the subject that uniquely matches NAME. */
   subject = submatch (usd,usf, utd,utf, owner, confnum, confname, name);
   if (subject  < 0) {  mdwrite (XWER, "chg_Esubj2",  null.md);   goto done; }
   if (subject == 0) {  mdwrite (XWER, "chg_Enosubj", null.md);   goto done; }

   /*** If we're doing this interactively, display subject name and
   /    attached item numbers. */
   if (EMPTYCHX (sign)) {
      for (i=1;
           (line = getline2 (utd, utf, confnum, owner, 0, subject, i,
                            (Textbuf) NULL)) != nullchix;
           ++i) {
         unit_write (XWTX, line);
         unit_write (XWTX, NEWLINE);
      }
   }


   /*** We now have the subject clearly identified.  Next, see if SIGN
   /    and RANGE are specified.  If no SIGN, prompt for both add and
   /    remove RANGEs.  Otherwise, prompt for RANGE if needed. */
   is_plus  = chxeq (sign, plus);
   is_minus = chxeq (sign, minus);

   if (EMPTYCHX (range)) {
      if ( (EMPTYCHX(sign)  ||  is_plus)   &&
           mdprompter ("chg_Psubadd", nullchix, 0, "chg_Hsubadd",
                        80, range, null.chx) > -2) {
         subchange (name, confnum, confname, owner, 0, range);
         if (NOT mine)  subuncat (1, range, subject);
      }

      if ( (EMPTYCHX(sign)  ||  is_minus)  &&
           mdprompter ("chg_Psubrem", null.chx, 0, "chg_Hsubrem", 
                         80, range, null.chx) > -2)
         subchange (name, confnum, confname, owner, 1, range);
   }
   else {
      if (is_plus)     subchange (name, confnum, confname, owner, 0, range);
      if (is_minus)    subchange (name, confnum, confname, owner, 1, range);
      if (is_plus && NOT mine)  subuncat (1, range, subject);
   }
   success = 1 ;

done:
   chxfree (name);
   chxfree (minus);
   chxfree (plus);
   chxfree (owner);
   chxfree (range);
   chxfree (sign);
   chxfree (keep[0]);
   chxfree (keep[1]);
   chxfree (empty);
   RETURN  (success);
}
