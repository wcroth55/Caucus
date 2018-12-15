
/*** SYSLOCKER.   Lock or unlock a byte in a lockmap file.
/
/    ok = syslockfile (pathname, filename);
/
/    ok = syslocker (lock, readwrite, bytenum);
/
/    Parameters:
/       int    ok;          (success?)
/
/       char  *pathname;    (path of file to lock/unlock)
/       char  *filename;    (file in path to lock/unlock)
/
/       int    lock;        (1=>lock,  0=>unlock)
/       int    readwrite;   (1=>write, 0=>read)
/       int4   bytenum;     (byte to lock/unlock)
/
/    Purpose:
/       Provides a form of "advisory locking" for cooperating
/       processes.  Allows locking and unlocking of a byte "semaphore"
/       in a single file.
/
/    How it works:
/       syslockfile() must be called first, to pass in the path and
/       file names of a file to be used as a lock map.  Typically
/       syslockfile() is only called once.  (If it is called a second
/       time, all previously locked locks are immediately unlocked.)
/
/       syslocker() is called to lock or unlock any particular byte
/       in the lock map file.  Locks may be write (exclusive) or
/       read (shared).
/
/    Returns: syslockfile()
/                1 on success, 0 on failure (file cannot be opened)
/
/             syslocker()
/                1 if byte has been locked, 0 otherwise (typically
/                  because someone else has locked it).
/
/    Operating system dependencies: considerable!
/
/    Known bugs:      none
/
/    Home:  sys1/syslocker.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/10/98  0:04 New function.  Cannibalized from unit_lock etc. */

#include <stdio.h>
#include "systype.h"
#include "handicap.h"
#include "int4.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if UNIX
#include <unistd.h>
static int    sf_fd = -1;
#endif

#if WNT40
#include <windows.h>
#include <iostream.h>
static HANDLE sf_fd = -1;
#endif


FUNCTION  syslockfile (char *pathname, char *filename)
{
   char   fullpath[512];

#if WNT40
   if (sf_fd >= 0)  CloseHandle (sf_fd);
#endif
#if UNIX
   if (sf_fd >= 0)  close       (sf_fd);
#endif
   sf_fd = -1;

   strcpy (fullpath, pathname);
   strcat (fullpath, "/");
   strcat (fullpath, filename);

#if WNT40
   sf_fd = CreateFile (fullpath, GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                 0, OPEN_ALWAYS, 0, 0);
   if (sf_fd == INVALID_HANDLE_VALUE)  return (0);
#endif

#if UNIX
#define  RDWR  2
   if ( (sf_fd = open (fullpath, RDWR)) < 0) {
      if ( (sf_fd = creat (fullpath, (mode_t) 0700)) >= 0)  close (sf_fd);
      sf_fd = open (fullpath, RDWR);
   }
   if (sf_fd < 0)  return (0);
#endif

   return (1);
}


FUNCTION  syslocker (int lock, int readwrite, int4 bytenum)
{
#if WNT40
   OVERLAPPED overlap;
   DWORD      flags;
   BOOL       success;

   if (sf_fd < 0)  return (0);

   overlap.Offset     = bytenum;
   overlap.OffsetHigh = 0;
   if (lock) {
      flags   = LOCKFILE_FAIL_IMMEDIATELY | 
                (readwrite ? LOCKFILE_EXCLUSIVE_LOCK : 0);
      success = LockFileEx (sf_fd, flags, 0, 1, 0, &overlap);
      return (success ? 0 : -1);
   }

   else {
      UnlockFileEx (sf_fd, 0, 1, 0, &overlap);
      return (1);
   }
#endif

#if UNIX
   struct flock alock = { 0, SEEK_SET, 0, 1 };

   if (sf_fd < 0)  return (0);

   if (lock) alock.l_type = (readwrite ? F_WRLCK : F_RDLCK);
   else      alock.l_type = F_UNLCK;
   alock.l_start  = bytenum;
   return (fcntl (sf_fd, F_SETLK, &alock) >= 0);
#endif

   return (0);
}
