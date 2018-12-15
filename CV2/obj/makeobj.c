
/*** MAKE_OBJ.   Make (allocate) an Object.
/
/    obj = make_obj (name, memnames, persist);
/
/    Parameters:
/       Object   obj;       (RETURNed/created object)
/       char    *name;      (object name; if empty, make one up)
/       char    *memnames;  (list of members)
/       int      perist;    (persistence)
/
/    Purpose:
/       Allocate and initialize an Object;
/
/    How it works:
/
/    Returns: newly created object
/             NULL on error
/
/    Error Conditions: insufficient heap memory, or heap catastrophe.
/  
/    Related functions:  free_obj()
/
/    Known bugs:      none
/
/    Home:  obj/makeobj.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/25/98 15:20 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "entryex.h"
#include "object.h"

#define   ZERO(x)  (!(x[0]))

static int objnum = 0;

FUNCTION  Object  make_obj (char *name, char *memnames, int persist)
{
   Object  obj;
   int     which;
   char    word[100];
   char   *sysmem(), *strtoken();

   ENTRY  ("make_obj", "");

   /*** Allocate the object itself, and give it it's name. */
   obj = (Object) sysmem (sizeof (struct object_t), "obj");
   if (obj == NULL)            RETURN (NULL);

   obj->name[0] = 0177;
   if  (name==NULL  ||  ZERO(name)) 
         sprintf (obj->name+1, "T%07d", objnum++);
   else  strcpy  (obj->name+1, (name[0] == 0177 ? name+1 : name));

   /*** Copy the list of member names. */
   obj->memnames = sysmem (strlen (memnames) + 2, "memnames");
   if (obj->memnames == NULL)  RETURN (NULL);
   strcpy (obj->memnames, memnames);

   /*** Set initial values of other parts. */
   obj->persists = persist;
   obj->updated  =   0;
   obj->class[0] = '\0';
   obj->wait     =   0;
   obj->lockfd   =   0;

   /*** Allocate the array of chix for the member values. */
   for (which=1;   strtoken (word, which, memnames) != NULL;   ++which) ;
   obj->members = which - 1;
   obj->taint   =          sysmem (                obj->members, "taint");
   obj->value   = (Chix *) sysmem (sizeof (Chix) * obj->members, "value");
   if (obj->value == NULL)       RETURN (NULL);
   for (which=0;   which < obj->members;   ++which) {
      obj->value[which] = chxalloc (L(80), THIN, "make_obj value");
      obj->taint[which] = 0;
   }
   if (obj->value[obj->members-1] == nullchix)   RETURN (NULL);

   RETURN (obj);
}
