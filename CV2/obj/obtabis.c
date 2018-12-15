
/*** OBTAB_IS.   Is this object in the obtab?
/
/    slot = obtab_is (ot, name);
/   
/    Parameters:
/       int     slot;   (returned slot number in VARS)
/       Obtab   ot;     (object table)
/       char   *name;   (object name)
/
/    Purpose:
/       Is object NAME in the object table OT?
/
/    How it works:
/       Scans OT for NAME.  If not found, tries to
/       load it (assuming it is a persistent object), and
/       add it to OT.
/
/    Returns: slot number on success
/             -1   if not found
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  obj/obtabis.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/28/98 14:32 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "entryex.h"
#include "object.h"

#define  ZERO(x)  (!(x[0]))

FUNCTION  obtab_is (Obtab ot, char *name)
{
   int    num, new;
   char   oname[100];
   Object obj, load_obj();

   ENTRY ("obtab_is", "");

   if (name==NULL  ||  ZERO(name))                     RETURN (-1);

   oname[0] = 0177;
   strcpy (oname+1, name + (name[0]==0177 ? 1 : 0));

   /*** Scan all of the objects currently in the table... */
   for (num=0;   num<ot->used;   ++num) {
      if (streq (ot->obj[num]->name, oname))           RETURN (num);
   }

   /*** If not found, try to load it (assuming that it is a 
   /    currently-not-loaded persistent object). */
   if ( (obj = load_obj (oname, ot->class)) == NULL)   RETURN (-1);

   /*** Put the object in a slot in the object table.  If the table
   /    is nearly full, attempt to expand it. */
   new = ot->used++;
   if (new > ot->max - 5  &&  NOT obtab_exp (ot))      RETURN(-1);
   ot->obj[new] = obj;
   RETURN (new);
}
