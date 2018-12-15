/*** A_DEBUG.  Debugging report function for the Caucus API.
/
/    a_debug (type, object);
/
/    Parameters:
/       int      type;    (type of debugging report)
/       Ageneric object;  (object to debug)
/
/    Purpose:
/       Report debugging activities.
/
/    How it works:
/       Type A_ALLOC records objects allocated, in
/       file "apialoc.out" in the current directory.
/
/       Type A_FREE records objects freed, in
/       file "apifree.out" in the current directory.
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:
/
/    Called by:  API functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/adebug.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  8/01/93  9:33 New function. */
/*: CP  9/02/93 17:11 Put sysprv() calls around fopen() calls. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

FUNCTION  a_debug (int type, Ageneric object)
{
   static FILE *f_aloc = NULL;
   static FILE *f_free = NULL;

   if (type == A_ALLOC) {
      if (f_aloc==NULL) {
         sysprv(0);
         nt_chkfile ("apialoc.out", 0700);
         f_aloc = fopen ("apialoc.out", "a");
         sysprv(1);
      }
      if (f_aloc==NULL)  return (0);
      fprintf (f_aloc, "%s %d\n", object->dname, object->tag);
      fflush  (f_aloc); 
   }

   if (type == A_FREE) {
      if (f_free==NULL) {
         sysprv(0);
         nt_chkfile ("apifree.out", 0700);
         f_free = fopen ("apifree.out", "a");
         sysprv(1);
      }
      if (f_free==NULL)  return (0);
      fprintf (f_free, "%s %d\n", object->dname, object->tag);
      fflush  (f_free); 
   }

   if (type == A_CLOSE) {
      if (f_aloc != NULL)  fclose (f_aloc);
      if (f_free != NULL)  fclose (f_free);
   }

   return (1);
}
