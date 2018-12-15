
/*** SYS_SUBSERVER.  Spawn off child subserver from master swebd.
/
/    child = sys_subserver (conf, secure, cd);
/ 
/    Parameters:
/       int    child;              (subserver process id)
/       struct sweb_conf_t *conf;  (configuration info)
/       int    secure;             (security code for subserver)
/       int    cd;                 (open connection descriptor)
/
/    Purpose:
/
/    How it works:
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  sweb/syssubse.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/26/97 17:32 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "systype.h"
#include "sweb.h"

#if UNIX
#include <unistd.h>
#include <errno.h>
#endif

#if WNT40
#include <windows.h>
#include <iostream.h>
#endif

#define SLASH(x)  (x=='/'  ||  x=='\\')

FUNCTION  int sys_subserver (
   struct sweb_conf_t *conf,
   int    secure,
   int    cd )
{
   int    child;
   char   temp[256];


#if UNIX
   char   program[256], arg1[256], arg2[32];
   int    rc;

   child = fork();

   if (child < 0)  return (child);   /* parent, failure to spawn. */
   if (child > 0)  return (child);   /* parent, successful spawn. */

   /*** From here the child process builds the info it needs to
   /    exec swebs. */
   sprintf (program, "%s/SWEB/swebs", conf->caucus_path);
   if (SLASH(conf->filename[0]))  strcpy (arg1, conf->filename);
   else
      sprintf (arg1, "%s/SWEB/%s", conf->caucus_path, conf->filename);
   sprintf (arg2, "%d", (secure ^ 0xAA55));

   close (cd);  /* Don't leave it hanging open...! */

   /*** If possible, set the real and effective group ids chosen in
   /    the swebd.conf configuration file.  Each #if section has the
   /    anticipated "correct" system types from systype.h.  These may
   /    be wrong for different versions of a particular operating
   /    system (e.g., Solaris 2.5 and 2.3 may work differently. */
#if LNX12 | DEC32 | IRX53 | SOL25 | SUN41
   seteuid (0);
   rc = setregid (conf->real_group, conf->caucus_group);
#endif
#if SOL24 | BSD21 | AIX41
   seteuid (0);
   setgid  (conf->real_group);
   setegid (conf->caucus_group);
#endif

#if HPXA9
   setresgid (conf->real_group, conf->caucus_group, conf->real_group);
#endif

#if 0
   setgid  (conf->real_group);
#endif

   /*** Set the real userid as desired.  This must be done carefully;
   /    different versions of Unix handle this differently.  Check
   /    the results for each Unix version! */
#if LNX12 | DEC32 | SOL24 | SOL25 | IRX53 | BSD21 | AIX41
   seteuid (0);
   rc = setuid (conf->real_id);
#endif
#if HPXA9 | SUN41
   setuid (0);
   setuid (conf->real_id);
#endif

/* fprintf (stderr, "euid=%d, ruid=%d\n", geteuid(), getuid()); */

   execl (program, "swebs", arg1, arg2, NULL);
   fprintf (stderr, "\nCould not execute swebs! '%s %s %s'\n",
            program, arg1, arg2);
   exit  (1);

#endif


#if WNT40
   STARTUPINFO          startup;
   PROCESS_INFORMATION  procinfo;
   BOOL                 success;
   char                 program[256], arg1[128], cmdline[256], curdir[256];

   GetStartupInfo (&startup);

   sprintf     (program, "%s/SWEB/swebs.exe", conf->caucus_path);
   nt_filename (program);

   strcpy      (arg1, conf->filename);
   nt_filename (arg1);
   if (arg1[1] != ':') sprintf (arg1, "%s/SWEB/%s",
                                conf->caucus_path, conf->filename);
   sprintf (cmdline, "swebs %s %d ", arg1, (secure ^ 0xAA55));
   logger  (0, LOG_FILE, cmdline);

   strcpy      (curdir, conf->caucus_path);
   nt_filename (curdir);

   success = CreateProcess (program, cmdline,
      0, 0, FALSE,
      DETACHED_PROCESS,
      0, /* environment */
      curdir,
      &startup,
      &procinfo);

   return (success ? procinfo.dwProcessId : -1);
#endif

}
