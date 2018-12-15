
/*** PARSE_ROUT.   Parse output redirection from command line.
/
/    parse_rout (red, command);
/
/    Parameters:
/       Redirect *red;       (fill in this redirection object)
/       Chix      command;   (with information parsed from this command line)
/
/    Purpose:
/       Parse output redirection information (e.g., "LIST ALL >file")
/       from a command line, and fill in a Redirect object.
/
/    How it works:
/       Parses and removes ">file", ">>file", ">@", "PRINT", and "-PRINT" 
/       fields from COMMAND line.  Fills in members of Redirect:
/          unit     unit number
/          inout    input(0), output(1), append(2)
/          fname    name of file (if ">file" or ">>file" forms)
/
/       If no redirection (or error) is found, sets the "unit" member to XWTX.
/
/       Parsing or permission errors (redirection not allowed) are
/       written directly to the user's terminal, unit XWER.
/
/    Returns: 1 on successful parse
/             0 if couldn't make sense of redirection
/
/    Error Conditions:
/ 
/    Side effects: may write to user's terminal.
/
/    Related functions:
/
/    Called by:  Caucus UI functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  gen/parserou.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  8/09/93 15:43 New function, based on old parse_output(). */
/*: CK  8/20/93 14:33 aschofchx() requires int4 args. */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"
#include "redirect.h"

extern struct flag_template  flag;
extern Userreg               thisuser;
extern struct namlist_t     *sysmgrprint;
extern union  null_t         null;
extern Chix                  yesnotable;
extern Chix                  ss_outred, ss_appred;

FUNCTION  parse_rout (red, command)
   Redirect *red;
   Chix      command;
{
   static Chix s_print1, s_print2, s_bb;
   struct namlist_t *printptr;
   short  redirect=0, append=0, print=0, dshprint=0;
   Chix   word;
   Chix   outred, appred, print1, print2, bucket;
   Chix   keep[2];
   short  i;
   int    success, u_comm, u_keyb;
   int4   pos;

   ENTRY ("parse_rout", "");

   red->unit     = XWTX;
   red->inout    = 1;
   red->locked   = 0;
   red->fname[0] = '\0';

   keep[0]   = chxalloc (L(80),  THIN, "parseout keep0");
   keep[1]   = chxalloc (L(80),  THIN, "parseout keep1");
   word      = chxalloc (L(80),  THIN, "parseout word");
   outred    = chxalloc (L(10),  THIN, "parseout outred");
   appred    = chxalloc (L(10),  THIN, "parseout appred");
   print1    = chxalloc (L(30),  THIN, "parseout print");
   print2    = chxalloc (L(30),  THIN, "parseout print2");
   bucket    = chxalloc (L(20),  THIN, "parseout bucket");
   success   = 0;

   /*** Remove any quoted strings, but be prepared to put them back later. */
   chxquote (command, 0, keep);
  
   /*** Map "> file" into ">file" for convenience. */
   chxconc (outred, ss_outred, CQ(" "));
   chxconc (appred, ss_appred, CQ(" "));
   while   (chxalter (command, L(0), outred, ss_outred)) ;
   while   (chxalter (command, L(0), appred, ss_appred)) ;

   /*** Look at each word in COMMAND for the telltale ">" or "PRINT". */
   mdstr (print1, "gen_Kprint1",  &s_print1);
   mdstr (print2, "gen_Kprint2",  &s_print2);
   mdstr (bucket, "gen_Kbitbuck", &s_bb);
   for (i=2;   (pos = chxtoken (word, null.chx, i, command)) != -1;   ++i) {

      redirect = (chxindex (word, L(0), ss_outred) == 0);
      append   = (chxindex (word, L(0), ss_appred) == 0);
      print    = (chxeq    (word, print1));
      dshprint = (chxeq    (word, print2));

      if (redirect || append || print || dshprint)  break;
   }

   /*** Remove the redirection file name (or PRINT modifier) from the command
   /    line, and put it into WORD. */
   if (pos >= 0)  syspluck (command, pos, word);
   chxquote (command, 1, keep);
   if (pos <  0  ||  EMPTYCHX(word))  FAIL;

   /*** Complain if redirection is not allowed. */
   u_comm = input_control (IC_ISUNIT | IC_COMM, nullchix);
   u_keyb = input_control (IC_ISUNIT | IC_SESS, nullchix);
   if (redirect  &&  NOT (flag.redirect  ||  u_comm!=u_keyb  ||
                         (flag.in_macro  &&  NOT flag.restrict_user))) {
      mdwrite (XWER, "err_Toutred", null.md);
      FAIL;
   }

   /*** If PRINT, set up for printing. */
   red->unit = XIRE;
   if (chxeq (word, bucket))  red->unit = XIBB;
   if (print  ||  dshprint) {
      if (NOT flag.print) {
         mdwrite (XWER, "gen_Enoprint", null.md);
         FAIL;
      }

      red->inout = 2;
      printptr = (flag.print == 1  ||  thisuser->print->next == null.nl
                    ? sysmgrprint : thisuser->print);
      chxcpy (word, (printptr->next)->str);
      red->unit = XIPT;
   }
   if (append) red->inout = 2;

   ascofchx (red->fname, word, L(0), L(198));

 done:
   chxfree (word);
   chxfree (keep[0]);
   chxfree (keep[1]);
   chxfree (outred);
   chxfree (appred);
   chxfree (print1);
   chxfree (print2);
   chxfree (bucket);

   RETURN  (success);
}
