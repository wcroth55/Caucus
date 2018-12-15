/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** WHATNM.   What name partfile is this name in?
/
/    ptr = whatnm (str, nmdir);
/
/    Parameters:
/       Namelist  ptr;     (returned namelist node pointer)
/       Chix      str;     (name that caller is looking for)
/       Namelist  nmdir;   (namelist of namepart files)
/
/    Purpose:
/       Find which name partfile contains a name STR.
/
/    How it works:
/       NMDIR contains the namepart file directory, a namelist of
/       the namepart files.  STR is the name the caller is looking 
/       for.
/
/       Whatnm() scans NMDIR and returns a pointer to the node in
/       NMDIR that contains the first namepart file that might contain STR.
/
/    Returns: pointer to Namelist node in NMDIR, or
/             NULL on error.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  people/whatnm.c
/ */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: CR  4/12/90 15:11 Rename strcomp() -> sysstrcomp(). */
/*: DE  4/29/92 14:21 Chixified */
/*: CR 11/16/92 16:48 Return Namelist ptr, not partfile number. */
/*: CR 12/18/92 13:25 Break statements for easier debugging. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

FUNCTION  Namelist whatnm (str, nmdir)
   Chix     str;
   Namelist nmdir;
{
   int      result;

   ENTRY ("whatnm", "");

   for (;   nmdir->next != null.nl;   nmdir = nmdir->next) {
      result = chxcompare (str, (nmdir->next)->str, ALLCHARS);
      if (result < 0)  RETURN (nmdir);
   }

   RETURN (null.nl);
}
