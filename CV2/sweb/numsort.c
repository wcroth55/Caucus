
/*** NUM_SORT.  Numeric sort.
/
/    num_sort (result, arg);
/   
/    Parameters:
/       Chix       result;  (append resulting value here)
/       Chix       arg;     (function argument)
/
/    Purpose:
/       Evaluate $num_sort(): sort a list of numbers.
/
/    How it works:
/
/    Returns: 
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  numsort.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/02/97 13:00 New function. */


#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "null.h"

FUNCTION  int num_sort (Chix result, Chix arg)
{
   int   direction, count, id, nx, swap;
   int  *value;
   int4  pos, pos1;
   char  temp[40];
   Chix  word;
   char *sysmem();
   ENTRY ("num_sort", "");

   word = chxalloc (L(40), THIN, "num_sort word");

   pos  = 0;
   direction = chxint4 (arg, &pos);
   pos1 = pos;

   for (count=0;   chxnextword (word, arg, &pos) >= 0;   ++count) ;

   value = (int *) sysmem (L((count+2) * sizeof(int)), "num_sort");
   if (value == (int *) NULL)  RETURN (0);
   
   pos = pos1;
   for (id=0;   id < count;   ++id)  value[id] = chxint4 (arg, &pos1);

   for (id=0;   id < count-1;   ++id) {
      for (nx=1;   nx < count;   ++nx) {
         if ( (direction > 0  &&  value[nx-1] > value[nx])  ||
              (direction < 0  &&  value[nx-1] < value[nx]) ) {
            swap        = value[nx-1];
            value[nx-1] = value[nx];
            value[nx  ] = swap;
         }
      }
   }

   for (id=0;   id < count;   ++id) {
      sprintf (temp, "%d ", value[id]);
      chxcat  (result, CQ(temp));
   }

   chxfree (word);
   sysfree ( (char *) value);
   RETURN  (1);
}
