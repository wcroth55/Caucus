
/*** GET_ENT.  Read an entry from the passwd file open on PASS.
/
/    code = get_ent (pass, name, passwd, line);
/   
/    Parameters:
/       int     code;     (return code)
/       FILE   *pass;     (open password file)
/       char   *name;     (returned userid)
/       char   *passwd;   (returned encrypted password)
/       char   *line;     (returned entire text line)
/
/    Purpose:
/       Read an entry from an NCSA-style "flat" password file,
/       and extract the userid and encrypted password fields.
/
/    How it works:
/       The password file must already be open on PASS.
/
/    Returns: 1 on success
/             0 on end-of-file.
/
/    Known bugs:
/
/    Home:  gen/getent.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/14/97 11:56 New function. */

#include <stdio.h>

#define  FUNCTION

FUNCTION  get_ent (FILE *pass, char *name, char *passwd, char *line)
{
   char  *xn, *xl;

   if (fgets (line, 300, pass) == NULL)   return (0);
   line [strlen(line) - 1] = '\0';

   if (line[0] == '#'  ||  line[0] == '+') {
      name[0] = passwd[0] = '\0';
      return (1);
   }

   /*** Pluck out user name. */
   for (xn=name, xl=line;  (*xl) && (*xl != ':');   ++xl)   *xn++ = *xl;
   *xn = '\0';

   /*** Pluck out encrypted password. */
   for (xn=passwd, ++xl;   (*xl) && (*xl != ':');   ++xl)   *xn++ = *xl;
   *xn = '\0';

   return (1);
}
