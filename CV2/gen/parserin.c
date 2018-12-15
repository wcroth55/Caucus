
/*** PARSE_RIN.   Parse input redirection from command line.
/
/    parse_rin (red, command, unit);
/
/    Parameters:
/       Redirect *red;       (fill in this redirection object)
/       Chix      command;   (with information parsed from this command line)
/       int       unit;      (unit number to use for input redirection)
/
/    Purpose:
/       Parse input redirection information (e.g., "ADD ITEM >file")
/       from a command line, and fill in a Redirect object.
/
/    How it works:
/       Parses and removes "<file" field from COMMAND line.  Fills in 
/       members of Redirect:
/          unit     uses supplied 'unit' arg if redirection occurs.
/          inout    input(0), output(1), append(2)
/          fname    name of file
/
/       If no redirection (or an error) is found, sets the "unit" 
/       member to XKEY.
/
/       Parsing or permission errors (redirection not allowed) are
/       written directly to the user's terminal, unit XWER.
/
/    Returns: 1 if redirection was found
/             0 otherwise
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
/    Home:  gen/parserin.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  8/09/93 19:41 New function, based on old parse_inp(). */
/*: CK  8/20/93 14:33 aschofchx() requires int4 args. */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"
#include "redirect.h"

extern struct flag_template  flag;
extern union  null_t         null;
extern Chix                  ss_inred, ss_dotnl;

FUNCTION parse_rin (red, command, unit)
   Redirect  *red;
   Chix       command;
   int        unit;
{
   Chix   redblank, word, keepquote[2];
   int4   inred;
   int    success, u_comm, u_keyb;

   ENTRY ("parse_rin", "");

   word        = chxalloc (L(80),  THIN, "parseinp word");
   redblank    = chxalloc (L(20),  THIN, "parseinp redblank");
   keepquote[0]= chxalloc (L(160), THIN, "parseinp keepquote0");
   keepquote[1]= chxalloc (L(160), THIN, "parseinp keepquote1");
   success     = 0;

   red->unit     = XKEY;
   red->inout    = 0;
   red->locked   = 0;
   red->fname[0] = '\0';

   /*** Is the input redirection symbol ("<") here at all? */
   if (chxindex (command, L(0), ss_inred) < 0)  FAIL;

   /*** If so, it might be inside a quoted string.  Remove any such
   /    strings, but be ready to put them back. */
   chxquote (command, 0, keepquote);

   /*** Map "< file" into "<file" for convenience. */
   chxconc (redblank, ss_inred, CQ(" "));
   while   (chxalter (command, L(0), redblank, ss_inred)) ;

   /*** Find the input redirection symbol. */
   inred = chxindex (command, L(0), ss_inred);

   /*** If the redirection symbol is there, pluck out the redirection
   /    file name.  Put back any quoted strings.  Make sure that a
   /    file name was supplied! */
   if (inred >= 0)  syspluck (command, inred, word);
   chxquote (command, 1, keepquote);
   if (inred <  0)      FAIL;
   chxtrim (word);
   if (EMPTYCHX(word))  FAIL;

   /*** Make sure redirection is allowed. */
   u_comm = input_control (IC_ISUNIT | IC_COMM, nullchix);
   u_keyb = input_control (IC_ISUNIT | IC_SESS, nullchix);
   if (NOT (flag.redirect  ||  u_comm!=u_keyb  ||
              (flag.in_macro  &&  NOT flag.restrict_user))) {
      mdwrite (XWER, "err_Tinred", null.md);
      FAIL;
   }

   red->unit = unit;
   ascofchx (red->fname, word, L(0), L(198));
   SUCCEED;

 done:
   chxfree (redblank);
   chxfree (word);
   chxfree (keepquote[0]);
   chxfree (keepquote[1]);

   RETURN  (success);
}
