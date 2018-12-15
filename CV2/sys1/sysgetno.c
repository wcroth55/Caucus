
/*** SYSGETNODE.  Get node name. 
/
/    Gets node name into NODENAME.  This is the name
/    **as known to other computers**.
/    
/    Nodenames are limited to 20 characters.
/    Returns 1 on success, 0 on failure.
/    */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  1/03/90 11:57 Function created. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#if SYSV | HPXA9 | BSD21 | LNX12 | NUT40 | FBS22
#include <sys/utsname.h>
#endif

#if WNT40
#include <windows.h>
#include <iostream.h>
#include <iomanip.h>
#endif

extern int debug;

FUNCTION  sysgetnode (nodename)
   Chix   nodename;
{
   int    success = 0;

/*------------------------------------------------------------------*/
#if WNT40
   char   name[512];
   DWORD  namelen = 512;
   ENTRY   ("sysgetnode", "");

   GetComputerName (name, &namelen);
   chxcpy  (nodename, CQ(name));
   success = 1;
#endif

/*------------------------SYSV Unix---------------------------------*/
#if SYSV | HPXA9 | BSD21 | LNX12 | NUT40 | FBS22
   struct  utsname u;

   ENTRY   ("sysgetnode", "");
   uname   (&u);
   chxcpy  (nodename, CQ(u.nodename));
   if (debug) printf ("Node: %s\n", nodename->p);
   success = 1;
#endif

/*-----------------------BERKELEY Unix------------------------------*/
#if SUN41
   char hostname[120];

   ENTRY ("sysgetnode", "");

   gethostname (hostname, 100);
   chxcatsub   (nodename, CQ(hostname), L(0), L(20));
   jixreduce   (nodename);
   success = 1;
#endif

   RETURN (success);
}
