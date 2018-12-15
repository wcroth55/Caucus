
/*** PARSE_INPUT.   Parse input redirection from command line.
/
/    input = parse_input (str, unit);
/
/    Parameters:
/       int    input;       (XKEY or 'unit')
/       Chix   str;         (command line)
/       int    unit;        (unit number to use if redirection)
/
/    Purpose:
/       Parse a command line (such as "ADD ITEM <FILE") for an input
/       redirection string, and open the appropriate file.
/
/    How it works:
/       Looks for (and removes) input redirection indicators ("<file",
/       "< file").  Lock the file on UNIT, if found.
/
/    Returns: 'unit' if redirection found and file opened ok
/             XKEY   otherwise
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  gen/parseinp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: CW  5/18/89 14:25 Add XW unitcodes. */
/*: CR  5/01/90 15:08 Allow input redirect if in <file. */
/*: CR  9/28/90 16:19 Make locking/reading of input file more robust. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: DE  4/01/92 12:16 Chixified */
/*: JX  5/27/92 15:20 Fixify. */
/*: CX  6/05/92 15:34 Replace XKEY with u_keybd. */
/*: CR  4/18/92 16:41 Add processing of flag.restrict_user. */
/*: CR  6/09/92 14:16 Add 2nd "unit" argument, test for empty file name. */
/*: CI 10/05/92 16:36 Chix Integration. */
/*: CR 10/15/92 12:29 Use new version of syspluck(). */
/*: JV 10/15/92 09:03 Parse file redirection properly. */
/*: TP 10/19/92 14:23 Make compatible with input_control(). */
/*: CR 10/21/92 11:11 Merge TP and 2.4 versions. */
/*: CL 12/10/92 14:52 Long chxalter args. */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: CP  6/27/93 20:29 Change header comments.  Does NOT return w file open! */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"

extern struct flag_template  flag;
extern union  null_t         null;
extern Chix                  ss_inred, ss_dotnl;

FUNCTION parse_input (str, unit)
   Chix  str;
   int   unit;
{
   Chix   redblank, word, keepquote[2];
   int4   inred;
   int    pass, success, u_comm, u_keyb;

   ENTRY ("parseinput", "");

   word        = chxalloc (L(80),  THIN, "parseinp word");
   redblank    = chxalloc (L(20),  THIN, "parseinp redblank");
   keepquote[0]= chxalloc (L(160), THIN, "parseinp keepquote0");
   keepquote[1]= chxalloc (L(160), THIN, "parseinp keepquote1");
   success     = 0;

   /*** Is the input redirection symbol ("<") here at all? */
   if (chxindex (str, L(0), ss_inred) < 0)  DONE(XKEY);

   /*** If so, it might be inside a quoted string.  Remove any such
   /    strings, but be ready to put them back. */
   chxquote (str, 0, keepquote);

   /*** Map "< file" into "<file" for convenience. */
   chxconc (redblank, ss_inred, CQ(" "));
   while   (chxalter (str, L(0), redblank, ss_inred)) ;

   /*** Find the input redirection symbol. */
   inred = chxindex (str, L(0), ss_inred);

   /*** If the redirection symbol is there, pluck out the redirection
   /    file name.  Put back any quoted strings.  Make sure that a
   /    file name was supplied! */
   if (inred >= 0)  syspluck (str, inred, word);
   chxquote (str, 1, keepquote);
   if (inred <  0)      DONE(XKEY);
   chxtrim (word);
   if (EMPTYCHX(word))  DONE(XKEY);

   /*** Make sure redirection is allowed. */
   u_comm = input_control (IC_ISUNIT | IC_COMM, nullchix);
   u_keyb = input_control (IC_ISUNIT | IC_SESS, nullchix);
   if (NOT (flag.redirect  ||  u_comm!=u_keyb  ||
              (flag.in_macro  &&  NOT flag.restrict_user))) {
      mdwrite (XWER, "err_Tinred", null.md);
      DONE(XKEY);
   }

   /*** Lock the file and return the unit number to the caller. */
   if (   pass = unit_lock (unit, READ, L(0), L(0), L(0), L(0), word)) {
      if (pass = unit_view (unit))  unit_close (unit);
      else                          unit_unlk  (unit);
   }
   if (NOT pass) {
      mdwrite    (XWER, "err_Tnofile", null.md);
      unit_write (XWER, word);
      unit_write (XWER, ss_dotnl);
      DONE(XKEY);
   }

   DONE(unit);

 done:

   chxfree (redblank);
   chxfree (word);
   chxfree (keepquote[0]);
   chxfree (keepquote[1]);

   RETURN  (success);
}
