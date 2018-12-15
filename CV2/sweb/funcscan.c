
/*** FUNC_SCAN.  Scan LINE for CML functions, substitute value into RESULT.
/
/    func_scan (result, line, conf, cmlargs, vars, form, incvars, 
/                       macs, cfile, otab, tg, aref);
/   
/    Parameters:
/       Chix      result;   (put resulting text here)
/       Chix      line;     (line of CML text to scan)
/       SWconf    conf;     (server configuration info)
/       char     *cmlargs;  (CML script arguments)
/       Vartab    vars;     (CML variables)
/       Vartab    form;     (form data variables)
/       Chix      incvars;  (text of 'include' variables)
/       Vartab    macs;     (defined macros)
/       CML_File  cfile;    (original cml file for this page)
/       Obtab     otab;     (object table)
/       Target    tg;       (output target)
/       Chix      aref;     (array reference from $array_eval())
/
/    Purpose:
/       Scan the CML text in LINE, evaluate CML functions as they
/       are found, and put the result into RESULT.
/
/    How it works:
/       Scans for strings of the form "$name(value)", and inserts in
/       their place the evaluation of the function.  Recursion is allowed, 
/       i.e. VALUE may also contain function evaluations.
/
/       See func_value() for a list of defined functions.
/
/       The form $(var) evaluates to the value of variable VAR.
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funcscan.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/10/95 17:48 New function. */
/*: CR  3/25/95 13:46 Convert to chix. */
/*: CR  4/04/00 12:33 Test for bad $func(), rescan as text if failed. */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */
/*: CR 10/30/01 11:36 Use prot_ functions instead of protect variable. */
/*: CR 10/04/04 Add infinite recursion testing. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  ST_TEXT   0
#define  ST_NAME   1
#define  ST_ARG    2

#define  DEEPEST_CALL  100   /* call_depth beyond this is error! */

FUNCTION  func_scan (result, line, conf, cmlargs, vars, form, 
                     incvars, macs, cfile, otab, tg, aref)
   Chix       result, line;
   char      *cmlargs;
   SWconf     conf;
   Vartab     vars, form;
   Chix       incvars;
   Vartab     macs;
   CML_File   cfile;
   Obtab      otab;
   Target     tg;
   Chix       aref;
{
   static int  recursive_evaluation = 0;
   static Chix dollar = nullchix;
   static char goodname[256];
   Chix        name, args, recurs, append;
   int4   pos, value, posdollar;
   int    type, state, parens, gc;
   char   ascname[200];
   static int counter    = 0;
   static int call_depth = 0;
   int        thistime;
   void       prot_push(), prot_pop();
   ENTRY ("func_scan", "");

   thistime = ++counter;
   if (call_depth > DEEPEST_CALL) {
      chxcat (result,
         CQ(" --- ERROR: infinite recursion in function or macro call ---"));
      RETURN (1);
   }
   ++call_depth;

   /*** First call? */
   if (dollar == nullchix)  {
      dollar = chxalsub (CQ("$"), L(0), L(4));
      for (gc=0;     gc<256;    ++gc)  goodname[gc] = 0;
      for (gc='0';   gc<='9';   ++gc)  goodname[gc] = 1;
      for (gc='a';   gc<='z';   ++gc)  goodname[gc] = 1;
      for (gc='A';   gc<='Z';   ++gc)  goodname[gc] = 1;
      goodname['_'] = 1;
   }

   type   = chxtype (line);
   name   = chxalloc ( L(30), type, "func_scan name");
   args   = chxalloc ( L(30), type, "func_scan args");
   recurs = chxalloc (L(100), type, "func_scan recurs");
   append = chxalloc (L(100), type, "func_scan append");

   chxclear (result);
   state = ST_TEXT;
   for (pos=0L;   (value = chxvalue (line, pos));   ++pos) {

      if (state == ST_TEXT) {

         if      (value == '$') {
            posdollar = pos;
            chxclear (name);
            state = ST_NAME;
         }

         else {
            /*** A "\" escapes the next character. */
            if (value == '\\'  &&  chxvalue (line, pos+1) == '$') {
               ++pos;
               value = '$';
            }
            chxcatval (result, type, value);
         }
      }

      else if (state == ST_NAME) {
         if (value == '(') {
            chxclear (args);
            parens = 0;
            state  = ST_ARG;
         }
         else if (value == ' ') ;
         else if (value < 256  &&  goodname[value]) 
            chxcatval (name, type, value);

         /*** Bad function name; revert to text and rescan; complain. */
         else {
            char temp[2000];

            state = ST_TEXT;
            chxcatval (result, THIN, L('$'));
            pos   = posdollar;

            sprintf (temp, "\n%s:%d No ()'s after apparent function $%s\n",
                            cfile->filename, cfile->lnums[cfile->cdex],
                            ascquick(name));
            chxcat  (cfile->errtext, CQ(temp));
         }
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
            ascofchx  (ascname, name, L(0), L(100));

/*          if (streq (ascname, "reval"))    recursive_evaluation = 1; */
            if (streq (ascname, "protect"))  prot_push (1);

            chxclear   (append);
            if (chxindex  (args, L(0), dollar) >= 0) {
               func_scan  (recurs, args,         conf, cmlargs, vars, form, 
                                         incvars, macs, cfile, otab, tg, aref);
               func_value (append, name, recurs, conf, cmlargs, vars, form, 
                                0, incvars, macs, cfile, otab, tg, aref);
            }
            else  
               func_value (append, name,   args, conf, cmlargs, vars, form,
                                0, incvars, macs, cfile, otab, tg, aref);
/*          fix_dollar (append, 1); */



            /*** If recursively expanding the result contains more 
            /    $function()s... */
            if (streq (ascname, "reval"))    recursive_evaluation = 1;
            if (recursive_evaluation && chxindex (append, L(0), dollar) >= 0) {
               func_scan  (recurs, append, conf, cmlargs, vars, form, 
                                       incvars, macs, cfile, otab, tg, aref);
               chxcpy     (append, recurs);
            }

            chxcat (result, append);
            state = ST_TEXT;
            if (streq (ascname, "reval"))    recursive_evaluation = 0;
            if (streq (ascname, "protect"))  prot_pop();
         }

         /*** Char in the middle of the arg list. */
         else chxcatval (args, type, value);
      }
   }

   /*** E-Report: if we came out in the middle of a NAME or ARGS, complain! */
   if (state == ST_NAME) {
      char temp[2000];
      sprintf (temp, "\n%s:%d No ()'s after apparent function $%s\n",
                      cfile->filename, cfile->lnums[cfile->cdex],
                      ascquick(name));
      chxcat  (cfile->errtext, CQ(temp));
   }
   if (state == ST_ARG) {
      char temp[2000];
      sprintf (temp, "\n%s:%d No closing ) after function call $%s(...\n",
                      cfile->filename, cfile->lnums[cfile->cdex],
                      ascquick(name));
      chxcat  (cfile->errtext, CQ(temp));
   }

   chxfree (name  );
   chxfree (args  );
   chxfree (recurs);
   chxfree (append);

   --call_depth;
   RETURN  (1);
}
