
/*** IS_REGISTERED.  Is this user registered?
/
/    ok = is_registered (reg);
/
/    Parameters:
/       int      ok;          (is user registered now?)
/       Userreg  reg;         (user registration struct)
/       
/    Purpose:
/       Load this user's registration file.  If the user is not registered,
/       go through the (interactive) process of registering the user.
/
/    How it works:
/
/    Returns: 1 on success
/             0 aborted by user (or internal failure)
/            -1 too many users already registered
/
/    Error Conditions:
/ 
/    Known bugs:      none
/
/    Home:  people/isregist.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:49 Source code master starting point */
/*: CR 10/12/88 11:16 Disallow " " as a user's name! */
/*: AM  2/07/89 10:31 Jcaucus changes*/
/*: JV  9/19/89 16:45 Added _register_user AFM. */
/*: CW  5/18/89 15:36 Add XW unit codes. */
/*: CW  7/21/89 15:34 echoinput() brief introduction. */
/*: CR  8/29/89 16:37 Check sysudnum() before trying loadreg(). */
/*: CR 10/17/89 15:17 Replace 2nd arg of FIRE_ARM with "". */
/*: CR 10/24/89 13:23 Add 3rd diagnostic argument to sysudnum(). */
/*: CR  3/19/90 12:24 Use textenter() to get brief introduction. */
/*: CR  3/26/90 20:46 Clear XWTX after brief intro text entered. */
/*: CR  4/23/90  1:10 Add missing arg to nlcat(). */
/*: CR  6/11/90 13:43 Registration succeeds even if no brief intro. */
/*: CR  8/12/90 15:02 Add "(userid)" as part of full name of person. */
/*: CR  9/26/90 12:11 Make Lint happy when JAPAN is OFF. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  7/28/91 17:16 Use addxperson() instead of chgxperson(). */
/*: JV  7/29/91 12:59 Use chgxperson() instead of addxperson(). */
/*: CR  7/30/91 18:10 Add 3rd arg to loadreg(). */
/*: CR  8/02/91 12:44 Add NOPUNCT as new last arg to modnames() call. */
/*: CR 11/18/91 17:20 Add CNUM argument to modnames() call. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: CR  1/07/92 15:10 Use new decode_setopts(). */
/*: CR  1/09/92 13:41 Use new sys opt stuff. */
/*: CX  3/17/92 15:44 Replace FIRE_AFM with fire_afm(). */
/*: DE  4/13/92 11:46 Chixified */
/*: JX  5/26/92 17:48 Remove JAPAN. */
/*: CR  8/06/92 17:23 Do not reformat intro text; chg arg to textenter(). */
/*: CI 10/05/92 16:07 Chix Integration. */
/*: CR  1/26/93 15:49 Use PLAIN and strplain() to decrypt license info. */
/*: CP  5/06/92 12:41 Add L(0) as 'item' arg to modnames() call. */
/*: CP  8/18/93 17:59 Loading this user must set att_ui_type. */
/*: CP 11/01/93 16:38 Creating this user must also set att_ui_type. */
/*: CR 10/30/95 12:43 Add total # users check, change return code. */

#include <stdio.h>
#include "caucus.h"

extern struct screen_template  screen;
extern struct flag_template    flag;
extern union  null_t           null;
extern Chix                    newuseropts, confid;
extern Chix   ss_inprom, ss_luid, ss_ruid;
extern Chix   afm_name;
extern Chix   sitevar[];
extern int    debug;

extern Chix att_ui_type;

FUNCTION  is_registered (reg)
   Userreg  reg;
{
   struct namlist_t *add, *del, *nlnode(), *nlnames();
   Chix   userid, str, nextconf, name;
   short  c;
   int    error, success, total;

   ENTRY ("is_registered", "");

   str       = chxalloc (L(80), THIN, "isregistered str");
   name      = chxalloc (L(80), THIN, "isregistered name");
   userid    = chxalloc (L(40), THIN, "isregistered userid");
   nextconf  = chxalloc (L(40), THIN, "isregistered nextconf");
   success   = 0;

   /*** If this user is already registered, return success. */
   sysuserid (userid);
   if (sysudnum (userid, -1, 0) > 0  &&  loadreg (userid, reg, sitevar)) {
      debug = reg->debug;
      screen.lines = reg->lines;
      screen.width = reg->width;
      chxcpy (att_ui_type, reg->terminal);
      SUCCEED;
   }

   /*** Otherwise, we must go through the whole registration process.
   /    Note that the user can abort registration at any point. */
   mdwrite (XWTX, "reg_Tintro", null.md);

   /*** Get the user's full name, and make sure it's not in use. */
   while (1) {
      c = mdprompter ("reg_Pname", null.chx, 0, "reg_Hname", 50, reg->name,
                      null.chx);
      if (c <= -3)  DONE(0);
      if (c == -2)  continue;

      /*** Remove additional blanks, and make sure the result isn't null. */
      chxsimplify (reg->name);
      if (NOT EMPTYCHX (reg->name))  break;
   }
   chxcpy (reg->id, userid);

   /*** Get the user's "lastname", i.e. one we alphabetize by. */
   if (NOT lastname (reg))  DONE(0);

   /*** Add the "(userid)" part to the full name. */
   chxclear  (str);
   chxformat (str, CQ(" %s%s"), L(0), L(0), ss_luid, userid);
   chxformat (str, CQ("%s"), L(0), L(0), ss_ruid, null.chx);
   chxcat  (reg->name, str);

   /*** Get the user's telephone number. */
   if (mdprompter ("reg_Pphone", null.chx, 0, "reg_Hphone", 38, reg->phone,
                   null.chx) <= -3)   DONE(0);

   /*** Set user-settable options to system defaults. */
   decode_setopts (reg, null.chx, null.chx, newuseropts);
   screen.lines = reg->lines;
   screen.width = reg->width;
   chxcpy (att_ui_type, reg->terminal);

   /*** Create the user's personal Caucus directory(s). */
   sysdirchk (userid);

   /*** Get a brief introduction from the user. */
   unit_write (XWTX, NEWCPAGE);
   mdwrite (XWTX, "reg_Tdescribe", null.md);
   nlfree (reg->briefs);
   reg->briefs = nlnode(1);

   if (NOT unit_lock (XITX, WRITE, L(0), L(0), L(0), L(0), null.chx))    DONE(0);
   textenter (XITX, 1, 0, 0, 1);

   unit_write (XWTX, NEWCPAGE);
   mdwrite    (XWTX, "reg_Twait", null.md);

   if (unit_view (XITX)) {
      while (unit_read  (XITX, str, L(0)))   nlcat (reg->briefs, str, 0);
      unit_close (XITX);
      unit_kill  (XITX);
   }
   unit_unlk  (XITX);

   /*** Set the user's printcode instructions to the system default. */
   nlfree (reg->print);
   reg->print = nlnode (4);

   /*** Create the user's personal mail directory. */
   if (NOT unit_lock (XUMD, WRITE, L(0), L(0), L(0), L(0), userid))      DONE(0);
   if (NOT unit_make (XUMD))                { unit_unlk (XUMD);  DONE(0); }
   unit_write (XUMD, CQ("0\n"));
   unit_close (XUMD);
   unit_unlk  (XUMD);

   /*** Add each word in this user's full name to the global nameparts
   /    directory. */
   del = nlnode  (1);
   chxpunct (name, reg->name);
   add = nlnames (name, userid);

   c = modnames (XSND, XSNN, null.chx, 0, L(0), add, del, NOPUNCT);
   nlfree (add);
   nlfree (del);
   if (NOT c)  DONE(0);
  
   /*** If distributed, write change name transaction.  (What should be
   /    done if chgxperson() returns an error?) */
   /* 
      if (PLAIN(license.function_mask) & FM_DIST) {
         chgxperson (reg->name, null.chx, userid, reg->phone, &error);
      }
   */

   /* Finally fire off AFM. */
   fire_afm ("_register_user", null.chx, nextconf);

   SUCCEED;

done:
   chxfree (str);
   chxfree (name);
   chxfree (userid);
   chxfree (nextconf);

   RETURN  (success);
}
