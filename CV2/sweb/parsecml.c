
/*** PARSE_CML.   Parse part of a CML file.
/
/    lnum = parse_cml (tg, cfile, size, start, conf, vars, form, 
/                      query, broke, incvars, macs, otab, contd, aref);
/   
/    Parameters:
/       int        lnum;    (last line # parsed)
/       Target     tg;      (output target)
/       CML_File   cfile;   (CML file in memory)
/       int        size;    (size of CML file, in # lines)
/       int        start;   (start parsing at this line #)
/       Vartab     vars;    (CML variable list)
/       Vartab     form;    (form data variable list)
/       char      *query;   (CML arguments from client)
/       int       *broke;   (was a 'break' encountered?)
/       Chix       incvars; (text of 'include' variables)
/       Vartab     macs;    (list of defined macros)
/       Obtab      otab;    (object table)
/       int       *contd;   (continued?)
/       Chix       aref;    (array reference from $array_eval())
/
/    Purpose:
/       Parse a portion of a CML file, and send the output to Target tg.
/
/    How it works:
/       Parse until the end of the file, or until the first
/       un-paired 'end' statement, or until a "RETURN" directive.
/
/    Returns: number of last line (in cfile) parsed.
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/parsecml.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/26/95 13:05 New function. */
/*: CR  8/25/98 13:56 Replace lines[] with cfile. */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */
/*: CR  2/10/11 Correct(?) detection of 'return' from inside loops. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "diction.h"
#include "api.h"

extern int   cml_debug;

#define DONE(x)   { rval = x;   goto done; }

#define MAXTEMP 512
static char temp[MAXTEMP+2];

FUNCTION  parse_cml (tg, cfile, size, start, conf, vars, form, 
                     query, broke, incvars, macs, otab, contd, aref)
   Target    tg;
   CML_File  cfile;
   int       size, start;
   SWconf    conf;
   Vartab    vars, form;
   char     *query;
   int      *broke;
   Chix      incvars;
   Vartab    macs;
   Obtab     otab;
   int      *contd;
   Chix      aref;
{
   Chix   word, result, vname, increst, token, pastmac;
   int4   pos, truth, count, count0, count1, which, cval, vptr;
   int    num, newnum, rval, was_if, more, in, vn, numvars, hit_break, ok;
   int    argn, include_lnum, slot, mnum, hit_cont;
   char   verb[100];
   char  *where, argword[200], argreplace[200];
   char  *strtoken();
   ENTRY ("parse_cml", "");

   word    = chxalloc ( L(30), THIN, "parse_word");
   token   = chxalloc ( L(30), THIN, "parse_token");
   result  = chxalloc (L(100), THIN, "parse_result");
   vname   = chxalloc (L(100), THIN, "parse_vname");
   increst = chxalloc (L(100), THIN, "parse_increst");
   pastmac = chxalloc (L(200), THIN, "parse_pastmac");

   /*** Parse the CML file. */
   hit_break = 0;
   hit_cont  = 0;
   was_if    = 0;
   for (num=start;   num<size;   ++num) {
      cfile->cdex = num;

      /*** HTML lines are scanned (evaluating any macros, then any
      /    functions), and then converted from chix to ascii and 
      /    written to CD. */
      if (is_html (cfile->lines[num]) >= 0) {
         was_if = 0;
         func_mac   (pastmac, cfile->lines[num], conf, query, vars, form, 
                              incvars, macs, cfile, otab, tg, aref);
         func_scan  (result,  pastmac,    conf, query, vars, form, 
                              incvars, macs, cfile, otab, tg, aref);
/*       fix_dollar (result, 0);  */
/*       if (cml_debug)  fprintf (sublog, "\n---"); */
         pos = is_html (result) + 1;
         chxcatval (result, THIN, L('\n'));
         if      (tg->po != nullchix) chxcatsub (tg->po, result, pos, ALLCHARS);
         else if (NOT convert_and_write (tg->cd, result, pos, conf))   break;
         continue;
      }
/*    if (cml_debug) { fprintf (sublog, "\n");   fflush (sublog); } */

      chxtoken (word, nullchix, 1, cfile->lines[num]);
      clean_newlines (word);
      chxtrim        (word);
      ascofchx (verb, word, L(0), L(100));
      if (NULLSTR(verb))            continue;

      /*** END statement means return to caller (nested 'for's and 'if's
      /    are processed by recursive calls to parse_cml()). */
      if (streq (verb, "end"))      DONE (num)

      /*** RETURN statement returns to the caller.  Returning from a
      /    CML file is like going off the end of the file; returning from
      /    an include file continues on in the "includer". */
      else if (streq (verb, "return"))     DONE (S_INFINITY - 1)

      /*** QUIT statement is like going off the end of the original
      /    CML file, no matter where it is encountered, including inside
      /    an include file. */
      else if (streq (verb, "quit"))     DONE (S_INFINITY)

      /*** BREAK... */
      else if (streq (verb, "break"))   {
        *broke = 1;
         num = skip_cml (cfile, size, num+1);
         DONE (num);
      }

      /*** CONTINUE... */
      else if (streq (verb, "continue")) {
        *contd = 1;
         num = skip_cml (cfile, size, num+1);
         DONE (num);
      }

      /*** FOR statement...   "for x [y...] in list" */
      else if (streq (verb, "for")) {
         was_if    = 0;
         hit_break = 0;

         /*** Scan and evaluate macros & functions, e.g. "for x in $func(x)". */
         func_mac   (pastmac, cfile->lines[num], conf, query, vars, form, 
                                     incvars, macs, cfile, otab, tg, aref);
         func_scan  (result,  pastmac,    conf, query, vars, form, 
                                     incvars, macs, cfile, otab, tg, aref);
/*       fix_dollar (result, 0);  */
         clean_newlines (result);

         /*** Find IN, and the number of vars between FOR and IN. */
         chxcpy (token, CQ("in"));
         in = tablefind (result, token);
         if (in < 0) { in = 2;   numvars = 1; }
         else        { in++;     numvars = in - 2; }

         /*** Get the variable name, and for each value of the variable,
         /    parse the block of CML code inside the 'for' loop. */
         chxtoken  (vname,    nullchix, 2, result);
         for (which=in+1;   (1);   which += numvars) {
            for (vn=0;   vn<numvars;   ++vn) {
               more = (chxtoken (token, nullchix, which+vn, result) > 0); 
               chxtoken (vname, nullchix, vn+2, result);

               if ( (cval = chxvalue (vname, L(0))) == 0177 || cval=='.') {
                  if (NOT ob_set (vname, token, otab, cfile))
                     { more = 0;   break; }
               }
               else vartab_add  (vars, vname, token);
            }
            if (NOT more)  break;

            hit_cont  = 0;
            newnum = parse_cml (tg, cfile, size, num+1, conf, vars, form, 
                     query, &hit_break, incvars, macs, otab, &hit_cont, aref);
            if (hit_cont)   continue;
            if (hit_break)  break;
            if (newnum >= S_INFINITY-1)  DONE (newnum);
         }

         /*** Figure out where we END'ed.  If the 'for' loop was never
         /    executed, skip the entire block. */
         if (hit_break  ||  hit_cont  ||  which == in+1)  
                 num = skip_cml (cfile, size, num+1);
         else    num = newnum;
         hit_break = 0;
         hit_cont  = 0;
         if (num >= S_INFINITY-1)        DONE (num);
      }

      /*** WHILE statement...  "while x" */
      else if (streq (verb, "while")) {
         was_if    = 0;
         hit_break = 0;

         while (1) {
            hit_cont = 0;

            /*** Each time through the "while x" loop, evaluate "x". */
            func_mac   (pastmac, cfile->lines[num], conf, query, vars, form, 
                                        incvars, macs, cfile, otab, tg, aref);
            func_scan  (result,  pastmac,    conf, query, vars, form, 
                                        incvars, macs, cfile, otab, tg, aref);
/*          fix_dollar (result, 0);  */
            clean_newlines (result);
            chxtoken  (token, nullchix, 2, result);
            ok = chxnum (token, &count);
            if (NOT ok  ||  count == 0)  break;

            newnum = parse_cml (tg, cfile, size, num+1, conf, vars, form, 
                     query, &hit_break, incvars, macs, otab, &hit_cont, aref);
            if (hit_cont)   continue;
            if (hit_break)  break;
            if (newnum >= S_INFINITY-1)  DONE (newnum);
         }

         num = skip_cml (cfile, size, num+1);
         hit_break = 0;
         hit_cont  = 0;
         if (num >= S_INFINITY-1)  DONE (num);
      }

      /*** SET or COPY statement... */
      else if (streq (verb, "set")  ||  streq (verb, "copy")) {
         was_if = 0;

         /*** Scan and evaluate functions, e.g. "set x $func(x)". */
         func_mac   (pastmac, cfile->lines[num], conf, query, vars, 
                              form, incvars, macs, cfile, otab, tg, aref);
         func_scan  (result,  pastmac,    conf, query, vars, form, incvars, 
                                             macs, cfile, otab, tg, aref);
/*       fix_dollar (result, 0);  */

         /*** Remove (only the) trailing newline. */
/*       clean_newlines (result); */
/*       chxchomp (result); */
/*       chxtrim  (result); */ /* Should this still be here??? */

         vptr = chxtoken (vname,   nullchix, 2, result);
         chxclear  (token);
         if (streq (verb, "set")) chxtoken (nullchix,   token, 3, result);
         else chxcatsub (token, result, vptr + chxlen(vname) + 1, ALLCHARS);

         /*** Handle "set arg(n) data" => resets a CML argument(!) */
         if (chxindex (vname, L(0), CQ("arg(")) >= 0) {
            pos  = L(4);
            if ( (argn  = chxint4  (vname, &pos)) > 0  &&
                 (where = strtoken (argword, argn, query)) ) {
               strtoken (argreplace, 1, ascquick(token));
               stralter (where, argword, argreplace);
            }
         }
         /*** Handle "set object.member value". */
         else if ( (cval = chxvalue (vname, L(0))) == 0177 || cval=='.')
              ob_set (vname, token, otab, cfile);
         
         /*** Handle normal variable set. */
         else vartab_add  (vars, vname, token);
      }


      /*** EVAL statement... */
      else if (streq (verb, "eval")) {
         was_if = 0;

         /*** Scan and evaluate macros & functions, e.g. "eval $func(x)". */
         func_mac   (pastmac, cfile->lines[num], conf, query, vars, form, 
                                       incvars, macs, cfile, otab, tg, aref);
         func_scan  (result,  pastmac,    conf, query, vars, form, incvars, 
                                                macs, cfile, otab, tg, aref);
      }

      /*** INCLUDE statement... */
      else if (streq (verb, "include")) {
         was_if = 0;

         /*** Scan and evaluate functions, e.g. "include $(inc)/file.i". */
         func_mac   (pastmac, cfile->lines[num], conf, query, vars, 
                              form, incvars, macs, cfile, otab, tg, aref);
         func_scan  (result,  pastmac,    conf, query, vars, form, incvars, 
                                             macs, cfile, otab, tg, aref);
/*       fix_dollar (result, 0);  */
         clean_newlines (result);
         chxtrim        (result);

         chxtoken    (vname,    nullchix, 2, result);
         chxtoken    (nullchix, increst,  3, result);
         include_lnum = include_cml (tg, vname, vars, form, conf, query, 
                     &hit_break, increst, macs, cfile, otab, &hit_cont, aref);
         if (cml_debug) {
/*          fprintf (sublog, "include: %s\n", ascquick(vname)); */
/*          fflush  (sublog); */
         }
         if (hit_break)  { *broke = 1;  DONE (num); }
         if (hit_cont )  { *contd = 1;  DONE (num); }
         if (include_lnum == S_INFINITY)  DONE (S_INFINITY);
      }

      /*** COUNT statement... */
      else if (streq (verb, "count")) {
         was_if    = 0;
         hit_break = 0;

         /*** Scan and evaluate functions, e.g. "count var $func(x)". */
         func_mac   (pastmac, cfile->lines[num], conf, query, vars, 
                              form, incvars, macs, cfile, otab, tg, aref);
         func_scan  (result,  pastmac,    conf, query, vars, form, incvars, 
                                             macs, cfile, otab, tg, aref);
/*       fix_dollar (result, 0);  */
         clean_newlines (result);

         /*** Get the variable name. */
         chxtoken  (vname,    nullchix, 2, result);

         /*** Parse A and B in "count var a b". */
         chxtoken  (token, nullchix, 3, result);   
         count0 = 1;
         count1 = 0;
         if (chxnum (token, &count0)) {
            chxtoken  (token, nullchix, 4, result);   
            if (NOT chxnum (token, &count1))  count1 = count0;
         }
         
         if  (count0 > count1)   newnum = skip_cml (cfile, size, num+1);

         for (count=count0;   count<=count1;   ++count) {
            hit_cont = 0;
            sprintf (temp, "%d", count);
            chxcpy  (token, CQ(temp));

            if ( (cval = chxvalue (vname, L(0))) == 0177 || cval=='.') {
               if (NOT ob_set (vname, token, otab, cfile))  break;
            }
            else vartab_add  (vars, vname, token);

            newnum = parse_cml (tg, cfile, size, num+1, conf, vars, form, 
                     query, &hit_break, incvars, macs, otab, &hit_cont, aref);
            if (hit_cont)    continue;
            if (hit_break)   break;
            if (newnum >= S_INFINITY-1)  DONE (newnum);
         }
         if (hit_break || hit_cont)   num = skip_cml (cfile, size, num+1);
         else                         num = newnum;
         hit_break = 0;
         hit_cont  = 0;
         if (num >= S_INFINITY-1)        DONE (num);
      }

      /*** IF statement... */
      else if (streq (verb, "if")) {
         was_if = 1;

         /*** Scan and evaluate functions, e.g. "if $equal(x)". */
         func_mac  (pastmac, cfile->lines[num], conf, query, vars, 
                             form, incvars, macs, cfile, otab, tg, aref);
         func_scan (result,  pastmac,    conf, query, vars, form, incvars, 
                                            macs, cfile, otab, tg, aref);
/*       fix_dollar (result, 0);  */
         clean_newlines (result);

         truth = 0;
         if (chxtoken (token, nullchix, 2, result) >= 0)  chxnum (token, &truth);

         if (truth) num = parse_cml (tg, cfile, size, num+1, conf, vars, 
                form, query, &hit_break, incvars, macs, otab, &hit_cont, aref);
         else       num = skip_cml  (    cfile, size, num+1);

         if (hit_break) { *broke = 1;  DONE (num); }
         if (hit_cont ) { *contd = 1;  DONE (num); }
         if (num >= S_INFINITY-1)      DONE (num);
      }

      /*** ELIF statement... */
      else if (streq (verb, "elif")) {

         if (NOT was_if || truth)  num = skip_cml (cfile, size, num+1);
         else {
            /*** Scan and evaluate functions, e.g. "elif $equal(x)". */
            func_mac   (pastmac, cfile->lines[num], conf, query, vars, form, 
                                        incvars, macs, cfile, otab, tg, aref);
            func_scan  (result,  pastmac,    conf, query, vars, form, 
                                        incvars, macs, cfile, otab, tg, aref);
/*          fix_dollar (result, 0);  */
            clean_newlines (result);

            truth = 0;
            if (chxtoken (token, nullchix, 2, result) >= 0)
                chxnum (token, &truth);

            if (truth) num = parse_cml (tg, cfile, size, num+1, conf, vars, 
                form, query, &hit_break, incvars, macs, otab, &hit_cont, aref);
            else       num = skip_cml  (    cfile, size, num+1);

            if (hit_break) { *broke = 1;  DONE (num); }
            if (hit_cont ) { *contd = 1;  DONE (num); }
            if (num >= S_INFINITY-1)      DONE (num);
         }
      }

      /*** Matching ELSE statement to an IF. */
      else if (streq (verb, "else")) {
         if (NOT was_if || truth) 
                num = skip_cml  (    cfile, size, num+1);
         else   num = parse_cml (tg, cfile, size, num+1, conf, vars, form, 
                      query, &hit_break, incvars, macs, otab, &hit_cont, aref);
         was_if = 0;
         if (hit_break) { *broke = 1;  DONE (num); }
         if (hit_cont ) { *contd = 1;  DONE (num); }
         if (num >= S_INFINITY-1)      DONE (num);
      }

      /*** E-Report: illegal statement keyword. */
      else {
         sprintf (temp, "\n%s:%d Illegal keyword '%s'\n", cfile->filename,
                  cfile->lnums[num], verb);
         chxcat (cfile->errtext, CQ(temp));
      }

   }
   DONE (size);

done:
   chxfree (word);
   chxfree (token);
   chxfree (result);
   chxfree (vname);
   chxfree (increst);
   chxfree (pastmac);
   RETURN  (rval);
}


FUNCTION convert_and_write (cd, result, pos, conf)
   Hose  cd;
   Chix  result;
   int4  pos;
   struct sweb_conf_t *conf;
{
   int4  inc;
   int   lang;
   ENTRY ("convert_and_write", "");

   lang = conf->browserform + 1;
   for (;   (1);   pos += inc) {
      switch (lang) {
         case (LANG_NORM):
         case (LANG_ISO):  inc = ascofchx  (temp, result, pos, MAXTEMP);  break;
         case (LANG_EUC):  inc = eucofjix  (temp, result, pos, MAXTEMP);  break;
         case (LANG_SJIS): inc = sjisofjix (temp, result, pos, MAXTEMP);  break;
         default:          inc = -1;                                      break;
      }
 
      if (conf->output > 0)   write (conf->output, temp, strlen(temp));
      else {
         if (NOT buf_write (cd, temp, strlen(temp)))  RETURN (0);
      }
/*    if (cml_debug) fprintf (sublog, "%s", temp); */

      if (inc <= 0)  break;
   }

   RETURN (1);
}
