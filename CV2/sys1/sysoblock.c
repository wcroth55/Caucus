
/*** SYSOBLOCK.   Lock or unlock an object.
/
/    ok = sysoblock (obj, lock, readwrite, path);
/
/    Parameters:
/       int    ok;          (success?)
/
/       Object obj;         (object to be locked)
/       int    lock;        (1=>lock,  0=>unlock)
/       int    readwrite;   (1=>lock for writing, 0=>lock for reading)
/       char  *path;        (path to caucus home directory)
/
/    Purpose:
/       Provides a form of "advisory locking" for entire CML objects
/       for cooperating processes.  Does most of the work of
/       the CML function $ob_lock() and $ob_unlock().
/
/    How it works:
/       Each request to lock an object tries to open (or create) a
/       lockfile (object name + trailing ".l"), and lock byte 0
/       of that file.  If the lock fails (due to a previous lock by
/       another process), the lockfile is closed.
/
/    Returns: 1 on success
/             0 on failure (file already locked)
/            -1 on error   (bad object, bad path, file cannot be opened, etc.)
/
/    Operating system dependencies: considerable!
/
/    Known bugs:
/
/    Home:  sys1/sysoblock.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/15/98 14:18 New function. */

#include <stdio.h>
#include "systype.h"
#include "handicap.h"
#include "int4.h"
#include "object.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if UNIX
#include <unistd.h>
#endif

#if WNT40
#include <windows.h>
#include <iostream.h>
#endif

FUNCTION  sysoblock (Object obj, int lock, int readwrite, char *path)
{
   char   lockfile[512];

   if (obj == NULL)                    return (-1);
   if (    lock  &&      obj->lockfd)  return ( 1);
   if (NOT lock  &&  NOT obj->lockfd)  return ( 1);

#if UNIX
#define  RDWR  2
 {
   struct flock alock = { 0, SEEK_SET, 0, 1 };
   int          fd, code;

   if (lock) {
      sprintf (lockfile, "%s/OBJECTS/%s.l", path, obj->name+1);
      if ((fd = open (lockfile, RDWR)) < 0) {
         if ( (fd = creat (lockfile, (mode_t) 0700)) >= 0)  close (fd);
         fd = open (lockfile, RDWR);
      }
      if (fd < 0)  return (-1);
   
      alock.l_type  = (readwrite ? F_WRLCK : F_RDLCK);
      alock.l_start = 0;
      code = fcntl (fd, F_SETLK, &alock);
      if (code < 0)               { close (fd);   return (0); }

      obj->lockfd = fd;
   }
   else {
      alock.l_type  = F_UNLCK;
      alock.l_start = 0;
      fcntl (obj->lockfd, F_SETLK, &alock);
      close (obj->lockfd);
      obj->lockfd = 0;
      sprintf (lockfile, "%s/OBJECTS/%s.l", path, obj->name+1);
      unlink  (lockfile);
   }
   return (1);
 }
#endif


#if WNT40
 {
   OVERLAPPED overlap;
   DWORD      flags;
   BOOL       success;
   HANDLE     fd;

   overlap.Offset     = 0;
   overlap.OffsetHigh = 0;

   if (lock) {
      sprintf (lockfile, "%s/OBJECTS/%s.l", path, obj->name+1);
      fd = CreateFile (lockfile, GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                 0, OPEN_ALWAYS, 0, 0);
      if (fd == INVALID_HANDLE_VALUE)  return (-1);
   
      flags   = LOCKFILE_FAIL_IMMEDIATELY | 
                (readwrite ? LOCKFILE_EXCLUSIVE_LOCK : 0);
      success = LockFileEx (fd, flags, 0, 1, 0, &overlap);
      if (success != 0)
         { CloseHandle (fd);   return (0); }

      obj->lockfd = (long) fd;
   }

   else {
      UnlockFileEx ((HANDLE) obj->lockfd, 0, 1, 0, &overlap);
      CloseHandle  ((HANDLE) obj->lockfd);
      obj->lockfd = 0;
   }
   return (1);
 }
#endif
}
