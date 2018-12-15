
/*** FUNC_SHELL.   Evaluate a $shell() or related CML function.
/
/    func_shell (result, output, synch, eff_real, arg, conf);
/ 
/    Parameters:
/       Chix                result;   (append result here)
/       int                 output;   (0=>none, 1=>eng, 2=>euc, 3=>sjis)
/       int                 synch;    (1=>synchronous, 0=>asynch)
/       int                 eff_real; (1=>effective id, 0=>real id)
/       Chix                arg;      (function argument)
/       struct sweb_conf_t *conf;     (configuration args)
/
/    Purpose:
/       Evaluate a shell command, and optionally append the
/       output to RESULT.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/ 
/    Known bugs:
/       WNT40 bug: what happens if $shell() task never terminates?
/       Check language codes!
/
/    Home:  funcshell.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/12/95 17:48 New function. */
/*: CR  3/26/95 10:28 Convert to chix. */

#include <stdio.h>
#include "systype.h"
#include "chixuse.h"
#include "sweb.h"

#if UNIX
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#if WNT40
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <process.h>
#include <windows.h>
#include <iostream.h>
#endif

#define  USE_SELECT   1  /* Make 0 if "select()" function call does not work. */
#define  MAXTEMP   1024

FUNCTION  func_shell (Chix result, int output, int synch, int eff_real,
                      Chix arg, struct sweb_conf_t *conf)
{
   int    tube[2], got, child_pid, init;
   char   temp[MAXTEMP+2];

#if UNIX
   fd_set rfds;
   struct timeval tv;
   int    retval, lang;
   int4   pos;
   ENTRY ("func_shell", "");

   /*** Asynchronous operation ignores any output. */
   if (NOT synch) {
      if (fork() > 0)   ;
      else {
#if SOL24 | HPXA9
         if (eff_real)  setuid (geteuid());
         else           setuid (getuid ());
#else
         if (eff_real)  setreuid (geteuid(), -1);
         else           setreuid (-1, getuid());
#endif
         ascofchx (temp, arg, L(0), L(MAXTEMP));
         execl    ("/bin/sh", "sh", "-c", temp, NULL);
         exit (1);
      }
      RETURN(1);
   }

   /*** Synchronous, no output... */
   if (output == 0) {
      if ( (child_pid = fork()) > 0)   waitpid ((pid_t) child_pid, NULL, 0);
      else {
#if SOL24 | HPXA9
         if (eff_real)  setuid (geteuid());
         else           setuid (getuid ());
#else
         if (eff_real)  setreuid (geteuid(), -1);
         else           setreuid (-1, getuid());
#endif
         ascofchx (temp, arg, L(0), L(MAXTEMP));
         execl    ("/bin/sh", "sh", "-c", temp, NULL);
         exit   (1);
      }
      RETURN(1);
   }

   /*** Synchronous, with output (and possible output processing) 
   /    Determine language translation of output from shell command. */
   lang = (output==1 ? conf->diskform : output);
  
   /*** We need to 'pipe' the output from the command back to this
   /    process to feed to the RESULT. */
   pipe (tube);

   /*** Parent.  Spawn off a child to execute ARG.  Read the output
   /    and append to RESULT. */
   if ( (child_pid = fork()) > 0) {
      close (tube[1]);

#if USE_SELECT
      FD_ZERO(&rfds);
      FD_SET (tube[0], &rfds);
#endif
      for (init=0;  (got = read (tube[0], temp, MAXTEMP)) != 0;   init=1) {

         /*** If there was nothing to read, but the pipe has not yet
         /    been closed, wait (intelligently with select(), or
         /    stupidly with sleep()) until there is something to read. */
         if (got < 0) {
#if USE_SELECT
            tv.tv_sec = 5;   tv.tv_usec = 0;
            retval = select(tube[0]+1, &rfds, NULL, NULL, &tv);
#else
            sleep (1);
#endif
         }
         else {
            temp[got] = '\0';
            to_jix (result, temp, init, lang+1);
         }
      }
      close (tube[0]);
      waitpid ((pid_t) child_pid, NULL, 0);
   }

   /*** Child.  Connect stdout to upstream end of TUBE, and execute
   /    ARG in a shell. */
   else {
      dup2     (tube[1], 1);
      close    (tube[0]);
#if SOL24 | HPXA9
      if (eff_real)  setuid (geteuid());
      else           setuid (getuid ());
#else
      if (eff_real)  setreuid (geteuid(), -1);
      else           setreuid (-1, getuid());
#endif
      ascofchx (temp, arg, L(0), L(MAXTEMP));
      execl    ("/bin/sh", "sh", "-c", temp, NULL);
      exit (1);
   }

   RETURN(1);
#endif


/*----------------------------WNT40--------------------------------------*/
#if WNT40
   STARTUPINFO          startup;
   PROCESS_INFORMATION  procinfo;
   BOOL                 success;
   HANDLE               rpipe, wpipe, in_bucket, out_bucket;
   SECURITY_ATTRIBUTES  sa;
   int                  xfer, lang;
   int4                 pos;
   int                  wait4, lasterr;
   char                 cmdline[512], program[256];
   ENTRY ("func_shell", "");

   GetStartupInfo (&startup);
   startup.dwFlags = startup.dwFlags | STARTF_USESTDHANDLES;

   /*** Define a security attribute that allows inheritance. */
   sa.nLength = sizeof (SECURITY_ATTRIBUTES);
   sa.lpSecurityDescriptor = 0;
   sa.bInheritHandle       = TRUE;

   out_bucket = CreateFile ("NUL", GENERIC_WRITE, 0, &sa, OPEN_ALWAYS, 0, 0);
   in_bucket  = CreateFile ("NUL", GENERIC_READ,  0, &sa, OPEN_ALWAYS, 0, 0);
   if (in_bucket == INVALID_HANDLE_VALUE) {
      chxcat (result, CQ("error creating bit bucket"));
      RETURN (0);
   }
   startup.hStdError = out_bucket;
   startup.hStdInput = in_bucket;

   /*** $silent() and $asynch() just write to NUL. */
   if (NOT synch)  output = 0;

   if (output == 0)   startup.hStdOutput = out_bucket;

   else if (output==1) {
      CreatePipe (&rpipe, &wpipe, &sa, 0);
      startup.hStdOutput = wpipe;
      lang = conf->diskform;
   }
  
   /*** $jshell(type cmd) */
   else if (output > 1) {
      CreatePipe (&rpipe, &wpipe, &sa, 0);
      startup.hStdOutput = wpipe;
      lang = output;
   }


   /*** Get the program name and command line. */
/*
   ascofchx    (cmdline, arg, 0, 512);
   nt_filename (cmdline);
   strtoken    (program, 1, cmdline);
*/
   ascofchx    (cmdline, arg, 0, 512);
   strtoken    (program, 1, cmdline);
   nt_filename (program);

   /*** Run it as a detached process, inheriting the changed stdout. */
   success = CreateProcess (program, cmdline,
      0, 0, TRUE,
/*    DETACHED_PROCESS, */
      CREATE_NEW_CONSOLE,
      0, /* environment */
      0, /* current directory */
      &startup,
      &procinfo);


   if (synch) 
      wait4 = WaitForSingleObject (procinfo.hProcess, INFINITE);

   /*** $shell() reads from the pipe and appends to RESULT. */
   if (output > 0) {
      CloseHandle (wpipe);
      for (init=0;
           ReadFile (rpipe, temp, MAXTEMP, &xfer, (LPOVERLAPPED) NULL);
           init=1) {
         if ( xfer <= 0)  break;
         temp[xfer] = '\0';
         to_jix (result, temp, init, lang+1);
      }
      lasterr = GetLastError();
      CloseHandle (rpipe);
   }

   CloseHandle (in_bucket);
   CloseHandle (out_bucket);

#endif
   RETURN(1);
}
