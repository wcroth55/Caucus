
/*** FUNC_WRAP2HTML.  Intelligently reformat 'word-wrapped' text into HTML.
/
/    func_wrap2html (result, arg, conf, cmlargs, vars, form, 
/                            incvars, macs, cfile, tg, aref);
/   
/    Parameters:
/       Chix    result;  (append resulting value here)
/       Chix    arg;     (function argument)
/       SWconf  conf;    (server configuration info)
/       char   *cmlargs; (CML script arguments)
/       Vartab  vars;    (CML variable list)
/       Vartab  form;    (form data variable list)
/       Chix    incvars; (text of 'include' variables)
/       Vartab  macs;    (defined macros)
/       CML_File cfile;  (original cml file)
/       Target     tg;   (output target)
/       Chix     aref;   (array reference from $array_eval())
/
/    Purpose:
/       Output filter: intelligently reformat 'word-wrapped' text into
/       HTML.
/
/    How it works:
/       Acts as much as possible like a typical word-processor.
/       Each 'newline' is like a w.p. "hard" RETURN.  A single
/       newline translates into an HTML <BR>; two or more in a row
/       translate into sequences of "&nbsp;<P>".
/
/       Groups of N spaces translate into N-1 &nbsp;'s and one regular
/       space.  A tab acts like (N=5) spaces.
/
/       All (HTML) special characters are escaped, and all URL's are
/       processed to be "blued".
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funct2html.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/25/96 16:46 New function, based on func_t2html. */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */
/*: CR  9/11/02 Fix ISALNUM def to include '_'. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  ST_TEXT   0
#define  ST_MACRO  1
#define  ST_ARGS   2
#define  ISALNUM(x)  (x < 256  &&  (isalnum(x) || x=='_'))

FUNCTION  func_wrap2html (
   Chix    result, 
   Chix    arg,
   SWconf  conf,
   char   *cmlargs,
   Vartab  vars,
   Vartab  form,
   Chix    incvars,
   Vartab  macs,
   CML_File cfile,
   Target   tg,
   Chix     aref)
{
   static Chix line = nullchix;
   static Chix accum, interim, para, br, newl;
   int4   linepos, pos, cval, start;
   int    new_para, count, spaces, type, parens, state;
   ENTRY ("func_wrap2html", "");

   if (line == nullchix) {
      line    = chxalloc (L(100), THIN, "func_w2html line");
      interim = chxalloc (L(100), THIN, "func_w2html interim");
      accum   = chxalloc (L(100), THIN, "func_w2html accum");
      para    = chxalsub (CQ("&nbsp;<P>\n"), L(0), L(20));
      br      = chxalsub (CQ("<BR>"),        L(0), L(10));
      newl    = chxalsub (CQ("\n"),          L(0), L(10));
   }

   /*** Did last line start a new para? */
   new_para = 1;

   /*** Scan and process each line of text in ARG. */
   type = chxtype (arg);
   for (linepos=0L;   chxnextline (line, arg, &linepos);   ) {

      /*** Blank lines become <P>'s. */
      if (chxvalue (line, L(0)) == 0) {
         chxcat (result, para);
         new_para = 1;
         continue;
      }

      /*** Unless this is right after a new para (<P>), start
      /    with a line break (<BR>). */
      if (NOT new_para)  chxcat (result, br);
      new_para = 0;

      /*** Scan the LINE, and only apply special processing to text
      /    *outside* of macro invocations. */
      state = ST_TEXT;
      chxclear (accum);
      for (pos=0;   cval = chxvalue (line, pos);   ++pos) {

         /*** % probably starts a macro.  The args must NOT be
         /    processed specially. */
         if (state == ST_TEXT) {
            if (cval == '%')  { start = pos;   parens = 0;   state = ST_MACRO; }
            else                chxcatval (accum, type, cval);
         }

         else if (state == ST_MACRO) {
            if      (ISALNUM(cval)  ||  cval == ' ')  continue;
            else if (cval == '(')                     state = ST_ARGS;
            else {
               pos = start;
               chxcatval (accum, type, L('%'));
               state = ST_TEXT;
            }
         }

         else if (state == ST_ARGS) {
            if      (cval == ')'  &&  parens)  --parens;
            else if (cval == '(')              ++parens;
            else if (cval == ')') {
               wrap2html (result, accum, conf, cmlargs, vars, form, 
                                               incvars, macs, cfile, tg, aref);
               chxclear  (accum);
               chxcatsub (result, line, L(start), L(pos-start+1));
               state = ST_TEXT;
            }
         }
      }

      /*** Hit end of line while still scanning for macros? */
      if (chxvalue (accum, L(0)) != 0)
         wrap2html (result, accum, conf, cmlargs, vars, 
                            form, incvars, macs, cfile, tg, aref);

      if (state != ST_TEXT)
         chxcatsub (result, line, L(start), L(pos-start));
         
      chxcat (result, newl);
   }

   RETURN (1);
}
