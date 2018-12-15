/*** CONFSIZE.  Return the size of the conference, in bytes.
/
/  size = confsize (cnum);
/
/  Parameters:
/    int4   size;  (size in bytes or 0 if failed)
/    int    cnum;  (conference number)
/
/  Purpose:
/     Return the size (in bytes) of a conference CNUM.
/
/  How it works:
/     Calls sysgfdir() to get the name of each file in the conference
/     directory.  Gets the size of each file.  Returns the total.
/
/  Returns: 0 or the amount of disk space that the conf takes up
/
/  Error Conditions:
/
/  Side Effects:
/
/  Related to:
/
/  Called by: 
/
/  Home: 
/     conf/confsize.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  3/26/93 14:45 Create this. */
/*: CP  7/16/93 17:00 Change return value to int4.  Add comments. */
/*: JV  9/23/93 13:17 Add ENTRY. */
/*: JB 10/26/93 15:44 Help the comments. */
/*: JZ 10/22/93 12:17 Add sysbpath. */
/*: CK 11/30/93 16:11 Declare & use path[] properly. */

#include <stdio.h>
#include "handicap.h"
#include "unitcode.h"
#include "chixuse.h"

#define  FN_SIZE  300

FUNCTION  int4 confsize (cnum)
   int    cnum;
{
   char  dname[FN_SIZE], fname[FN_SIZE], path[FN_SIZE];
   int   error, ok;
   int4  fsize, csize=0;

   ENTRY ("confsize", "");
   unit_name (dname, XCDD, L(cnum), L(0), L(0), L(0), nullchix);

   for (ok = sysgfdir (dname, fname, 1, &error);      ok == 1; 
        ok = sysgfdir (dname, fname, 0, &error)) {
      sysbpath (path, dname, fname);
      if (NOT sysfsize (path, &fsize, &error))  RETURN (L(0));
      csize += fsize;
   }

   if (ok == 2) RETURN (csize)
   else         RETURN (L(0));
}
