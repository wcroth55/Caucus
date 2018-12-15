/*** SOCKET_PATH   Determine the full pathname of the sweb SOCKET directory.
/
/    path = socket_path (caucus_path, arg0);
/   
/    Parameters:
/       char *path;          (returned path)
/       char *caucus_path;   (if not empty, caucus home dir)
/       char *arg0;          (argv[0])
/
/    Purpose:
/       Figure out the full pathname of the sweb SOCKET directory.
/
/    How it works:
/
/    Returns: pointer to static area containing socket path.
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home: socketpath.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/21/96 13:36 New function. */
/*: CR  7/11/96 15:36 Add additional heuristics to get socket path. */
/*: CR  4/11/03 Use <errno.h>. */

#include <stdio.h>
#include "chixuse.h"

#define  NOT   !
#define  NULLSTR(x)  (!(x[0]))
#define  FUNCTION

#if UNIX | NUT40
#include <unistd.h>
#include <errno.h>
#endif

#if WNT40
#include <direct.h>
#endif

#define  MAXVAL     300
#define  SLASH(x)  (x=='/'  ||  x=='\\')

FUNCTION  char *socket_path (char *caucus_path, char *arg0)
{
   static char value[MAXVAL];
   int         last, slashes;
   char       *ptr;
   FILE       *fp;

   /*** If CAUCUS_PATH is supplied, use it. */
   if (NOT NULLSTR (caucus_path)) {
      strcpy  (value, caucus_path);
      strcat  (value, "/SOCKET");
      return  (value);
   }

   /*** If ARG0 has a full pathname in it, use that to determine
   /    the base of the path to the SOCKET directory. */
   strcpy (value, arg0);
   for (ptr=value, slashes=0;   *ptr;   ++ptr)  if (SLASH(*ptr))  ++slashes;

   if (slashes >= 2) {
      ptr = value + strlen(value) - 1;
      while (*ptr  &&  NOT SLASH(*ptr))  --ptr;
      if    (*ptr)                       --ptr;
      while (*ptr  &&  NOT SLASH(*ptr))  --ptr;
      *ptr = '\0';
      strcat (value, "/SOCKET");
      return (value);
   }

   /*** Otherwise, the current working directory should be
   /    something like "/home/caucus/SWEB".  Trim off the last
   /    dirname, and build the socket path from it. */
   if (getcwd (value, MAXVAL) == NULL) {
/*    fprintf (fp, "errno=%d\n", errno);  */
   }
   last = strlen(value) - 1;
   if (SLASH(value[last]))  value[last--] = '\0';

   while (last > 0  &&  NOT SLASH(value[last]))  --last;
   value[last] = '\0';

   strcat (value, "/SOCKET");
   return (value);
}
