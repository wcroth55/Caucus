
/*** KILL_OBJ.   Delete a (persistent) object (from disk).
/
/    kill_obj (name);
/
/    Parameters:
/       char    *name;      (name of object to be 'killed')
/
/    Purpose:
/       Delete the disk entry for this object.
/
/    How it works:
/
/    Returns: 1 on success, 0 on system error
/
/    Known bugs:
/
/    Home:  obj/killobj.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/15/98 21:20 New function. */

#include <stdio.h>
#include "sweb.h"
#include "object.h"
#include "unitcode.h"

FUNCTION  kill_obj (char *name)
{
   static  Chix word = nullchix;

   ENTRY  ("kill_obj", "");

   if (word == nullchix) 
       word  = chxalloc (L(30), THIN, "kill_obj word");

   chxcpy (word, CQ(name+1));
   if (NOT unit_lock (XSOB, WRITE, L(0), L(0), L(0), L(0), word))  RETURN(0);
   unit_kill (XSOB);
   unit_unlk (XSOB);

   RETURN  (1);
}
