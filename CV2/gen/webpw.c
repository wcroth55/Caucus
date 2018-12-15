
/*** WEBPW.   Check web password file for user name & password combination.
/
/    ok = webpw (pwfile, name, password);
/   
/    Parameters:
/       int   ok;           (return status)
/       Chix  pwfile;       (full pathname of web password file)
/       Chix  name;         (userid)
/       Chix  password;     (supplied password)
/
/    Purpose:
/       Determine if a particular user name and password combination
/       is valid.
/
/    How it works:
/
/    Returns:
/       0                 success, name & password are correct
/       3  (BAD_NOENTRY)  userid does not exist
/       4  (BAD_OLDPW)    bad password
/       9  (BAD_NOREAD)   can't read  password file
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  webpw.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/29/96 17:29 New function. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include <sys/types.h>
#include <sys/stat.h>

#if UNIX | NUT40
#include <unistd.h>
#endif

#define  BAD_NOENTRY  3    /* userid does not exist (chg password) */
#define  BAD_OLDPW    4    /* bad old password */
#define  BAD_NOREAD   9    /* can't read  password file */

FUNCTION int webpw (Chix pwfile, Chix name, Chix password)
{
   char   passfile[200], user[200], id[200], oldpw[200], passwd[200],
          line[300];
   FILE  *fp, *pass;
   char  *crypt();

   ascofchx (passfile, pwfile,   0, 200);
   ascofchx (user,     name,     0, 200);
   ascofchx (oldpw,    password, 0, 200);

#if UNIX | NUT40
   if ( (pass = fopen (passfile, "r"))  == NULL)   return (BAD_NOREAD);
#endif
#if WNT40
   if ( (pass = fopen (passfile, "rt")) == NULL)   return (BAD_NOREAD);
#endif

   while (get_ent (pass, id, passwd, line)) {

      if (strcmp  (user, id) == 0)  {
         fclose (pass);
         if (strncmp (passwd, crypt (oldpw, passwd), 13) == 0)  return (0);
         return (BAD_OLDPW);
      }

   }
   fclose (pass);
   return (BAD_NOENTRY);
}
