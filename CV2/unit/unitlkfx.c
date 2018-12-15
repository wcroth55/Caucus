
/*** UNIT_LKFX.   Open proper lockmap file for use by unit_lock().
/
/    ok = unit_lkfx (n, nuser, nconf);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    n;           (type of lockmap file: LCNF, LUSR... )
/       Chix   nuser;       (if LUSR, userid of user)
/       int4   nconf;       (conference number, host number, etc.)
/
/    Purpose:
/       For Unix system V and related systems, unit_lock() operates
/       by mapping a particular file into a byte number, and then
/       locking that byte in a selected lock map file.
/
/       Unit_lkfx() ensures the the proper lockmap file is open and
/       ready to be used by unit_lock().
/
/    How it works:
/       N must be one of the types:
/          LUSR    locking a file about a particular user
/          LSYS    locking a system-wide file
/          LCNF    locking a file about a particular conference
/          LHST    locking a file about a particular host
/          LSHP    locking a file about a particular shipping bay
/          LRCV    locking a file about a particular receiving bay
/
/       If N is LSYS, NUSER and NCONF are ignored.
/       If N is LUSR, NUSER must be the user's userid.  NCONF is ignored.
/       For all other cases, NCONF is the number of the particular
/       conference, host, shipping bay, or receiving bay.  NUSER is ignored.
/
/       Unit_lkfx() checks the global locktype[], opens the proper lockmap
/       file (if not already open), and marks locktype[] appropriately.
/ 
/    Returns: 1 on success, 0 on error.
/
/    Error Conditions:
/       Cannot open or create lockmap file.
/ 
/    Side effects: may change data in global locktype[].
/
/    Related functions:
/
/    Called by:  unit_lock()
/
/    Operating system dependencies: no-op for non system-V
/
/    Known bugs:
/
/    Home:  unit/unitlkfx.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:54 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include <sys/types.h>
#include "caucus.h"
#include "handicap.h"
#include "systype.h"
#include "units.h"
#include "unitwipe.h"
#include "rwmacro.h"

extern struct locktype_t locktype[];

#if NUT40
#include <fcntl.h>
#endif

#if WNT40
#include <windows.h>
#include <iostream.h>
#endif

extern Chix   confid;
extern int    debug;

FUNCTION  unit_lkfx (n, nuser, nconf)
   int    n;
   int4   nconf;
   Chix   nuser;
{
   char  *bugtell();
   char   confidstr[200], nuser_str[60];

/*------------------------------Most UNIXes------------------------------*/
#if SYSV | HPXA9 | BSD21 | LNX12 | NUT40 | WNT40 | FBS22 | SUN41
   int    udnum, ret_val;

   ENTRY ("unit_lkfx", "");

   if (nuser==NULL  ||  EMPTYCHX(nuser))  nuser_str[0] = '\0';
   else   ascofchx (nuser_str, nuser, L(0), L(58));

   ret_val = 1;
   if (debug & DEB_LOCK) {
      char temp[300];
      sprintf (temp, "UNIT_LKFX: type=%d, nuser='%s', nconf=%d\n",
                      n, nuser_str, nconf);
      unit_write (XWDB, chxquick(temp,2));
   }

   ascofchx (confidstr, confid, L(0), L(200));

   switch (n) {

      case (LSYS):
         if (locktype[LSYS].fd >= 0)  break;
         sprintf (locktype[n].file, "%s/locksyst", confidstr);
         ret_val =  (unit_lkop (n, locktype[n].file, "", L(0)));
         break;

      case (LUSR):
         if (streq (locktype[LUSR].user, nuser_str))  break;
         udnum = sysudnum (nuser, (debug & DEB_LOCK ? 0 : -1), n);
         sprintf (locktype[n].file, "%s/USER%03d/%s/lockuser",
                   confidstr, udnum, nuser_str);
         ret_val =  (unit_lkop (n, locktype[n].file, nuser_str, L(0)));
         break;

      case (LCNF):
         if (locktype[LCNF].nconf == nconf)  break;
         sprintf (locktype[n].file, "%s/C%04d/lockconf", confidstr, nconf);
         ret_val =  (unit_lkop (n, locktype[n].file, "", nconf));
         break;

      case (LHST):
         if (locktype[LHST].nconf == nconf)  break;
         sprintf (locktype[n].file, "%s/lockh%03d", confidstr, nconf);
         ret_val =  (unit_lkop (n, locktype[n].file, "", nconf));
         break;

      case (LSHP):
         if (locktype[LSHP].nconf == nconf)  break;
         sprintf (locktype[n].file, "%s/H%03d/lockship", confidstr, nconf);
         ret_val =  (unit_lkop (n, locktype[n].file, "", nconf));
         break;

      case (LRCV):
         if (locktype[LRCV].nconf == nconf)  break;
         sprintf (locktype[n].file, "%s/RECV%03d/lockrecv", confidstr, nconf);
         ret_val =  (unit_lkop (n, locktype[n].file, "", nconf));
         break;

      default:
         buglist (bugtell ("Bad case for unitlkfx!", n, nconf, nuser_str));
         sysexit (1);
   }

   if (debug & DEB_LOCK) {
      char temp[300];
      sprintf (temp, "    ret_val=%d, lt.file='%s', lt.user='%s'\n",
                          ret_val, locktype[n].file, locktype[n].user);
      unit_write (XWDB, chxquick(temp, 2));
   }

   RETURN (ret_val);
#endif
}


#if SYSV | HPXA9 | BSD21 | LNX12 | NUT40 | WNT40 | FBS22 | SUN41

#define RDWR  2
#if NUT40
#undef  RDWR
#define RDWR  O_RDWR | O_BINARY
#endif

/*** UNIT_LKOP actually opens the selected type of lockmap file. */

FUNCTION  unit_lkop (n, file, nuser, nconf)
   int    n;
   int4   nconf;
   char   file[], nuser[];
{

/* fd = _sopen (file, _O_RDWR | _O_BINARY | _O_CREAT, 
                      _SH_DENYNO, _S_IREAD | _S_IWRITE); */
#if WNT40
   ENTRY ("unit_lkop", "");

   if (locktype[n].fd >= 0)  CloseHandle (locktype[n].handle);

   locktype[n].handle = CreateFile (file, GENERIC_READ | GENERIC_WRITE,
                                          FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                          0, OPEN_ALWAYS, 0, 0);
   locktype[n].fd = 100;
   if (locktype[n].handle == INVALID_HANDLE_VALUE) {
      locktype[n].fd = -1;
      if (debug & DEB_LOCK)   perror ("unit_lkfx: fd < 0");
   }
#endif

#if UNIX | NUT40
   int    fd, mask;

   ENTRY ("unit_lkop", "");

   if (locktype[n].fd >= 0)  _CLOSE (locktype[n].fd);

   mask = 0700;
   sysbrkignore();
   if ( (fd = open (file, RDWR)) < 0) {
      if ( (fd = creat (file, (mode_t) mask)) >= 0)  close (fd);
      fd = open (file, RDWR);
   }
   sysbrkinit();

   if ((debug & DEB_LOCK)  &&  fd < 0)   perror ("unit_lkfx: fd < 0");
   locktype[n].fd = fd;
#endif

   strcpy (locktype[n].user, nuser);
   locktype[n].nconf = nconf;
   if (locktype[n].fd < 0)  locktype[n].user[0] = locktype[n].nconf = 0;
   RETURN (locktype[n].fd >= 0);
}
#endif
