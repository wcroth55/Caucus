
/*** SYSSTATUS.   Display one line of conference status information on OUT.
/
/    Most systems just write out one line of info. 
/    DG systems put it at the top of the screen in reverse video.
/    (If output is not to screen, writes text normally.)
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/20/90 15:51 New function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

extern struct flag_template  flag;
extern Chix                  confname;

FUNCTION  sysstatus (out)
   int    out;
{
   Chix   newstr, format1, format2;
   static Chix *s_oneline1;
   static Chix *s_oneline2;
   int    newitems, totalresp, actitems, newmess;
   Chix   truncname, daytime, day, time;
   int4   systime();

   ENTRY ("sysstatus", "");

   newstr     = chxalloc (L(160), THIN, "sysstat newstr");
   format1    = chxalloc (L(160), THIN, "sysstat format1");
   format2    = chxalloc (L(160), THIN, "sysstat format2");
   truncname  = chxalloc (L(60),  THIN, "sysstat truncname");
   day        = chxalloc (L(30), THIN,  "sysstat day");
   time       = chxalloc (L(30), THIN,  "sysstat time");
   daytime    = chxalloc (L(60),  THIN, "sysstat daytime");

   calcstatus (&newitems, &totalresp, &actitems, &newmess);

#if UNIX | NUT40 | WNT40
   mdstr     (format1, "sys_Foneline1", s_oneline1);
   mdstr     (format2, "sys_Foneline2", s_oneline2);
   chxclear  (truncname);
   chxcatsub (truncname, confname, L(0), L(16));
   chxupper  (truncname);
   sysdaytime  (day, time, 0, systime());
   chxformat   (daytime, CQ("%s %s"), L(0), L(0), day, time);

/* Old: sys_Foneline: New in %-16s %3d Items %3d Resp (%3d items) %3d Msgs %s */
/* New: sys_Foneline1: New in %-16s %3d Items %3d Resp $
        sys_Foneline2: (%3d items) %3d Msgs %s */
   chxformat (newstr, format1, L(newitems), L(totalresp), truncname, nullchix);
   chxformat (newstr, format2, L(actitems), L(newmess), daytime, nullchix);

   unit_write (out, newstr);
#endif

   RETURN (1);
}
