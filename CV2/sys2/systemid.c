
/*** SYSTEMID.  Return system id in STR.  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:45 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#if SYSV | HPXA9 | BSD21 | LNX12 | NUT40 | FBS22
#include <sys/utsname.h>
#endif

FUNCTION  systemid (str)
   Chix   str;
{

#if WNT40
   ENTRY ("systemid", "");
   sysgetnode (str);
#endif

/*-----------------------BERKELEY Unix------------------------------*/
#if SUN41
   char   hostname[120];

   ENTRY ("systemid", "");

   gethostname (hostname, 100);
   chxclear  (str);
   chxcatsub (str, CQ(hostname), L(0), L(MAXSYSID));
   jixreduce (str);
#endif


/*---------------------------------------AT&T 3B------------------------*/
#if SYSV | BSD21 | LNX12 | NUT40 | FBS22
   char   hostname[120];
   struct utsname name;

   ENTRY ("systemid", "");

   uname    (&name);
   strcpy   (hostname, name.nodename);

   chxclear  (str);
   chxcatsub (str, CQ(hostname), L(0), L(MAXSYSID));
   jixreduce (str);
#endif


/*---------------------------------------HP/UX--------------------------*/
#if HPXA9
   char   hostname[120];
   struct utsname name;

   ENTRY ("systemid", "");

   uname    (&name);

   strcpy (hostname, name.idnumber);
   if (NULLSTR(hostname))  strcpy (hostname, name.nodename);
   chxclear  (str);
   chxcatsub (str, CQ(hostname), L(0), L(MAXSYSID));
   jixreduce (str);
#endif

   RETURN (1);
}
