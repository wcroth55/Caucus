
/*** TO_JIX.   Convert data from ascii to Jix.
/
/    to_jix (jix, ascii, init, lang);
/ 
/    Parameters:
/       Chix     jix;       (append converted result here)
/       char    *ascii;     (text to be converted)
/       int      init;      (init argument for sjis/euc functions)
/       int      lang;      (convert according to language code)
/
/    Purpose:
/       Convert ascii data to Jix, according to appropriate language.
/
/    How it works:
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  tojix.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/03/95 16:38 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "diction.h"

extern FILE *sublog;
extern int   cml_debug;

FUNCTION  void to_jix (jix, ascii, init, lang)
   Chix   jix;
   char  *ascii;
   int    init, lang;
{
   char   outseq[20];

   switch (lang) {
      case (LANG_EUC):
         jixofeuc  (jix, ascii, init, outseq);
         break;

      case (LANG_SJIS):
         jixofsjis (jix, ascii, init, outseq);
         break;
  
      case (LANG_NORM):   case (LANG_ISO):  default:
         chxcat (jix, CQ(ascii));
         break;
   }

   return;
}
