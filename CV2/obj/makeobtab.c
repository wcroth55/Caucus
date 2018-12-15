
/*** MAKE_OBTAB.   Make (allocate) an Obtab.
/
/    otab = make_obtab (size);
/
/    Parameters:
/       Obtab    otab;      (RETURNed/created object table)
/       int      size;      (initial size of table)
/
/    Purpose:
/       Allocate and initialize an Obtab.
/
/    How it works:
/
/    Returns: newly created object table
/             NULL on error
/
/    Error Conditions: insufficient heap memory, or heap catastrophe.
/  
/    Related functions:  free_obtab()
/
/    Known bugs:      none
/
/    Home:  obj/makeobtab.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/28/98 15:20 New function. */

#include <stdio.h>
#include "entryex.h"
#include "chixuse.h"
#include "object.h"

FUNCTION  Obtab  make_obtab (int size)
{
   Obtab   otab;
   char   *sysmem();

   ENTRY  ("make_obtab", "");

   if (size < 1)                 RETURN (NULL);

   otab = (Obtab) sysmem (sizeof (struct obtab_t), "obtab");
   if (otab == NULL)             RETURN (NULL);

   otab->obj = (Object *) sysmem (sizeof (Object) * size, "otab obj");
   if (otab->obj == NULL)        RETURN (NULL);

   otab->class = a_mak_vartab ("Obtab class");
   otab->max   = size;
   otab->used  = 0;
   strcpy (otab->err, "\177ERR");

   RETURN (otab);
}
