/*** MYTIME.   Representation of time as a string. 
/
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/07/95 16:52 New program. */

#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

#define   FUNCTION


FUNCTION  char *mytime(void)
{
   static char temp[200];
   long        now;

   now = time(NULL);
   strcpy (temp, ctime(&now));
   temp [strlen(temp)-1] = '\0';
   return (temp);
}
