
/*** PW_EXTERN.   Perform add, change, delete, verify actions on 
/                 an httpd password database, via external program.
/
/    code = pw_extern (action, passfile, id, pw, lock, prog);
/   
/    Parameters:
/       int      code;      (return code)
/       char    *action;    ("add", "change", "delete", "verify")
/       char    *passfile;  (full pathname of password file)
/       char    *id;        (userid)
/       char    *pw;        (password)
/       int      lock;      (lock passfile?)
/       char    *prog;      (external program)
/
/    Purpose:
/       Implement 4 actions on an an httpd password database, via
/       an external program.  The actions are "add" an id & pw,
/       "change" the pw on an existing id, "delete" an id, and
/       "verify" the pw for an id.
/
/    How it works:
/       ACTION controls which action is performed.  
/
/       PASSFILE contains the full pathname of the password file.
/
/       ID is the userid in question.  PW is the password.
/
/       LOCK means use advisory locking to enforce one-user write
/       locking and multi-user read (shared) locking on the
/       password file.  If LOCK is 0, no locking is performed.
/
/    Returns: 0 on success
/             one of the positive error codes #define'd 
/             in pw_actions() on error.
/
/    Known bugs:
/
/    Home:  pwextern.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/15/97 22:35 New function. */

#include <stdio.h>
#include <time.h>
#include "systype.h"
#include "chixuse.h"
#include "sweb.h"
#include "int4.h"

#if UNIX
#include <unistd.h>
#endif

FUNCTION int pw_extern (char *action, char *passfile, char *id, char *pw,
                         int   lock,  char *prog)
{
#if UNIX
   int4  now, code;
   int   to_child[2], from_child[2], i, status, got, result;
   char  what[512], reply[20];
   unsigned int xor_str();
   ENTRY ("pw_extern", "");

   /*** Build trust code. */
   now  = time(NULL);
   code = xor_str ((uint4) now,  pw);
   code = xor_str ((uint4) code, id);
   code = xor_str ((uint4) code, "caucus");

   sprintf (what, "%08x\n%s\n%s\n%s\n%s\n%s\n", 
            code, passfile, (lock ? "on" : "off"), action, id, pw);

   pipe (  to_child);
   pipe (from_child);

   /*** Parent.  Spawns off a child to execute PASSPROG.
   /    Send the userid etc. info down the pipe. */
   if (fork() > 0) {
      close (  to_child[0]);
      close (from_child[1]);
      write (  to_child[1], what, strlen(what));
      close (  to_child[1]);

      /*** Insist on reading some data from the child. */
      strcpy (reply, "12\n");
      for (i=0;   i<30;   ++i) {
         if ( (got = read (from_child[0], reply, 10)) > 0)  break;
         sleep (3);
      }
    
      wait  (&status);
      close (from_child[0]);
   }

   /*** Child.  Connect stdin to downstream end of pipe, stdout to
   /    upstream end, and execute PROG in a shell. */
   else {
      dup2  (  to_child[0], 0);
      dup2  (from_child[1], 1);
      close (  to_child[1]);    
      close (from_child[0]);
      execl (prog, prog, NULL);
/*    execl ("/bin/sh", "sh", "-c", progline, NULL); */
      exit  (1);
   }

   sscanf (reply, "%2d", &result);
   RETURN (result);
#endif

}
