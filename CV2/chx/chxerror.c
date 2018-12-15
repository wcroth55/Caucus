
/*** CHXERROR.  Handle error conditions from Chix package functions.
/
/    chxerror (error, s1, s2)
/
/    Parameters:
/       int   error;    ( Chix error code number CXE... )
/       char *s1;       ( Name of calling function. )
/       char *s2;       ( Other information (chix name, etc.) )
/
/    Purpose:
/       Handle error conditions that occur inside chix functions.
/       Since the chix package can (in theory) be used in a variety
/       of applications, any application-specific error handling
/       should be enclosed in #if/#endif's in chxerror().
/
/    How it works:
/       chxerror() is called from inside other chix functions when they
/       detect an error condition.  The ERROR code describes the kind
/       of error condition, such as:
/          CXE_NULL    chix argument is a null pointer
/          CXE_FREED   chix argument is a freed chix
/          CXE_NOTCHX  chix argument is not a chix at all
/          CXE_NOMEM   insufficient heap memory to allocate a chix
/
/       chxerror() writes an appropriate error message (based on ERROR)
/       to the current "logger()" log file, 
/       and takes appropriate action.  (Sometimes this means just 
/       logging a message.  More serious errors may mean exiting
/       immediately.)
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by: chxcheck(), any many other chix functions.
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  chx/chxerror.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: JV 10/16/92 16:11 Add CXE_SELF, CXE_UNKNOWN. */
/*: CR  8/28/98 15:45 Write errors to "logger()" log file. */

#include <stdio.h>
#include "chixdef.h"
#include "caucus.h"

#define  CAUCUS   0     /* Chix used in Caucus application? */

#if CAUCUS
#include "sysexit.h"
#endif

FUNCTION  chxerror (error, s1, s2)
   int    error;
   char  *s1, *s2;
{
   char   temp[512];
   ENTRY ("chxerror", "");

   switch (error) {

      case (CXE_NORESV):
         sprintf (temp, 
            "CXE_NORESV (%d), cannot allocate reserve memory in '%s'.\n",
                  error, s1);
         logger (1, LOG_FILE, temp);
#if CAUCUS
         sysexit (EXIT_CHX_NORESV);
#endif
         exit  (0);
         break;

      case (CXE_LOMEM):
         sprintf (temp, "CXE_LOMEM (%d), %s '%s'.\n  String='%s'\n",
                  error, "running out of heap memory in", s1, s2);
         logger (1, LOG_FILE, temp);
#if CAUCUS
         sysexit (EXIT_CHX_NOMEM);
#endif
         break;

      case (CXE_NOMEM):
         sprintf (temp, "CXE_NOMEM (%d), all out of heap memory in '%s'.\n",
                  error, s1);
         logger (1, LOG_FILE, temp);
#if CAUCUS
         sysexit (EXIT_CHX_NOMEM);
#endif
         exit  (0);
         break;


      case (CXE_NULL):
         sprintf (temp, 
             "CXE_NULL (%d), chix argument is null pointer in '%s'.\n",
                  error, s1);
         logger (1, LOG_FILE, temp);
         break;

      case (CXE_FREED):
         sprintf (temp, "CXE_FREED (%d), chix argument in '%s' was freed!\n"
             "  Old name of chix was '%s'\n", error, s1, s2);
         logger (1, LOG_FILE, temp);
         break;

      case (CXE_NOTCHX):
         sprintf (temp, "CXE_NOTCHX (%d), argument not a chix in '%s'.\n",
                  error, s1);
         logger (1, LOG_FILE, temp);
         break;

      case (CXE_RESIZE):
         sprintf (temp, "CXE_RESIZE (%d), chix '%s' resized too often.\n",
                  error, s2);
         logger (1, LOG_FILE, temp);
         break;

      case (CXE_NOEND):
         sprintf (temp, "CXE_NOEND (%d), chix '%s' had no 0 terminator!\n",
                  error, s2);
         logger (1, LOG_FILE, temp);
         break;

      case (CXE_SELF):
         sprintf (temp, 
             "CXE_SELF (%d), used chix '%s' twice in function call.\n",
                  error, s2);
         logger (1, LOG_FILE, temp);
                   
      default:
         sprintf (temp, "CXE_UNKNOWN (%d), unknown chix error for chix '%s'.\n",
                  error, s2);
         logger (1, LOG_FILE, temp);
         break;
   }

   RETURN (1);
}
