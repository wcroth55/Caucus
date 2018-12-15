
/*** LOAD_OBJ.   Load (read from disk) an Object.
/
/    obj = load_obj (name, ctab);
/
/    Parameters:
/       Object   obj;       (newly created and loaded object)
/       char    *name;      (name of requested object)
/       Vartab   ctab;      (table of classes)
/
/    Purpose:
/       Load (read) an object from disk.
/
/    How it works:
/       Creates a new instance of this object in memory, and
/       loads the data from disk into it.
/
/    Returns: new object on success
/             NULL on failure
/
/    Known bugs:      none
/
/    Home:  obj/loadobj.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/02/98 14:13 New function. */

#include <stdio.h>
#include "sweb.h"
#include "object.h"
#include "unitcode.h"

FUNCTION  Object load_obj (char *name, Vartab ctab)
{
   static  Chix word = nullchix;
   char    line[300], mname[100], class[100];
   Object  obj, make_obj();
   int4    pos, mnum;
   int     slot, first;

   ENTRY  ("load_obj", "");

   if (word == nullchix) 
       word  = chxalloc (L(80), THIN, "store_obj word");

   chxcpy (word, CQ(name+1));
   if (NOT unit_lock (XSOB, READ, L(0), L(0), L(0), L(0), word)) RETURN(NULL);
   if (NOT unit_view (XSOB))                { unit_unlk (XSOB);  RETURN(NULL); }

   /*** Determine list of member names of this object.  Use the current
   /    class definition, if it exists, in case it is a superset of what
   /    the object contained. */
   unit_read (XSOB, word, 0);
   ascofchx  (class, word, L(0), L(100));
   slot = vartab_is (ctab, word);
   unit_read (XSOB, word, 0);
   if (slot < 0) ascofchx  (line, word,              L(0), L(300));
   else          ascofchx  (line, ctab->value[slot], L(0), L(300));

   /*** Actually make the object. */
   if ( (obj = make_obj (name, line, 1)) == NULL) {
      unit_close (XSOB);
      unit_unlk  (XSOB);
      RETURN (NULL);
   }
   strcpy (obj->class, class);

   unit_read (XSOB, word, 0);    /* name */
   strcpy (obj->name, name);

   unit_read (XSOB, word, 0);    /* updated */
   pos = 0;
   obj->updated = chxint4 (word, &pos);

   mnum = -1;
   while (unit_read (XSOB, word, 0)) {
      if (chxvalue (word, L(0)) != ' ') {
         ascofchx (mname, word, L(0), L(100));
         mnum = strtable (obj->memnames, mname);
         if (mnum >= 0) chxclear  (obj->value[mnum]);
         first = 1;
      }
      else if (mnum < 0) ;
      else {
         if (NOT first) chxcatval (obj->value[mnum], THIN, L('\n'));
         first = 0;
         chxcatsub (obj->value[mnum], word, L(1), ALLCHARS);
      }
   }

   unit_close (XSOB);
   unit_unlk  (XSOB);

   RETURN  (obj);
}
