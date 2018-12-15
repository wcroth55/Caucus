
/*** FREE_OBTAB.   Free (deallocate) an Obtab.
/
/    free_obtab (otab);
/
/    Parameters:
/       Obtab    otab;      (object table to be freed)
/
/    Purpose:
/       Free (deallocate) an object table (and all its contained objects).
/
/    How it works:
/
/    Returns: 
/
/    Related functions:  make_obtab()
/
/    Known bugs:      none
/
/    Home:  obj/freeobtab.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/28/98 15:20 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "entryex.h"
#include "object.h"

FUNCTION  free_obtab (Obtab otab)
{
   int    which;

   ENTRY  ("free_obtab", "");

   if (otab == NULL)  RETURN (0);

   for (which=0;   which <otab->used;   ++otab)  free_obj (otab->obj[which]);

   a_fre_vartab (otab->class);

   sysfree ( (char *) otab->obj);
   sysfree ( (char *) otab);

   RETURN  (1);
}
