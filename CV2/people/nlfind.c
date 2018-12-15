
/*** NLFIND.   Find STR in data in a namelist.
/
/    ptr = nlfind (list, str);
/
/    Parameters:
/       Namelist  ptr;       (pointer to entry in namelist LIST)
/       Namelist  list;      (namelist being searched)
/       Chix      str;       (look for STR in LIST)
/
/    Purpose:
/       Find STR in a namelist LIST.
/
/    How it works:
/       Looks for an exact match of STR with the data (first word) in 
/       an entry in Namelist LIST.
/
/    Returns: ptr to first match, if found
/             NLNULL, otherwise.
/
/    Error Conditions:
/  
/    Operating system dependencies: None.
/
/    Known bugs:
/
/    Home:  people/nlfind.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/06/96 17:22 New function. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  Namelist nlfind (Namelist list, Chix str)
{
   static Chix word1 = nullchix;
   Namelist    ptr;

   ENTRY ("nlfind", "");

   if (list  == NLNULL)         RETURN (NLNULL);

   if (word1 == nullchix)       word1 = chxalloc (L(40), THIN, "nlfind word1");

   for (ptr = list->next;   ptr != NLNULL;   ptr=ptr->next) {
      chxtoken  (word1, nullchix, 1, ptr->str);
      if (chxeq (word1, str))   RETURN (ptr);
   }

   RETURN (NLNULL);
}
