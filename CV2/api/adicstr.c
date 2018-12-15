
/*** A_DICSTR.   Get a text string from a dictionary.
/
/    ok = a_dicstr (result, save, name, dicnum);
/
/    Parameters:
/       int     ok;      (success?)
/       Chix    result;  (place to put resulting string)
/       Chix   *save;    (place to save memory-resident strings)
/       char   *name;    (name of string to fetch)
/       int     dicnum;  (number of dictionary to use)
/
/    Purpose:
/       Look in dictionary DICNUM for a string called NAME.  Put
/       it in RESULT.
/
/    How it works:
/       If SAVE is not NULL, and *SAVE is NULL, then:
/         (a) allocate a chix.
/         (b) set SAVE to point to that chix.
/         (c) copy the string into that chix.
/
/       This also means that on future calls to a_dicstr(), if *SAVE is
/       already set to point to the memory-resident copy of the string,
/       then it is copied directly to RESULT w/o going to the dictionary.
/
/       For SAVE'd strings, a_dicstr() also remembers which dictionary
/       the string came from.  If the user switches dictionaries, 
/       a_dicstr() will take note and load the string from the new dictionary.
/
/    Returns: 1 on success, 0 if no string NAME in the dictionary.
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
/    Known bugs:      none
/
/    Home:  api/adicstr.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/11/93 17:15 New function, based on old 'mdstr'. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

FUNCTION  a_dicstr (Chix result, Chix *save, char *name, int dicnum)
{
   int    status;

   if (save != (Chix *) NULL  &&  *save != nullchix) {
      if ((*save)->dic == dicnum) { chxcpy (result, *save);   return (1); }
      chxfree (*save);
      *save = nullchix;
   }

   if (NOT sysmdstr (name, dicnum, result, &status))          return (0);

   result->dic = dicnum;

   if (save != (Chix *) NULL) {
      *save = chxalloc (L(40), THIN, "a_dicstr *save");
      chxcpy (*save, result);
   }

   return (1);
}
