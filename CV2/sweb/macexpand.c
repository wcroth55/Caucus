
/*** MAC_EXPAND.   Expand CML macros.
/
/    mac_expand (result, name, args, conf, cmlargs, 
/                        vars, form, incvars, macs, cfile, otab, tg, aref);
/   
/    Parameters:
/       Chix    result;   (append resulting value here)
/       Chix    name;     (macro name)
/       Chix    args;     (macro arguments)
/       SWconf  conf;     (server configuration info)
/       char   *cmlargs;  (CML script arguments)
/       Vartab  vars;     (CML variables)
/       Vartab  form;     (form data variables)
/       Chix    incvars;  (text of 'include' variables)
/       Vartab  macs;     (list of defined macros)
/       CML_File cfile;   (original cml file)
/       Obtab    otab;    (object table)
/       Target   tg;      (output target)
/       Chix     aref;    (array reference from $array_eval())
/
/    Purpose:
/       Expand CML macros in-line.
/
/    How it works:
/       Looks up macro NAME in the defined list of macros MACS.
/       Replaces the macro arguments @1, @2... @9 with the 1st,
/       2nd... 9th word in ARGS, and the argument @@ with the
/       entire content of ARGS.
/
/       Then calls func_scan() to evaluate any CML functions
/       to create the final RESULT.
/
/    Returns: 1 on success
/             0 if NAME does not exist
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

/*: CR  2/11/98 12:07 New function. */
/*: CR  4/04/00 12:48 Make temp[] big enough for any ascquick(). */
/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  mac_expand (
   Chix     result,
   Chix     name,
   Chix     args,
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
   static Chix word = nullchix;
   static Chix narg;
   Chix        interim;
   int         num, slot;
   char        temp[2000];

   ENTRY ("mac_expand", "");

   if (word == nullchix) {
      word    = chxalloc (L( 80), THIN, "mac_expand word");
      narg    = chxalloc (L(  4), THIN, "mac_expand narg");
   }

   slot = vartab_is (macs, name);
   if (slot < 0) {
      /*** E-Report: no such macro. */
      sprintf (temp, "\n%s:%d No such macro %%%s()\n",
                      cfile->filename, cfile->lnums[cfile->cdex],
                      ascquick(name));
      chxcat  (cfile->errtext, CQ(temp));
      RETURN  (0);
   }

   interim = chxalloc (L(200), THIN, "mac_expand interim");
   chxcpy (interim, macs->value[slot]);

   for (num=1;   num<10;   ++num) {
      chxtoken (word, nullchix, num, args);
      sprintf  (temp, "@%d", num);
      chxcpy   (narg, CQ(temp));
      chxreplace (interim, L(0), narg, word);
   }
   chxcpy (narg, CQ("@@"));
   chxreplace (interim, L(0), narg, args);

   func_scan (result, interim, conf, cmlargs, vars, form, 
                      incvars, macs, cfile, otab, tg, aref);
   chxfree (interim);
   RETURN  (1);
}
