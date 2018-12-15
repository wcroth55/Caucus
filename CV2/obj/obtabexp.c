
/*** OBTAB_EXP.   Expand the size of an object table.
/
/    slot = obtab_find (ot);
/   
/    Parameters:
/       Otab    ot;     (object table to be expanded)
/
/    Purpose:
/       Expand the size of object table OT.
/
/    How it works:
/       Doubles the current max size of OT.
/
/    Returns: 1 on success
/             0 if out of memory
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  obj/obtabexp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/28/98 14:14 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "entryex.h"
#include "object.h"

FUNCTION  obtab_exp (Obtab ot)
{
   int     num;
   Object *obj2;
   char   *sysmem();

   ENTRY ("obtab_exp", "");

   obj2 = (Object *) sysmem ( sizeof (Object *) * (ot->max * 2));
   if (obj2 == NULL)  RETURN (0);

   for (num=0;   num<ot->used;   ++num)  obj2[num] = ot->obj[num];
   ot->max = ot->max * 2;

   /*** Free old data areas. */
   sysfree ( (char *) ot->obj);
   ot->obj = obj2;

   RETURN (1);
}
