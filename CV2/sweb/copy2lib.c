/*** COPY2LIB.   Copy a file to the Caucus "library".
/
/    ok = copy2lib (orig, relative, conf);
/   
/    Parameters:
/       int                 ok;         (success?)
/       char               *orig;       (full pathname of "original" file)
/       char               *relative;   (relative pathname of file in library)
/       struct sweb_conf_t *conf;       (server configuration info)
/
/    Purpose:
/       Copy a file into the Caucus library.
/
/    How it works: 
/
/    Returns: full URL of file copied to library
/             NULL on error (bad file name...)
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home: sweb/copy2lib.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/18/96 16:26 New function. */

#include <stdio.h>
#include "systype.h"
#include "chixuse.h"
#include "sweb.h"
#include "rwmacro.h"

#if UNIX | NUT40
#include <sys/types.h>
#include <fcntl.h>
#include "fcntl2.h"
#include <unistd.h>
#define  PERMS  0611
#endif

#if WNT40
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define  PERMS  _S_IWRITE | _S_IREAD
#endif

#define  BUFMAX  4096
#define  SLASH(x)   (x=='/' || x=='\\')


FUNCTION  char *copy2lib (char *orig, char *relative, struct sweb_conf_t *conf)
{
   static char full_url[300];
   char  *pos;
   char   buf[BUFMAX+2];
   char   full[300];
   int    got, f_orig, f_libr;
   ENTRY ("copy2lib", "");

   if (NULLSTR(orig)             ||  NULLSTR(relative) || 
       NULLSTR(conf->caucuslib)  ||  NULLSTR(conf->httplib))   RETURN(NULL);

   /*** Remove all attempts to go outside (above) the library directory,
   /    and replace all ridiculous characters in file names. */
   while (stralter (relative, "../",  "") ) ;
   while (stralter (relative,   " ", "_") ) ;
   while (stralter (relative, "%20", "_") ) ;
   while (stralter (relative,   "*", "_") ) ;
   while (stralter (relative,   "%", "_") ) ;

   /*** Create any sub-directories along the way. */
   for (pos=relative;   *pos;   ++pos) {
      if (SLASH(*pos)) {
         *pos = '\0';
         strcpy   (full, conf->caucuslib);
         strcat   (full, "/");
         strcat   (full, relative);
         sysmkdir (full, 0755, NULL);
         *pos = '/';
      }
   }

   /*** Copy the file. */
   strcpy (full, conf->caucuslib);
   strcat (full, "/");
   strcat (full, relative);
   if ( (f_orig = _OPEN (orig, O_RDONLY | O_BINARY))                  < 0) 
      RETURN(NULL); 

   sysunlink (full);
#if WNT40
   nt_mkfile (full, 0755);
#endif
   if ( (f_libr = _OPEN3(full, O_WRONLY | O_BINARY | O_CREAT, PERMS)) < 0) { 
      _CLOSE  (f_orig);   RETURN(NULL); 
   }

   while ( (got = _READ (f_orig, buf, BUFMAX)) > 0)  _WRITE (f_libr, buf,  got);
   _CLOSE (f_libr);
   _CLOSE (f_orig);
#if UNIX | NUT40
   chmod (full, 0644);
#endif

   /*** Prepare the return value (full URL of library file). */
   strcpy (full_url, conf->httplib);
   strcat (full_url, "/");
   strcat (full_url, relative);
   RETURN (full_url);
}
