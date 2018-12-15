
/*** SYSEDITOR.  Invoke user-selected text editor on a file.
/
/    ok = syseditor (file, editstr);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    file;        (unit number)
/       Chix   editstr;     (command line or macro expression to run editor)
/
/    Purpose:
/       SYSEDITOR invokes the editor selected in EDITSTR on the file
/       numbered FILE.
/
/    How it works:
/       If EDITSTR contains a macro expression, SYSEDITOR substitutes
/       the file name in for argument "$2".  Otherwise, it just tacks
/       the file name onto the end of EDITSTR.
/
/    Returns: 2 if the file was successfully edited.
/             1 if the editor exists but the file was not changed.
/             0 if the editor could not be accessed.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  editfile()
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  sys/sysedito.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include <sys/types.h>
#include "caucus.h"
#include "sys.h"
#include "systype.h"
#include "units.h"

#if UNIX
#include <unistd.h>
#endif

extern struct unit_template units[];
extern struct termstate_t   term_in_caucus, term_original;
extern struct flag_template flag;
extern int                  debug;

FUNCTION  syseditor (file, editstr)
   int    file;
   Chix   editstr;
{
#define   CSIZE   L(300)
   char   command[CSIZE];

/*----------------------------------SYSV-ish Unixes------------------*/
#if SYSV | HPXA9 | BSD21 | LNX12 | FBS22
   char  filename[256];
   int   status, child, writable, mask;

   ENTRY ("syseditor", "");

   strcpy (filename, units[file].nameis);
   writable = (access (filename, 6) == 0);
   if (NOT writable)  chmod (filename, 0777);

   systurn (&term_in_caucus, GET_TERM, 0);
   systurn (&term_original,  SET_TERM, 0);
   child  = fork();
   if      (child == -1) {
      systurn (&term_in_caucus, SET_TERM, 0);
      RETURN (0);
   }

   /*** The parent task waits for the child to terminate. */
   else if (child >   0)
      while (wait (&status) != child)  { if (syshngtest()) break; }
 
   /*** The child task reverts to the Real uid, and exec's the
   /    selected editor. */
   else {
      setuid ((uid_t) getuid());
      if (chxindex (editstr, L(0), CQ("$2")) > 0) {
         ascofchx (command,  editstr, L(0), CSIZE);
         while  (stralter (command, "$2", filename)) ;
      }
      else
         sprintf (command, "%s %s", ascquick(editstr), filename);
      execl ("/bin/sh", "sh", "-c", command, NULL);
      _exit (1);
   }

   /*** If the child process exited with a status other than 0, we
   /    couldn't find the desired editor. */
   systurn (&term_in_caucus, SET_TERM, 0);

   if (NOT writable) chmod (filename, 0700);

   mask = 0xFD00;
#if LNX12
   mask = 0xFE00;
#endif
   RETURN ( (status & mask) ? 0 : 2);
#endif


/*---------------------Berkeley Unix----------------------------------*/
#if SUN41
   char  filename[80];
   int   status, child, writable;

   ENTRY ("syseditor", "");

   strcpy (filename, units[file].nameis);
   writable = (access (filename, 6) == 0);
   if (NOT writable)  chmod (filename, 0777);

   systurn (&term_in_caucus, GET_TERM, 0);
   systurn (&term_original,  SET_TERM, 0);
   child  = vfork();
   if      (child == -1)  RETURN (0)

   /*** The parent task waits for the child to terminate. */
   else if (child >   0)
      while (wait (&status) != child)  { if (syshngtest()) break; }
 
   /*** The child task reverts to the Real uid, and exec's the
   /    selected editor. */
   else {
      setreuid ((uid_t) getuid(), (uid_t) getuid());
      if (chxindex (editstr, L(0), CQ("$2")) > 0) {
         ascofchx (command, editstr, L(0), CSIZE);
         while  (stralter (command, "$2", filename)) ;
      }
      else
         sprintf (command, "%s %s", ascquick(editstr), filename);
      execl ("/bin/sh", "sh", "-c", command, NULL);
      _exit (1);
   }

   /*** If the child process exited with a status other than 0, we
   /    couldn't find the desired editor. */
   systurn (&term_in_caucus, SET_TERM, 0);
   if (NOT writable) chmod (filename, 0700);
   RETURN ( (status & 0xFF00) ? 0 : 2);
#endif


/*---------------------NT----------------------------------------*/
#if NUT40 | WNT40
   char  filename[256];
   int   status, child, writable, mask;

   strcpy (filename, units[file].nameis);

   if (chxindex (editstr, L(0), CQ("$2")) > 0) {
      ascofchx (command, editstr, L(0), CSIZE);
      while  (stralter (command, "$2", filename)) ;
   }
   else
      sprintf (command, "%s %s", ascquick(editstr), filename);

   system (command);
   return (2);
#endif

}
