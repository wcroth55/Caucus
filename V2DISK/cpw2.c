
/*** CPW2.   Caucus Password utility, version for 4.0
/
/    Invoked as:
/       cpw2
/
/    Arguments:
/       passfile     full path name of password file
/       lockflag     if "-l", do not lock password file (only applies for "v")
/
/    Purpose:
/       CPW2 is a prototype external program, meant to be run from
/       Caucus, for manipulating the password file for
/       an HTTP access authorization controlled directory.
/
/       As supplied, CPW2 acts on a "flat" (sometimes called "NCSA-style")
/       password file.  Feel free to rewrite this code as necessary
/       to work with your own site's methods of handling web access
/       authorization ids and passwords.  It is specifically meant
/       as a prototype for that purpose.
/
/       The full CPW2 program source consists of three files:
/          cpw2.c
/          pwactions.c
/          xorstr.c
/
/    How it works:
/       Reads several lines from stdin to determine its precise function,
/       as described below.
/
/       1. Read the encrypted "trust" code.  (Meant to prevent malicious 
/          users or programs from invoking CPW2 w/o authorization.)
/
/       2. Read the pathname of the password file.  (May be irrelevant
/          depending on your system, but a line must still be read.)
/
/       3. Read the locking status ("on" or "off")
/
/       4. Read the function (add, change, delete, or verify).
/
/       5. Read the userid.
/
/       6. Read the password.
/
/       7. Perform appropriate function, and write two-digit success code
/          to stdout.  ("00" is success, all other codes indicate an
/          error from the #define BAD_... codes below.)
/
/       NOTE!  See "OS Dependencies" section below.
/
/    Exit status: 0
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  cpw1.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/15/97 17:32 New function, based on old cpw1. */

#include <stdio.h>
#include <time.h>
#define  FUNCTION
typedef  unsigned int   int4;  /* 4 byte unsigned integer on this platform */

#define  BAD_NOARG    1    /* no arguments! */

FUNCTION int main (argc, argv)
   int   argc;
   char *argv[];
{
   char   trust[100], pw[100], user[100], func[100], passfile[256], lockon[100];
   int4   code, now;
   int    result, is_lock;
   unsigned int xor_str();

   umask(022);

   /*** Read all input. */
   user[0] = '\0';
   fgets (trust,    100, stdin);
   fgets (passfile, 256, stdin);
   fgets (lockon,   100, stdin);
   fgets (func,     100, stdin);
   fgets (user,     100, stdin);
   fgets (pw,       100, stdin);
   if (user[0] == '\0')        exit_with (BAD_NOARG);

   trust    [strlen(trust)    -1] = '\0';
   passfile [strlen(passfile) -1] = '\0';
   lockon   [strlen(lockon)   -1] = '\0';
   func     [strlen(func)     -1] = '\0';
   user     [strlen(user)     -1] = '\0';
   pw       [strlen(pw)       -1] = '\0';

   /*** Check "trust code". */
   sscanf (trust, "%8x", &code);
   code = xor_str (code, "caucus");
   code = xor_str (code, user);
   code = xor_str (code, pw);
   now  = time(NULL);
   if (now - code > 60)   exit_with (/*BAD_NOARG*/ 20);

   is_lock = (strcmp (lockon, "on") == 0);

   result = pw_actions (func, passfile, user, pw, is_lock);

   exit_with (result); 
}


FUNCTION exit_with (code)
   int   code;
{
   FILE   *fp;

   printf  ("%02d\n", code);
   exit    (0);
}
