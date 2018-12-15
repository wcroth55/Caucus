
/*** commaList.   Implement $commaList() convenience function.
/
/    commaList (result, arg);
/   
/    Parameters:
/       Chix       result;  (append resulting value here)
/       Chix       arg;     (function argument)
/
/    Purpose:
/       Evaluate $commaList(type elements)
/
/    How it works:
/       type:  n[umber]   elements are numbers
/       type:  s[tring]   elements are strings, must be single-quoted.
/
/    Returns: 
/
/    Home:  commalist.c
/ */

/*** Copyright (C) 2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 06/08/06 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "null.h"

FUNCTION   int commaList (Chix result, Chix arg)
{
   Chix    word;
   int4    pos;
   int     quoted, count;
   ENTRY ("commaList", "");

   word = chxalloc (L(40), THIN, "trip_sort word");

   pos = 0;
   if (chxnextword (word, arg, &pos) < 0)  RETURN(1);
   jixreduce (word);
   quoted = (chxvalue (word, 0) == 's');
   
   for (count=0;   chxnextword (word, arg, &pos) >= 0;   ++count) {
      if (count > 0)  chxcatval (result, THIN, L(','));
      if (quoted) {
         chxcatval (result, THIN, L('\''));
         func_escsingle (result, word);
         chxcatval (result, THIN, L('\''));
      }
      else  chxcat (result, word);
   }

   RETURN  (1);
}
