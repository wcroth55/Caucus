
/*** SYSMKDIR.  Make a directory DIR. 
/
/    ok = sysmkdir (name, perms, owner);
/
/    Parameters:
/       int    ok;      (success?)
/       char  *name;    (full path of directory to make)
/       int    perms;   (protection mode, unix std)
/       char  *owner;   (name of owning userid)
/
/    Purpose:
/
/    How it works:
/       Creates a new directory NAME, given the full path.  Sets the
/       permissions on the directory according to the usual Unix 9 bit mask:
/                      owner    group    world
/          read        0400     0040     0004
/          write       0200     0020     0002
/          traverse    0100     0010     0001
/
/       The Unix code ignores the OWNER argument.
/
/       The WNT40 code ignores the group bits, and gives the
/       same permissions to "administrator" as it does to owner.
/       Also, if OWNER is NULL, sysmkdir() uses the current userid as
/       the owner.  Otherwise, it uses the name in OWNER, regardless of
/       the actual userid.
/
/    Returns: 1 on success, 0 otherwise
/
/    Known bugs:      none
/
/    Home:  sys1/sysmkdir.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CR  6/19/97 21:15 Change meaning of 2nd arg. */
/*: CR  7/13/99 12:33 Add OWNER argument. */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "systype.h"

#include "caucus.h"

#if WNT40
#include <windows.h>
#include <iostream.h>
#endif

extern int  debug;

FUNCTION  sysmkdir (char *name, int perms, char *owner)
{
   char  *bugtell();

#if UNIX
   int    success, savemask, len, pos;
   char temp[2000];
   ENTRY  ("sysmkdir", "");

   len = strlen(name);
   if (perms != 0700)  savemask = umask ((mode_t) 0);

   for (pos=0;   pos<len;   ++pos) {
      if (pos > 0  &&  name[pos] == '/') {
         strcpy (temp, name);
         temp[pos] = '\0';
         mkdir (temp, (mode_t) perms);
      }
   }
   success = (mkdir (name, (mode_t) perms) == 0);

   if (perms != 0700)             umask ((mode_t) savemask);

   RETURN (success);
#endif

#if WNT40
   SECURITY_ATTRIBUTES sa;
   SECURITY_DESCRIPTOR sd;
   BYTE          aclbuf[1024];
   PACL          pacl = (PACL) &aclbuf;
   BYTE          sidbuf[100];
   DWORD         sidbufsize = 100;
   PSID          psid = (PSID) &sidbuf;
   char          username[100];
   DWORD         usernamesize = 100;
   DWORD         mask;
   DWORD         ace_index;
   ACE_HEADER   *ah;
   int           success;

   if (owner == NULL)  GetUserName (username, &usernamesize);
   else                strcpy      (username, owner);

   InitializeSecurityDescriptor (&sd, SECURITY_DESCRIPTOR_REVISION);
   InitializeAcl (pacl, 1024, ACL_REVISION);

   /*** Owner access. */
   if (strcmp (username, "administrator") != 0) {
      lookup (username, psid, sidbufsize);
      mask = 0;
      if  ( (perms & 0700) == 0700)  mask  = FILE_ALL_ACCESS;
      else {
         if (perms & 0400)           mask |= FILE_GENERIC_READ;
         if (perms & 0200)           mask |= FILE_GENERIC_WRITE;
         if (perms & 0100)           mask |= FILE_GENERIC_EXECUTE;
      }
      if (mask)  AddAccessAllowedAce (pacl, ACL_REVISION, mask, psid);
   }

   /*** Administrator access. */
   lookup ("administrator", psid, sidbufsize);
   mask = 0;
   if  ( (perms & 0700) == 0700)  mask  = FILE_ALL_ACCESS;
   else {
      if (perms & 0400)           mask |= FILE_GENERIC_READ;
      if (perms & 0200)           mask |= FILE_GENERIC_WRITE;
      if (perms & 0100)           mask |= FILE_GENERIC_EXECUTE;
   }
   if (mask)  AddAccessAllowedAce (pacl, ACL_REVISION, mask, psid);


   /*** World access. */
   if (perms & 0007) {
      lookup ("everyone", psid, sidbufsize);
      mask = 0;
      if  ( (perms & 0007) == 0007)  mask  = FILE_ALL_ACCESS;
      else {
         if (perms & 0004)           mask |= FILE_GENERIC_READ;
         if (perms & 0002)           mask |= FILE_GENERIC_WRITE;
         if (perms & 0001)           mask |= FILE_GENERIC_EXECUTE;
      }
      if (mask)  AddAccessAllowedAce (pacl, ACL_REVISION, mask, psid);
   }

   /*** Make all the ACEs have both container and object inherit. */
   for (ace_index=0;   GetAce (pacl, ace_index, &ah);   ++ace_index)
      ah->AceFlags |= CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE;

   SetSecurityDescriptorDacl (&sd, TRUE, pacl, FALSE);
   sa.nLength = sizeof(SECURITY_ATTRIBUTES);
   sa.bInheritHandle = FALSE;
   sa.lpSecurityDescriptor = &sd;

   success = CreateDirectory (name, &sa); 

   return (success ? 1 : 0);
#endif

}


#if WNT40
FUNCTION  lookup (char *username, PSID psid, int sbsize)
{
   char          dombuf[80];
   DWORD         dombufsize = 80;
   DWORD         sidbufsize;
   SID_NAME_USE  snu;
   int           rc1;

   sidbufsize = sbsize;
   rc1 = LookupAccountName (0, username, psid, &sidbufsize, dombuf, 
      &dombufsize, &snu);

}
#endif
