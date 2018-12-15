/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

#include <stdio.h>
#include <windows.h>
#include <iostream.h>

main (int argc, char *argv[])
{
   BYTE          sidbuf[100];
   DWORD         sidbufsize = 100;
   PSID          psid = (PSID) &sidbuf;
   char          dombuf[80];
   DWORD         dombufsize = 80;
   SID_NAME_USE  snu;
   char          username[100];
   DWORD         usernamesize = 100;
   int           n;


   printf ("SidTypeUser           = %d\n", SidTypeUser);
   printf ("SidTypeGroup          = %d\n", SidTypeGroup);
   printf ("SidTypeDomain         = %d\n", SidTypeDomain);
   printf ("SidTypeAlias          = %d\n", SidTypeAlias);
   printf ("SidTypeWellKnownGroup = %d\n", SidTypeWellKnownGroup);
   printf ("SidTypeUnknown        = %d\n", SidTypeUnknown);
   printf ("SidTypeInvalid        = %d\n", SidTypeInvalid);
   printf ("\n");

   GetUserName (username, &usernamesize);
   LookupAccountName (0, username, psid, &sidbufsize, dombuf, 
      &dombufsize, &snu);
   printf ("%-21s = %d\n", username, snu);

   for (n=1;   n<argc;   ++n) {
      LookupAccountName (0, argv[n], psid, &sidbufsize, dombuf, 
          &dombufsize, &snu);
      printf ("%-21s = %d\n", argv[1], snu);
   }
   exit   (0);
}
