
/*** SYSUSERID.  Copies the account name of the current user
/                over the string supplied. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:46 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CR  5/02/03 Complete rewrite for Caucus 4.47 */
/*: CR 11/12/03 Move BSD and FBS to new 2003 code section. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "sysexit.h"

#if WNT40
#include <windows.h>
#include <iostream.h>
#endif

#if UNIX
/* (old) #include <unistd.h> */
#include <pwd.h>
#include <sys/types.h>
#endif

#define SAVE_USERID_MAX 100

extern struct flag_template flag;
static char   save_userid[SAVE_USERID_MAX];

FUNCTION  sysuserid (str_chx)
   Chix     str_chx;
{
   char   *s;
   char    str[160];

/*---New (2003) code should use getpwuid() whereever possible.  All
/    old code has been marked (with "5 = ;") to make it uncompilable,
/    forcing us to adapt all flavors of unix to the new code form. */

#if LNX12 | DEC32 | SOL25 | BSD21 | FBS22
   struct passwd *p;
   ENTRY ("sysuserid", "");

   if (flag.login)  strcpy (str, flag.userid);
   else {
      if (NULLSTR (save_userid)) {
         sysprv (0);
         p = getpwuid (getuid());
         sysprv (1);
         if (p == NULL  ||  NULLSTR(p->pw_name))  sysexit (EXIT_NOUID);

         strcpy (save_userid, p->pw_name);
      }
      strcpy (str, save_userid);
   }
#endif

/*-------------------------------All Unix besides SV---------------------*/
#if AIX41 | IRX53 | SOL24 | HPXA9 | SUN41
   5 = ;
   char  *p;
   char  *cuserid();

   ENTRY ("sysuserid", "");
   if (flag.login)  strcpy (str, flag.userid);
   else {
      if (NULLSTR (save_userid)) {
         sysprv (0);
         p = cuserid( (char *) NULL);
         sysprv (1);
         if (p == NULL  ||  NULLSTR(p))  sysexit (EXIT_NOUID);

         strcpy (save_userid, p);
      }
      strcpy (str, save_userid);
   }
#endif

/*-------------------------------Unisys SV Unix--------------------------*/
#if 0
   5 = ;
   static char save_userid[SAVE_USERID_MAX];
   char  *ptr, *getlogin();

   ENTRY ("sysuserid", "");

   if (flag.login)  strcpy (str, flag.userid);
   else {
      if (NULLSTR (save_userid)) {
         sysprv (0);
         if ( (ptr = getlogin()) == NULL  ||  NULLSTR(ptr)) {
            printf ("Bad return from getlogin()!\n");
            exit   (9);
         }
         strcpy   (save_userid, ptr);
         sysprv (1);
      }
      strcpy (str, save_userid);
   }
#endif

/*-------------------------------WNT40 NT--------------------------*/
#if WNT40
   static char save_userid[SAVE_USERID_MAX];
   int         sulen = SAVE_USERID_MAX;
   char  *ptr;

   ENTRY ("sysuserid", "");

   if (flag.login)  strcpy (str, flag.userid);
   else {
      if (NULLSTR (save_userid))   GetUserName (save_userid, &sulen);
      strcpy (str, save_userid);
   }
#endif

#if UNIX | NUT40 | WNT40
   for (s=str;   *s;   ++s)    if (cf_upper(*s))    *s += ('a' - 'A');
#endif

   if (flag.underscore  &&  NOT flag.login)  strcat (str, "_");
   chxofascii (str_chx, str);

   RETURN (1);
}
