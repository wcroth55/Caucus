
/*** FUNC_T2HTML.  Intelligently(?) reformat text into HTML.
/
/    func_t2html (result, arg, conf, cmlargs, 
/                         vars, form, incvars, macs, cfile, tg, aref)
/   
/    Parameters:
/       Chix     result;  (append resulting value here)
/       Chix     arg;     (function argument)
/       SWconf   conf;    (server configuration info)
/       char    *cmlargs; (CML script arguments)
/       Vartab   vars;    (CML variable list)
/       Vartab   form;    (form data variable list)
/       Chix     incvars; (text of 'include' variables)
/       Vartab   macs;    (defined macros)
/       CML_File cfile;   (original cml file)
/       Target   tg;      (output target)
/       Chix     aref;    (array reference from $array_eval())
/
/    Purpose:
/       Output filter: intelligently(?) reformat plain text into
/       HTML.
/
/    How it works:
/       This is a pretty simple version:
/          Blank lines become <P>'s.
/
/          Indented lines force start of a new line, and each indenting
/          space is translated into a "&nbsp;".
/          
/          protocol://...  becomes a link to the URL (cool!)
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

/*: CR  6/23/95 16:28 New function. */
/*: CR 11/21/96 23:07 Simplify, use "&nbsp;" for indents. */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */
/*: CR  4/30/02 Remove call to t2hbr, t2url does it internally. */
/*: CR  9/11/02 Fix ISALNUM def to include '_'. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  ST_TEXT     0
#define  ST_MACRO    1
#define  ST_ARGS     2
#define  ISALNUM(x)  (x < 256  &&  (isalnum(x) || x=='_'))

FUNCTION  func_t2html (
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
   static Chix interim, accum, para, br, nbsp, newl;
   int4   pos, spaces, cval, start, linepos;
   int    count, last_para, this_para, state, parens, type;
   ENTRY ("func_t2html", "");

   if (line == nullchix) {
      line    = chxalloc (L(100), THIN, "func_t2html line");
      interim = chxalloc (L(100), THIN, "func_t2html interim");
      accum   = chxalloc (L(100), THIN, "func_t2html accum");
      para    = chxalsub (CQ("<P>\n"),  L(0), L(10));
      br      = chxalsub (CQ("<BR>"),   L(0), L(10));
      nbsp    = chxalsub (CQ("&nbsp;"), L(0), L(10));
      newl    = chxalsub (CQ("\n"),     L(0), L(10));
   }

   /*** Did last line start a new para? */
   last_para = 1;

   /*** Scan and process each line of text in ARG. */
   for (linepos=0L;   chxnextline (line, arg, &linepos);   ) {

      /*** Blank lines become <P>'s. */
      if ( (this_para = chxvalue (line, L(0)) == 0) )  {
         chxcat (result, para);
         last_para = this_para;
         continue;
      }

      /*** Everything else, we have to analyze more. */

      /*** Count up and leading (indenting) spaces, and replace
      /    them with "&nbsp;"s. */
      for (spaces=0;   chxvalue (line, spaces) == ' ';   ++spaces) ;
      if  (spaces) {
         if (NOT last_para)  chxcat (result, br);
         for (count=0;   count < spaces;   ++count)   chxcat (result, nbsp);
      }

      /*** Scan the LINE, and only apply special processing to text
      /    *outside* of macro invocations. */
      state = ST_TEXT;
      chxclear (accum);
      type = chxtype (line);
      for (pos=spaces;   cval = chxvalue (line, pos);   ++pos) {

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
               chxcpy (interim, accum);

               /*** Use func_t2url() to detect and translate URL's. */
               func_t2url (interim, conf, cmlargs, 
                           vars, form, incvars, macs, cfile, tg, aref);
               chxcat     (result, interim);
               chxcatsub  (result, line, L(start), L(pos-start+1));
               chxclear   (accum);
               state = ST_TEXT;
            }
         }
      }

      /*** Hit end of line while still scanning for macros? */
      if (chxvalue (accum, L(0)) != 0) {
         chxcpy     (interim, accum);
         func_t2url (interim, conf, cmlargs, 
                              vars, form, incvars, macs, cfile, tg, aref);
         chxcat     (result, interim);
      }

      if (state != ST_TEXT)
         chxcatsub (result, line, L(start), L(pos-start));

      chxcat (result, newl);

      /*** Prepare for next loop. */
      last_para = this_para;
   }

   RETURN (1);
}
