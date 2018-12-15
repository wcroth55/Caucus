
/*** CHG_PASS.  Let user change his/her password. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:33 Source code master starting point */
/*: CR  1/20/89 16:08 Map passwords into lower case. */
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: JV  2/23/90 05:00 Changed sysecho() to turn_echo(). */
/*: CR  4/09/90 17:28 Replace turn_echo() with systurn(). */
/*: CR  9/11/90 15:29 Use MAXPASS. */
/*: CJ 11/22/90 22:35 Use new form of systurn() call. */
/*: CJ 11/26/90 16:48 Add OS_ECHO to systurn(). */
/*: CR  8/02/91 12:44 Add NOPUNCT as new last arg to modnames() call. */
/*: CX 11/10/91 18:22 Chixify. */
/*: CR 11/18/91 17:20 Add CNUM argument to modnames() call. */
/*: JX  5/19/92 13:06 Fix Chix. */
/*: CI 10/05/92 11:30 Chix Integration. */
/*: JV 10/11/92 14:37 Chxclear before chxformat. */
/*: CR 10/11/92 20:57 Don't use CQ in mdprompter() call. */
/*: CP  5/06/92 12:41 Add L(0) as 'item' arg to modnames() call. */

#include <stdio.h>
#include "caucus.h"

#define   PROMPT(p,h,a)      mdprompter (p, null.chx, 0, h, MAXPASS, a, empty)
#define   FINISHED           goto finished

extern struct flag_template flag;
extern struct termstate_t   term_in_caucus;
extern union  null_t        null;

FUNCTION  chg_pass()
{
   struct namlist_t *add, *del, *nlnode();
   Chix   pw1, pw2, pw3, pwstr, id, empty;
   short  c, success;
   char  *error;
   int4   pass, mycrypt();

   ENTRY ("chgpass", "");

   if (NOT EMPTYCHX (flag.webpasswd)) {
      mdwrite (XWTX, "chg_Pnoten", null.md);
      RETURN  (0);
   }

   success = 0;
   error   = null.str;

   pw1   = chxalloc (L(30), THIN, "chg_pass pw1");
   pw2   = chxalloc (L(30), THIN, "chg_pass pw2");
   pw3   = chxalloc (L(30), THIN, "chg_pass pw3");
   id    = chxalloc (L(30), THIN, "chg_pass id");
   pwstr = chxalloc (L(30), THIN, "chg_pass pwstr");
   empty = chxalloc (L(30), THIN, "chg_pass empty");

   /*** If not using the login shell, let the system change the password. */
   if (NOT flag.login) { success = syspass();  FINISHED; }

   /*** With echo off, get the old password (to verify this is the real
   /    user), and then get the new password twice (to make sure it's
   /    typed correctly).  An interrupt cancels the new password entry. */
   unit_write (XWTX, NEWCPAGE);
   systurn    (&term_in_caucus, CAUCUS_ECHO + OS_ECHO, OFF);
   c =             PROMPT ("chg_Poldpass",  "chg_Holdpass",  pw1);
   if (c > -3) c = PROMPT ("chg_Pnewpass",  "chg_Hnewpass",  pw2);
   if (c > -3) c = PROMPT ("chg_Pnewpass2", "chg_Hnewpass2", pw3);
   systurn    (&term_in_caucus, CAUCUS_ECHO + OS_ECHO, ON);

   unit_write (XWTX, NEWLINE);
   jixreduce  (pw2);
   jixreduce  (pw3);

   if (c < -2)                 FINISHED;
   error = "chg_Tnomatch";
   if (NOT chxeq (pw2, pw3))   FINISHED;

   /*** Find this user and the user's encrypted password. */
   sysuserid (id);
   pwfind    (id, &pass);

   /*** If the user knew the old password, go ahead and change it. */
   error = "chg_Tbadpass";
   if (mycrypt (pw1) != pass) {
      jixreduce (pw1);
      if (mycrypt (pw1) != pass)  FINISHED;
   }

   add = nlnode(1);
   del = nlnode(1);
   chxformat (pwstr, CQ("%s %d"), mycrypt(pw1), L(0), id, null.chx);
   nladd     (del, pwstr, 0);
   chxclear  (pwstr);
   chxformat (pwstr, CQ("%s %d"), mycrypt(pw2), L(0), id, null.chx);
   nladd     (add, pwstr, 0);
   modnames  (XSPD, XSPN, nullchix, 0, L(0), add, del, NOPUNCT);
   nlfree    (add);
   nlfree    (del);
   success = 1;
   error   = null.str;

finished:
   mdwrite (XWTX, error, null.md);
   mdwrite (XWTX, (success ? "chg_Tpaschg" : "chg_Tpasfail"), null.md);
   chxfree (pw1);
   chxfree (pw2);
   chxfree (pw3);
   chxfree (id);
   chxfree (pwstr);
   chxfree (empty);
   RETURN  (1);
}
