
/*** FUNC_MAC.  CML macro in-line expansion.
/
/    func_mac (result, arg, conf, cmlargs, vars, form, incvars, 
/                      macs, cfile, otab, tg, aref);
/   
/    Parameters:
/       Chix     result;  (append resulting value here)
/       Chix     arg;     (macro arguments)
/       SWconf   conf;    (server configuration info)
/       char    *cmlargs; (CML script arguments)
/       Vartab   vars;    (CML variable list)
/       Vartab   form;    (form data variable list)
/       Chix     incvars; (text of 'include' variables)
/       Vartab   macs;    (macro list)
/       CML_File cfile;   (accumulated text of errors)
/       Obtab    otab;    (object table)
/       Target   tg;      (output target)
/       Chix     aref;    (array reference from $array_eval())
/
/    Purpose:
/       Expand CML macros in-line.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/funcmac.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/09/98 22:36 New function. */
/*: CR  4/04/00 12:48 Make temp[] big enough for any ascquick(). */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  ST_TEXT  1
#define  ST_NAME  2
#define  ST_ARG   3

FUNCTION  func_mac (
   Chix     result, 
   Chix     arg,
   SWconf   conf,
   char    *cmlargs,
   Vartab   vars,
   Vartab   form,
   Chix     incvars,
   Vartab   macs,
   CML_File cfile, 
   Obtab    otab,
   Target   tg,
   Chix     aref)
{
   static Chix percent = nullchix;
   static Chix slashdol, slash2;
   Chix        name, args, append, recurs, fscan;
   static char goodname[256];
   int         start_from, mdex, state, type, parens;
   int4        pos, value;
   char        temp[2000];

   ENTRY ("func_mac", "");

   /*** Initialization. */
   if (percent == nullchix)  {
      percent  = chxalsub (CQ("%"),     L(0), L(4));
      slashdol = chxalsub (CQ("\\$"),   L(0), L(4));
      slash2   = chxalsub (CQ("\\\\$"), L(0), L(4));
      for (mdex= 0 ;   mdex< 256;   ++mdex)  goodname[mdex] = 0;
      for (mdex='a';   mdex<='z';   ++mdex)  goodname[mdex] = 1;
      for (mdex='A';   mdex<='Z';   ++mdex)  goodname[mdex] = 1;
      for (mdex='0';   mdex<='9';   ++mdex)  goodname[mdex] = 1;
      goodname['_'] = goodname['.'] = 1;
   }

   name    = chxalloc (L( 20), THIN, "func_mac name");
   args    = chxalloc (L(100), THIN, "func_mac args");
   append  = chxalloc (L(200), THIN, "func_mac append");
   recurs  = chxalloc (L(200), THIN, "func_mac recurs");
   fscan   = chxalloc (L(200), THIN, "func_mac fscan");

   type = chxtype (arg);
   chxclear (result);
   state = ST_TEXT;
   for (pos=0L;   (value = chxvalue (arg, pos));   ++pos) {

      if (state == ST_TEXT) {

         if      (value == '%'  &&  goodname[chxvalue (arg, pos+1)]) {
            chxclear (name);
            state = ST_NAME;
            start_from = pos;
         }

         else {
            /*** A "\%" escapes the % character. */
            if (value == '\\'  &&  chxvalue (arg, pos+1) == '%') {
               ++pos;
               value = '%';
            }
            chxcatval (result, type, value);
         }
      }

      else if (state == ST_NAME) {

         /*** End macro name, begin accumulating args. */
         if (value == '(') {
            chxclear (args);
            parens = 0;
            state  = ST_ARG;

            /*** Non-existent macro. */
            if (vartab_is (macs, name) < 0) {
               chxcatval (result, THIN, L('%'));
               state = ST_TEXT;
               pos   = start_from;

               /*** E-Report: no such macro. */
               sprintf (temp, "\n%s:%d No such macro %%%s()\n",
                               cfile->filename, cfile->lnums[cfile->cdex],
                               ascquick(name));
               chxcat  (cfile->errtext, CQ(temp));
            }
         }

         else if (value == ' ')  ;

         /*** If bad name, go back to "%" and treat this all as plain text. */
         else if (value > 255  ||  NOT goodname[value]) {
            chxcatval (result, THIN, L('%'));
            state = ST_TEXT;
            pos   = start_from;
         }

         else   chxcatval (name, type, value);
      }

      /*** In the middle of argument list. */
      else if (state == ST_ARG) {

         /*** ...skip over ()'s inside the argument list. */
         if      (value == '(') {
            ++parens;
            chxcatval (args, type, value);
         }
         else if (value == ')'  &&  parens > 0) {
            --parens;
            chxcatval (args, type, value);
         }

         /*** At the end of the argument list... */
         else if (value == ')') {
            chxclear   (append);
            if (chxindex  (args, L(0), percent) >= 0) {
               func_mac   (recurs, args,   conf, cmlargs, vars, form, 
                                   incvars, macs, cfile, otab, tg, aref);
               chxreplace (recurs, L(0), slashdol, slash2);
               func_scan  (fscan,  recurs, conf, cmlargs, vars, form, 
                                   incvars, macs, cfile, otab, tg, aref);
               mac_expand (append, name, fscan, conf, cmlargs, vars, 
                              form, incvars, macs, cfile, otab, tg, aref);
            }
            else {
               chxreplace (args,   L(0), slashdol, slash2);
               func_scan  (fscan,  args, conf, cmlargs, vars, form, 
                                   incvars, macs, cfile, otab, tg, aref);
               mac_expand (append, name, fscan, conf, cmlargs, vars, 
                                   form, incvars, macs, cfile, otab, tg, aref);
            }

            /*** If recursively expanding the result contains more 
            /    %macro()s... */
            if (chxindex (append, L(0), percent) >= 0) {
               func_mac (recurs, append, conf, cmlargs, vars, form, 
                                 incvars, macs, cfile, otab, tg, aref);
               chxcpy   (append, recurs);
            }

            chxcat (result, append);
            state = ST_TEXT;
         }

         /*** Char in the middle of the arg list. */
         else chxcatval (args, type, value);
      }
   }

   /*** E-Report: if we came out in the middle of a NAME or ARGS, complain! */
   if (state == ST_NAME) {
      sprintf (temp, "\n%s:%d No ()'s after apparent macro %%%s\n",
                      cfile->filename, cfile->lnums[cfile->cdex],
                      ascquick(name));
      chxcat  (cfile->errtext, CQ(temp));
   }
   if (state == ST_ARG) {
      sprintf (temp, "\n%s:%d No closing ) after macro %%%s(...\n",
                      cfile->filename, cfile->lnums[cfile->cdex],
                      ascquick(name));
      chxcat  (cfile->errtext, CQ(temp));
   }

   chxfree (name);
   chxfree (args);
   chxfree (append);
   chxfree (recurs);
   chxfree (fscan);

   RETURN  (1);
}
