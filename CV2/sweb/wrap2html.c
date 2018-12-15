
/*** WRAP2HTML.  Intelligently reformat 'word-wrapped' text into HTML.
/
/    wrap2html (result, arg, conf, cmlargs, vars, form, incvars, 
/               macs, cfile, tg, aref);
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
/       Target   tg;     (output target)
/       Chix     aref;   (array reference from $array_eval())
/
/    Purpose:
/       Sub-function of func_wrap2html(); does the actual work
/       of processing word-wrapped text "intelligently" into HTML.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  wrap2html.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/25/96 16:46 New function, based on func_t2html. */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */
/*: CR  4/30/02 Remove call to t2hbr(), t2url() does it internally. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  wrap2html (
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
   static Chix temp = nullchix;
   static Chix nbsp;
   int4   pos, cval;
   int    count, spaces, type;

   ENTRY ("wrap2html", "");

   if (temp == nullchix) {
      temp    = chxalloc (L(100), THIN, "func_w2html temp");
      nbsp    = chxalsub (CQ("&nbsp;"),      L(0), L(10));
   }

   type = chxtype (arg);


   /*** Use func_t2url() to detect and translate URL's. */
   chxcpy     (temp, arg);
   func_t2url (temp, conf, cmlargs, vars, form, incvars, macs, cfile, tg, aref);

   /*** Scan the line for multiple spaces, character by character. */
   for (spaces=pos=0;   (cval = chxvalue (temp, pos));   ++pos) {

      /*** If we're in the midst of a group of spaces... */
      if (spaces) {
         if (cval == ' ')  { ++spaces;     continue; }

         for (count=0;   count < spaces-1;   ++count)
            chxcat (result, nbsp);
         chxcatval (result, THIN, L(' '));
         spaces = 0;
      }

      if (cval == ' ')     { spaces = 1;   continue; }
      if (cval == '\t')    { spaces = 5;   continue; }

      /*** "Normal" characters. */
      chxcatval (result, type, cval);
   }

   /*** Left over spaces at end... */
   if (spaces) {
      for (count=0;   count < spaces-1;   ++count)   chxcat (result, nbsp);
      chxcatval (result, THIN, L(' '));
   }

   RETURN (1);
}
