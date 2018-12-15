/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SYSVERSION.  Print banner with current version number. */

/*: AA  7/29/88 17:46 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#if BSD21
#define  VERSION    "sys_Tv_bi"
#endif

#if FBS22
#define  VERSION    "sys_Tv_fb"
#endif

#if HPXA9
#define  VERSION    "sys_Tv_hu"
#endif

#if SUN41
#define  VERSION    "sys_Tv_sn"
#endif

#if SYSV
#define  VERSION    "sys_Tv_u5"
#endif

#if LNX12
#define  VERSION    "sys_Tv_lx"
#endif

#if NUT40 | WNT40
#define  VERSION    "sys_Tv_nt"
#endif

extern union null_t null;

FUNCTION  sysversion (out, usedic)
   int    out, usedic;
{
   Chix   newstr, tempchx;

   ENTRY   ("sysversion", "");

   newstr  = chxalloc (L(160), THIN, "sysversi newstr");

   unit_write (out, CQ("\n"));

/*
   if (usedic  &&  mdstr (newstr, VERSION, null.md))  unit_write (out, newstr);
   else {
*/

      unit_write (out, CQ("Caucus (TM) version 2.7/"));
      chxcpy     (newstr, CQ(VERSION));
      chxupper   (newstr);
      unit_write (out, tempchx = CHXSUB(newstr,7L));
      chxfree    (tempchx);
      unit_write (out, CQ(".  Copyright (C) 1996 Caucus Systems Inc.\n"));
/* } */

   chxfree (newstr);

   RETURN  (1);
}
