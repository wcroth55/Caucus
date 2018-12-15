/*** TRIM_FILENAME.   Remove any full pathname specification from a file name.
/
/    trim_filename (name);
/
/    Parameters:
/       Chix   name;      (file name to be trimmed)
/
/    Purpose:
/       File names supplied by the user may have a full path specification.
/       For certain purposes in Caucus (such as attaching files to a 
/       response), we need to trim off everything but the actual name
/       of the file.
/
/    How it works:
/       Remove any Unix-style or Vax-VMS style full-path specification
/       information from NAME.
/
/    Returns: 1
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  gen/trimfile.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/05/94 12:49 New function. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  trim_filename (name)
   Chix   name;
{
   Chix   old;
   int4   bracket, semi, slash;

   ENTRY ("trim_filename", "");

   old = chxalloc (L(80), THIN, "trim_fil old");

   chxcpy (old, name);

   /*** Vax filespec is "disk:[directory]name;ver". */
   if ( (bracket = chxindex (old, L(0), CQ("]"))) >= 0) {
      semi = chxindex (old, bracket+1, CQ(";"));
      if (semi < L(0))  semi = ALLCHARS;
      chxclear  (name);
      chxcatsub (name, old, bracket+1, semi);
   }

   /*** Unix is "/a/b/.../name". */
   else if ( (slash = chxrevdex (old, L(2000), CQ("/"))) >= 0) {
      chxclear  (name);
      chxcatsub (name, old, slash+1, ALLCHARS);
   }

   chxfree (old);
   return  (1);
}
