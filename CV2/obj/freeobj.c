
/*** FREE_OBJ.   Free (unallocate) an Object.
/
/    free_obj (obj);
/
/    Parameters:
/       Object   obj;       (object to be freed)
/
/    Purpose:
/       Unallocate an Object;
/
/    How it works:
/
/    Related functions:  make_obj()
/
/    Known bugs:      none
/
/    Home:  obj/freeobj.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/25/98 15:20 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "entryex.h"
#include "object.h"

FUNCTION  free_obj (Object obj)
{
   int    which;

   ENTRY  ("free_obj", "");

   if (obj == NULL)   RETURN (0);

   for (which=0;   which < obj->members;   ++which)
     chxfree (obj->value[which]);
   sysfree ( (char *) obj->value);
   sysfree (          obj->taint);

   sysfree ( (char *) obj->memnames);
   sysfree ( (char *) obj);

   RETURN  (1);
}
