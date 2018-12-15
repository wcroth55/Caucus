
/*** SYSAVAIL.   Return the amount of available space on the disk.
/
/    ok = sysavail (path, kbytes, files, error);
/
/    Parameters:
/       int    ok;          (did sysavail succeed in getting # bytes?)
/       char  *path;        (path name on a disk)
/       int4  *kbytes;      (returned number of free disk Kbytes)
/       int4  *files;       (returned number of available files)
/       int   *error;       (error code if sysavail failed)
/
/    Purpose:
/       Many programs need to know if there is enough disk available
/       before proceeding to write a disk file.  Sysavail() returns
/       the best possible guess at the amount of free disk space
/       and number of files that can be created ("file slots") on a
/       particular disk or partition.
/
/    How it works:
/       Sysavail first looks to see if a quota is enabled for this
/       user.  If so, it gets the remaining space & # of files
/       allowed for by the quota, and returns those values.
/
/       If there is no quota implemented on this system, or enabled
/       for this user, it examines the file system or partition that
/       contains the file whose full pathname is PATH.  If it succeeds in
/       finding the number of free bytes and file slots, it returns 1,
/       sets BYTES to the number of free bytes, sets FILES to the
/       number of free file slots, and sets ERROR to 0.
/
/       Since checking free disk space is very expensive on some OS's,
/       we don't really check each time.  In some cases, e.g. the CaucusLink
/       XFP, we need to check disk space frequently.  To avoid unnecessary
/       delays, sysavail() caches the results, and uses the cached data
/       if the most recent call was "recent enough".  sysavail() remembers
/       the time of the last call, and compares it to the present call.
/       After a certain amount of time has elapsed (CACHELIFE), it ditches
/       the cache.
/
/    Implementation Notes:
/       Some systems may not be able to find the number of free file
/       slots, in which case FILES is set to -1.
/
/       Some systems do not provide the block size to programs.  In this
/       case we assume 512 bytes.
/
/       On all Unix, NFS version 2 does not provide free inodes across
/       the network.  If we can't find out if this partition is NFS-
/       mounted, set FILES to -1.
/
/       Otherwise, it returns 0, and puts an error code in ERROR.
/
/    Returns: 1 on success, 0 on failure.
/
/    Error Conditions:  sets one of the following error codes from syserror.h:
/       SE_BADDIR       Bad directory or filename in PATH.
/       SE_BADDEV       Bad device name derived from PATH.
/       SE_NOTIMP       Function not implemented for this system.
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:  application.
/
/    Operating system dependencies: considerable!
/
/    Known bugs:
/
/    Home:  sys/sysavail.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/07/91 15:07 New function. */
/*: CR  8/08/91 14:37 Add missing systems to NONE. */
/*: JV  8/25/91 19:38 Add bug warning. */
/*: CR  6/23/92 13:58 Make RO a SYSV system.  Add quota stuff. */
/*: JV  7/16/92 15:21 Add U4, move include files. */
/*: JV  9/04/92 15:10 Move DYP to SYSV. */
/*: JV 10/14/92 15:40 Move HU to SYSV. */
/*: CV  5/10/93 13:11 Add ']' checking to VV code. */
/*: CP  3/28/93 18:59 Add MV code. */
/*: CP  7/08/93 16:00 Integrate 2.5 changes. */
/*: JV  8/06/93 16:36 Absorb diskfree code, fix minor bugs, remove AOS code. */
/*: JV  9/26/93 13:06 Add ENTRY. */
/*: CR  2/16/94 19:43 Add DX. */
/*: CR 12/09/94 12:39 Add LX; make HU like LX & SU. */
/*: CR  1/10/95 18:56 New code for RO. */
/*: CR  9/26/95 16:48 Return Kbytes, not bytes. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "handicap.h"
#include "systype.h"
#include "syserror.h"
#include "entryex.h"
#include "int4.h"

#include <sys/types.h>

#if SYSV
#include <sys/statvfs.h>
#endif

#if SUN41
#include <sys/vfs.h>
#include <sys/stat.h>
#include <ufs/quota.h>
#include <errno.h>
#endif

#if BSD21 | FBS22
#include <sys/param.h>
#include <sys/mount.h>
#endif

#if LNX12
#include <sys/vfs.h>
#endif

#if HPXA9
#include <sys/vfs.h>
#endif

#if NUT40 | WNT40
#include <windows.h>
#include <iostream.h>
#endif

#define CACHELIFE 120

FUNCTION  sysavail (path, kbytes, files, error)
   char  *path;
   int4  *kbytes, *files;
   int   *error;
{
   int4   now, systime();
   static int  lasterror, lastret;
   static int4 lastcall=0L, lastbytes=0L, lastfiles=0L;

/*---------------------------NT------------------------------------------*/
#if NUT40 | WNT40
   BOOL   success;
   unsigned int sectors_cluster, bytes_sector, freeclus, clusters;
   char     drive[20];

   strncpy (drive, path, 4);
   drive[2] = '\\';
   drive[3] = '\0';

   success = GetDiskFreeSpace (drive, &sectors_cluster, &bytes_sector,
                &freeclus, &clusters);
   if (NOT success)   { *error = SE_BADDIR;   return (0); }

   *kbytes = (sectors_cluster * (bytes_sector/100)) * (freeclus/10);
   *files  = freeclus;

   return (1);
#endif


/*---------------------------UNIX----------------------------------------*/
#if UNIX

#if SUN41 | LNX12 | HPXA9 | BSD21 | FBS22
   struct statfs buffer;
#endif

#if SYSV
   struct statvfs buffer;
#endif

   int    length;
 
   ENTRY ("sysavail", "");

   now = systime();               /* If we made a recent call, use that data. */
   if (now - lastcall < CACHELIFE) {
      *error  = lasterror;
      *kbytes = lastbytes;
      *files  = lastfiles;
      RETURN (lastret);
   }

   lastcall = now;

#if SUN41 | LNX12 | HPXA9 | BSD21 | FBS22
   if (statfs (path, &buffer)) {
#endif
#if SYSV
   if (statvfs (path, &buffer)) {
#endif
      *kbytes = L(1);
      *error  = lasterror = SE_BADDEV;
      lastret = 0;
      RETURN(0);
   }

#if HPXA9 | LNX12
   *kbytes = lastbytes = (buffer.f_bsize/512) * (buffer.f_bavail / 2);
   *files  = lastfiles = buffer.f_ffree;
#endif
#if SYSV | BSD21 | FBS22
   *kbytes = lastbytes = buffer.f_bavail;
   *files  = lastfiles = buffer.f_ffree;
#endif
#if SUN41
   *kbytes = lastbytes = (buffer.f_bsize/512) * (buffer.f_bfree / 2);
   *files  = lastfiles = buffer.f_ffree;
#endif

   if (*files == 0)  *files = lastfiles = -1;
   *error  = lasterror = 0;
   lastret = 1;

   RETURN (1);
#endif

}
