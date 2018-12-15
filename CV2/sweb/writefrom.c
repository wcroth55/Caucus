
/*** WRITE_FROM_JIX.    Write jix string to file, converting appropriately.
/
/    write_from_jix (fp, result, pos, lang);
/   
/    Parameters:
/       FILE   *fp;      (open file to write to)
/       Chix    result;  (jix string to write)
/       int4    pos;     (starting position in RESULT)
/       int     lang;    (language code used for writing)
/
/    Purpose:
/       Write text in RESULT (from starting position POS) to file
/       open on FP, converting to language format LANG on the fly.
/
/    How it works:
/
/    Returns: 
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  writefrom.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/03/95 17:19 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "diction.h"

extern int   cml_debug;

#define MAXTEMP 512
static char temp[MAXTEMP+2];

FUNCTION void write_from_jix (fp, result, pos, lang)
   FILE *fp;
   Chix  result;
   int   lang;
   int4  pos;
{
   int4  inc;
   
   for (;   (1);   pos += inc) {
      switch (lang) {
         case (LANG_NORM):
         case (LANG_ISO):  inc = ascofchx  (temp, result, pos, MAXTEMP);  break;
         case (LANG_EUC):  inc = eucofjix  (temp, result, pos, MAXTEMP);  break;
         case (LANG_SJIS): inc = sjisofjix (temp, result, pos, MAXTEMP);  break;
         default:          inc = -1;                                      break;
      }
 
      fprintf (fp, "%s", temp);

      if (inc <= 0)  break;
   }

   return;
}
