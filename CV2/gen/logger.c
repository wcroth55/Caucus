
/*** LOGGER.  Log errors from sweb programs.
/
/    logger      (force, code, str);
/    logger_init (path, arg0, program);
/   
/    Parameters:
/       int     force;    (1=force debugging, 0=inherit prev, -1=reset)
/       int     code;     (LOG_ERR, LOG_OUT, or LOG_FILE)
/       char   *str;      (string to be logged)
/
/       char   *path;     (path to socket directory if known)
/       char   *arg0;     (path to program being run)
/       char   *program;  (prefix for log file, e.g. "swd" or "sws"
/                             or "soc")
/    Purpose:
/       Log errors or debugging information from any of the "sweb"
/       programs.
/
/    How it works:
/       Program must call logger_init() to initialize logging.
/
/       Thereafter, calls to logger() write to:
/          stderr     (if LOG_ERR  specified)
/          stdout     (if LOG_OUT  specified)
/          a log file (if LOG_FILE specified)
/       Logical ORs of any or all of these are allowed.
/       
/       Logging occurs if the SOCKET/debug file exists, or if FORCE
/       is true.
/
/       Log files are written in the SOCKET directory if it can be
/       found, otherwise in the system temporary directory.  (Typically
/       /tmp on Unix, and C:\TEMP on NT.)
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  logger.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/14/97 New function. */
/*: CR  5/28/03 Add force = -1 to reset option */

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "chixuse.h"
#include "logger.h"

#if UNIX
#include <unistd.h>
#endif

#define   FUNCTION
#define   NULLSTR(x)  (!(x[0]))

static char dir[256], prog[80];
static int  do_log = -1;

FUNCTION  logger_init (char *path, char *arg0, char *program)
{
   char  *socket_path();

   if (NULLSTR(path)  &&  NULLSTR(arg0))  
         systmpdir (dir, 0);
   else  strcpy    (dir, socket_path (path, arg0));

   strcpy (prog, program);
   do_log = -1;
   
   return (1);
}


FUNCTION  logger (int force, int code, char *str)
{
   FILE      *fp;
   char       temp[200], now[120];
   char      *mytime();
   struct     timeval tv;

   /*** First call, determine if DEBUG file exists. */
   if (do_log < 0) {
      do_log = 0;
      sprintf (temp, "%s/debug", dir);
      if ( (fp = fopen (temp, "r")) != NULL) {
         do_log = 1;
         fclose (fp);
      }
   }

   /*** On any call: log debugging info ? */
   if (force   > 0)  do_log = 1;
   if (force   < 0)  do_log = 0;
   if (do_log == 0)  return(1);

   strcpy  (now, mytime());
   if (code & LOG_ERR)  fprintf (stderr, "%s: %s\n", now, str);
   if (code & LOG_OUT)   printf (        "%s: %s\n", now, str);
   if (code & LOG_FILE) {
      sprintf (temp, "%s/%s%05d.log", dir, prog, (int) getpid());
      nt_chkfile (temp, 0755);
      if ( (fp = fopen (temp, "a")) != NULL) {
         gettimeofday(&tv, NULL);
         fprintf (fp, "%s %3d: %s\n", now, tv.tv_usec/1000, str);
         fclose  (fp);
      }
   }

   return(1);
}
