
/*** SYSUDNUM.   Return user-directory number for a given userid.
/
/    Under Unix system V, a directory may only contain up to 1000
/    sub-directories.  Thus it has been necessary to partition
/    the Caucus directories for each userid into sets, e.g.
/    the USER directory has been replaced with:
/
/        USER001
/        USER002
/        etc.
/
/    SYSUDNUM is handed a userid, and returns the number of the 
/    USERnnn directory that contains the directory for that userid.
/    If the userid is the empty string, SYSUDNUM returns 0 immediately.
/
/    If no such directory exists, SYSUDNUM looks at the value of CREATE:
/       If >  0, SYSUDNUM creates the directory for that userid.
/       if == 0, SYSUDNUM returns 0 and complains.
/       if <  0, SYSUDNUM returns, silently.
/
/    Any complaints or problems are also logged to the bugslist file.
/
/    For non-Unix systems, SYSUDNUM always returns 1.  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CR 10/07/01 15:00 Raise userid length. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

extern Chix  confid;
extern int   debug;

FUNCTION  sysudnum (uid_chx, create, diagnose)
   Chix   uid_chx;
   int    create, diagnose;
{

#if UNIX | NUT40 | WNT40

/*** Userid & directory number "cache", so we don't have to go look
/    up this information every time! */
#define  UCSIZE  10
static struct uc_t {
   int4   lastuse;
   char   id[MAX_USERID];
   short  dnum;
} uc[UCSIZE];

   char   userdir[100], textdir[100];
   char   uid[160], confid_str[200];
   static short i0 = 0;
   short  i, o;
   int4   d;
   int4   systime();
   char  *bugtell();

   ENTRY ("sysudnum", "");

   if (uid_chx == nullchix) RETURN (0);

   ascofchx (uid,        uid_chx, L(0), L(160));
   ascofchx (confid_str, confid,  L(0), L(200));

   if (NULLSTR(uid))  RETURN (0);

   d = (int4) diagnose;
   if (debug & DEB_OBJC) {
      sprintf (userdir, "In SYSUDNUM: uid='%s', create=%d\n", uid, create);
      unit_write (XWDB, CQ(userdir));
      for (i=0;   i<UCSIZE;   ++i)   if (uc[i].lastuse > L(0)) {
         sprintf (userdir, "   %2d %03d %9d '%s'\n", i, uc[i].dnum, 
                               uc[i].lastuse, uc[i].id);
         unit_write (XWDB, CQ(userdir));
      }
   }

   /*** If the requested userid is in the cache, return the directory
   /    number and set last-time-used. */
   for (i=0;   i<UCSIZE;   ++i)    if (streq (uid, uc[i].id))  {
      uc[i].lastuse = systime();
      RETURN (uc[i].dnum);
   }

   /*** Otherwise, find a spot in the cache for it by replacing the
   /    oldest entry.   (I0 ensures that the first userid, that of the
   /    person running Caucus, gets locked in and never replaced.) */
   o = i0;
   for (i=i0+1;   i<UCSIZE;   ++i)  
      if (uc[i].lastuse < uc[o].lastuse)  o = i;

   /*** Now find the USERnnn directory that contains this userid. */
   for (i=1;   (1);   ++i) {
      /*** Does USERnnn exist? */
      sprintf (userdir, "%s/USER%03d", confid_str, i);
      if (NOT syscheck (userdir))  break;

      /*** Does USERnnn/userid exist? */
      sprintf (userdir, "%s/USER%03d/%s", confid_str, i, uid);
      if (NOT syscheck (userdir))  continue;

      /*** Yes!  Mark it in the cache and return the directory number. */
      i0 = 1;
      strcpy (uc[o].id, uid);
      uc[o].lastuse = systime();
      uc[o].dnum    = i;
      RETURN (i);
   }

   /*** If we got here, the userid does not exist anywhere.  If we're
   /    not allowed to create it, complain. */
   --i;
   if (create == 0) {
      buglist (bugtell ("SYSUDNUM: not allowed to create userid", i, d, uid));
      RETURN (0);
   }
   if (create <  0)   RETURN (0);

   /*** Try to create userid directory. */
   sprintf  (userdir, "%s/USER%03d/%s", confid_str, i, uid);
   if (NOT sysmkdir (userdir, 0700, NULL)) {
   
      /*** Failed.  This probably means that the previous USERnnn was
      /    full.  Increment nnn, create the new USERnnn, and try again. */
      ++i;
      sprintf  (userdir, "%s/USER%03d", confid_str, i);
      sprintf  (textdir, "%s/TEXT%03d", confid_str, i);
      if (NOT sysmkdir (userdir, 0700, NULL)  ||  
          NOT sysmkdir (textdir, 0700, NULL)) {
         buglist (bugtell ("SYSUDNUM: Can't create USER/TEXTnnn!", i, d, uid));
         RETURN  (0);
      }
      sprintf  (userdir, "%s/USER%03d/%s", confid_str, i, uid);
      if (NOT sysmkdir (userdir, 0700, NULL)) {
         buglist (bugtell ("SYSUDNUM: confused! Can't create", i, d, uid));
         RETURN  (0);
      }
   }

   /*** Here if we succeeded in creating the userid directory.
   /    Mark it in the cache and return the directory number. */
   i0 = 1;
   strcpy (uc[o].id, uid);
   uc[o].lastuse = systime();
   uc[o].dnum    = i;
   RETURN (i);
#endif

}
