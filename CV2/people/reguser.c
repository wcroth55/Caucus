/*** REG_USER.  Register a user.
/
/    ok = reg_user (reg, captive, passwd)
/
/  Parameters:
/     Userreg  reg;     // Registration info for the user
/     Flag     captive; // Is this a captive user?
/     Chix     passwd;  // Captive password (optional)
/
/  Purpose:
/     The pre-registration feature needs a non-interactive method of
/     adding a user.  This function provides that 'silent' method.
/ 
/  How it works:
/     Just like is_registered(), reg_user() creates the appropriate
/     directories and adjusts other pieces of the Caucus database using
/     other functions like: sysdirchk() and modnames().
/
/     REG must have the following fields filled in:
/     * NAME
/     * LASTNAME
/     * ID
/     * SETOPTS (just the text of the string)
/     * PHONE (may be an empty, alloc'd Chix)
/
/     In addition, the following field will be stored if filled in:
/     * BRIEFS  (brief intro)
/
/     If PASSWD is an alloc'd, non-empty Chix, this user is a captive user,
/     and an entry will be added to the XSPD/N database.
/
/     SITEVAR is an array of 10 chixen.  Any of these which are
/     filled in will be stored in the user's XURG file.
/
/  Returns: 1 on success, 0 on failure.
/
/  Error Conditions:
/     Failure is usually caused by lack of disk space or permission problems
/     In this case we are left with an inconsistent database, which should
/     be cleaned up.  Failure can also be caused by reaching the total
/     limit on registered users (in the license block).
/
/  Related to:  is_registered()
/
/  Called by:
/
/  Home:  people/reguser.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  8/04/92 17:24 Create this. */
/*: JX  8/24/92 18:59 Add 2nd arg to storereg(). */
/*: CR 10/09/92 17:18 Remove unused 'exists'. */
/*: JV 12/04/92 13:44 Add sitevar arg to storereg(). */
/*: CL 12/10/92 17:06 Long ascofchx args. */
/*: CR  1/26/93 15:49 Use PLAIN and strplain() to decrypt license info. */
/*: CV  6/06/93 14:32 Use unit_check to determine XURG's existence. */
/*: CP  5/06/92 12:41 Add L(0) as 'item' arg to modnames() call. */
/*: CP  8/02/93 14:15 Does user exist?  Check XUDD before checking XURG. */
/*: CR 11/01/95 16:31 Add total # registered users limit. */

#include <stdio.h>
#include "caucus.h"

extern union  null_t           null;
extern struct flag_template    flag;
extern Chix   newuseropts, confid;
extern Chix   ss_luid, ss_ruid, sitevar[];
extern int    debug;

FUNCTION  reg_user (reg, captive, passwd)
   Userreg  reg;
   Flag     captive;
   Chix     passwd;
{
   struct namlist_t *add, *del, *nlnode(), *nlnames();
   Chix   str, name, pwstr, alert, opts;
   short  c;
   int    error, success, exists, total;
   int4   mycrypt();

   ENTRY ("reg_user", "");

   str     = chxalloc (L(80), THIN, "reguser str");
   name    = chxalloc (L(80), THIN, "reguser name");
   alert   = chxalloc (L(80), THIN, "reguser alert");
   pwstr   = chxalloc (L(80), THIN, "reguser pwstr");
   success = 0;

   /*** Validation: does this ID already exist? */
   exists = 0;
   if (unit_lock (XUDD, READ, L(0), L(0), L(0), L(0), reg->id)) {
      if (unit_check (XUDD)) {
         if (unit_lock (XURG, READ, L(0), L(0), L(0), L(0), reg->id)) {
            exists = unit_check (XURG);
            unit_unlk  (XURG);
         }
      }
      unit_unlk (XUDD);
   }
   if (exists) {
      chxformat  (alert,
                 CQ("User %s already exists.  Not created. Continuing.\n"),
                 L(0), L(0), reg->id, nullchix);
      unit_write (XITT, alert);
      FAIL;
   }

   if (debug) {
      chxformat (str, CQ("Adding user %s.\n"), L(0), L(0), reg->id, nullchix);
      unit_write (XITT, str);
   }

   /*** Add the " (userid)" part to the full name. */
   chxclear  (str);
   chxformat (str, CQ(" %s%s"), L(0), L(0), ss_luid, reg->id);
   chxformat (str, CQ("%s"), L(0), L(0), ss_ruid, nullchix);
   chxcat  (reg->name, str);

   /*** Set user-settable options. */
   if (EMPTYCHX (reg->setopts)) opts = nullchix;
   else                         opts = reg->setopts;
   decode_setopts (reg, opts, nullchix, newuseropts);

   /*** Create the user's personal Caucus directory(s).
   /    FLAG stuff is a kludge to tell sysdirchk to not ask the OS
   /    for the userid, but use the one in flag.userid instead. */
   flag.login = 1;
   ascofchx (flag.userid, reg->id, L(0), L(30));
   sysdirchk(reg->id);

   /*** Create and store user's registration file. */
   storereg (reg, 0, sitevar);

   /*** Create the user's personal mail directory. */
   if (NOT unit_lock (XUMD, WRITE, L(0), L(0), L(0), L(0), reg->id)) FAIL;
   if (NOT unit_make (XUMD))                { unit_unlk (XUMD);  FAIL; }
   unit_write (XUMD, CQ("0\n"));
   unit_close (XUMD);
   unit_unlk  (XUMD);

   /*** Add each word in this user's full name to the global nameparts
   /    directory. */
   del = nlnode  (1);
   chxpunct (name, reg->name);
   add = nlnames (name, reg->id);

   c = modnames (XSND, XSNN, nullchix, 0, L(0), add, del, NOPUNCT);
   nlfree (add);
   nlfree (del);
   if (NOT c)  FAIL;
  
   /*** If captive, add the password to the database. */
   if (captive) {
      chxformat (pwstr, CQ("%s %d"), mycrypt(passwd), L(0), reg->id, nullchix);
      add = nlnode(1);
      del = nlnode(1);
      nladd (add, pwstr, 0);
      modnames (XSPD, XSPN, nullchix, 0, L(0), add, del, NOPUNCT);
      nlfree (add);
      nlfree (del);
   }

   /*** If distributed, write change name transaction.  (What should be
   /    done if chgxperson() returns an error?) */
   /* 
      if (PLAIN(license.function_mask) & FM_DIST) {
         chgxperson (reg->name, nullchix, reg->id, reg->phone, &error);
      }
   */

   SUCCEED;

done:
   chxfree (str);
   chxfree (name);
   chxfree (alert);
   chxfree (pwstr);

   RETURN (success);
}
