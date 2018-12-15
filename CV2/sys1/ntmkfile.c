
/*** NT_MKFILE.   Create a Windows/NT file with certain permissions.
/
/    nt_mkfile (name, perms);
/   
/    Parameters:
/       char  *name;     (name of file to be created)
/       int    perms;    (permissions in Unix format)
/
/    Purpose:
/       Create an (empty) file, using the traditional Unix file
/       permission mask.
/
/    How it works:
/       The argument PERMS contains the usual Unix 9-bit mask.
/       Nt_mkfile() interprets only the owner bits and the
/       world bits; group bits are ignored.
/
/       If the user is not "administrator", the owner bits
/       are also applied to the group "administrators".
/
/    Returns:
/
/    Known bugs:
/
/    Home:  nkmkfile.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/27/97 15:31 New function. */

#include "systype.h"

#if WNT40
#include <windows.h>
#include <iostream.h>
#endif

#define   FUNCTION

FUNCTION nt_mkfile (char *name, int perms)
{
#if WNT40
   SECURITY_ATTRIBUTES sa;
   SECURITY_DESCRIPTOR sd;
   BYTE          aclbuf[1024];
   PACL          pacl = (PACL) &aclbuf;
   BYTE          sidbuf[100];
   DWORD         sidbufsize = 100;
   PSID          psid = (PSID) &sidbuf;
   char          dombuf[80];
   DWORD         dombufsize = 80;
   SID_NAME_USE  snu;
   char          username[100];
   DWORD         usernamesize = 100;
   HANDLE        file;
   DWORD         mask;

   GetUserName (username, &usernamesize);

   InitializeSecurityDescriptor (&sd, SECURITY_DESCRIPTOR_REVISION);
   InitializeAcl (pacl, 1024, ACL_REVISION);

   LookupAccountName (0, username, psid, &sidbufsize, dombuf, 
      &dombufsize, &snu);
   mask = 0;
   if  ( (perms & 0700) == 0700)  mask  = FILE_ALL_ACCESS;
   else {
      if (perms & 0400)           mask |= FILE_GENERIC_READ;
      if (perms & 0200)           mask |= FILE_GENERIC_WRITE | DELETE;
      if (perms & 0100)           mask |= FILE_GENERIC_EXECUTE;
   }
   if (mask != 0)  AddAccessAllowedAce (pacl, ACL_REVISION, mask, psid);

   if (strcmp (username, "administrator") != 0) {
      LookupAccountName (0, "administrators", psid, &sidbufsize, dombuf, 
         &dombufsize, &snu);
      if (mask != 0)  AddAccessAllowedAce (pacl, ACL_REVISION, mask, psid);
   }

   LookupAccountName (0, "everyone", psid, &sidbufsize, dombuf, 
      &dombufsize, &snu);
   mask = 0;
   if  ( (perms & 0007) == 0007)  mask  = FILE_ALL_ACCESS;
   else {
      if (perms & 0004)           mask |= FILE_GENERIC_READ;
      if (perms & 0002)           mask |= FILE_GENERIC_WRITE | DELETE;
      if (perms & 0001)           mask |= FILE_GENERIC_EXECUTE;
   }
   if (mask != 0)  AddAccessAllowedAce (pacl, ACL_REVISION, mask, psid);

   SetSecurityDescriptorDacl (&sd, TRUE, pacl, FALSE);

   sa.nLength = sizeof(SECURITY_ATTRIBUTES);
   sa.bInheritHandle = FALSE;
   sa.lpSecurityDescriptor = &sd;

   file = CreateFile (name, GENERIC_READ | GENERIC_WRITE,
          0, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
   CloseHandle (file);

#endif
   return(1);
}
