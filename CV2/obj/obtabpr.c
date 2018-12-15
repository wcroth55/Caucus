
/*** OBTAB_PRINT.   Print an object table.
/
/    obtab_print (ot);
/   
/    Parameters:
/       Obtab   ot;     (object table)
/
/    Purpose:
/       Print object table info on stderr.
/
/    How it works:
/
/    Returns: 
/
/    Known bugs:
/
/    Home:  obj/obtabpr.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/15/98 21:32 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "entryex.h"
#include "object.h"

#define  ZERO(x)  (!(x[0]))

FUNCTION  obtab_print (Obtab ot)
{
   int    num;
   Object obj;
   ENTRY ("obtab_pr", "");

   if (ot == NULL)   RETURN (0);

   for (num=0;   num<ot->used;   ++num) {
      obj = ot->obj[num];
      if (obj == NULL)  fprintf (stderr, "%4d NULL!\n", num);
      else              fprintf (stderr, "%4d %30s  %2d members\n", 
                                 num, obj->name, obj->members);
   }

   RETURN (1);
}
