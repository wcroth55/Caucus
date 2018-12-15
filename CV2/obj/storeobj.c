
/*** STORE_OBJ.   Store (write to disk) an Object.
/
/    store_obj (obj);
/
/    Parameters:
/       Object   obj;       (object to be stored)
/
/    Purpose:
/       Store an Object.
/
/    How it works:
/       This version uses an individual file for each object.
/       Eventually this should be replaced with a true
/       database of some sort.
/
/       Once the object is written, clears all "wait" or "taint" flags
/       for OBJ.
/
/    Returns: 1 on success
/             0 on failure
/
/    Known bugs:      none
/
/    Home:  obj/storeobj.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/02/98 14:13 New function. */

#include <stdio.h>
#include "sweb.h"
#include "object.h"
#include "unitcode.h"

FUNCTION  store_obj (Object obj)
{
   static  Chix word = nullchix;
   static  Chix newl, blank;
   int     mnum, how1, how0, first;
   int4    pos;
   char    temp[100];
   char   *strtoken();

   ENTRY  ("store_obj", "");

   if (word == nullchix) {
      word  = chxalloc (L(80), THIN, "store_obj word");
      newl  = chxalsub (CQ("\n"), L(0), L(2));
      blank = chxalsub (CQ(" "),  L(0), L(2));
   }

   chxcpy (word, CQ(obj->name+1));
   if (NOT unit_lock (XSOB, WRITE, L(0), L(0), L(0), L(0), word))  RETURN(0);
   if (NOT unit_make (XSOB))                 { unit_unlk (XSOB);   RETURN(0); }

   unit_write (XSOB, CQ(obj->class));       unit_write (XSOB, newl);
   unit_write (XSOB, CQ(obj->memnames));    unit_write (XSOB, newl);
   unit_write (XSOB, CQ(obj->name));        unit_write (XSOB, newl);
   sprintf (temp, "%d\n", obj->updated);
   unit_write (XSOB, CQ(temp));

   for (mnum=0;   mnum < obj->members;   ++mnum) {
      obj->taint[mnum] = 0;
      strtoken (temp, mnum+1, obj->memnames);
      unit_write (XSOB, CQ(temp));     unit_write (XSOB, newl);
      for (pos=0;  (how1 = chxnextline (word, obj->value[mnum], &pos)) > 0; ) {
         unit_write (XSOB, blank);
         unit_write (XSOB, word);
         unit_write (XSOB, newl);
         how0 = how1;
      }
      if (how0 == 2) { unit_write (XSOB, blank);   unit_write (XSOB, newl); }
   }
   obj->wait = 0;
   unit_close (XSOB);
   unit_unlk  (XSOB);

   RETURN (1);
}
