
/*** GEN_SORT.   Sort a list of words, returned sorted list of indices.
/
/    gen_sort (result, arg);
/   
/    Parameters:
/       Chix       result;  (append resulting value here)
/       Chix       arg;     (function argument)
/
/    Purpose:
/       Evaluate $gen_sort(): sort a list of words, return indices to 
/       sorted order (e.g., $gen_sort(1 a c b) returns "1 3 2".)
/
/    How it works:
/
/    Returns: 
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  gensort.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/19/97 16:54 New function */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "namelist.h"
#include "null.h"

extern union null_t null;

FUNCTION  int gen_sort (Chix result, Chix arg)
{
   Namelist list, node, nlnode();
   Chix     word;
   int      token, direction;
   int4     pos;
   char     temp[40];
   ENTRY ("gen_sort", "");

   list = nlnode(1);
   word = chxalloc (L(80), THIN, "gen_sort word");

   pos = 0;
   direction = chxint4 (arg, &pos);

   for (token=2;   chxtoken (word, nullchix, token, arg) >= 0;   ++token) {
      nladd (list, word, token-1);
   }

   chxclear (word);
   for (node=list->next;   node!=null.nl;   node=node->next) {
      sprintf (temp, "%d ", node->num);
      if (direction > 0)  chxcat  (word, CQ(temp));
      else                chxconc (word, CQ(temp), word);
   }
   chxcat (result, word);

   chxfree (word);
   nlfree  (list);

   RETURN  (1);
}
