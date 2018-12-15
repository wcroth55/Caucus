/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** Sendmgr.   Send a message to the CaucusLink Manager.
/
/   int n = sendmgr (text, title)
/
/   Parameters:
/      Chix text, title;    (Text and title of msg to send)
/
/   Purpose:
/     Sometimes CaucusLink needs to tell the CaucusMgr something.
/     Since the CaucusLink programs aren't interactive, we have tofind
/     some other way to do this.  SENDMGR is this "other way."
/     It sends a Caucus message to the Caucus Manager.
/
/   How it works:
/      First it finds out who the Caucus Manager is.  Then it sends the
/      the text and title specified.
/
/   Returns:
/      1 on success
/      0 on failure.  In this case it logs the error in the bugfile.
/
/   Error Conditions:
/
/   Related Functions:
/
/   Called by:
/
/   Home: xfu/sendmgr.c
/ */

/*: JV 12/31/91 16:00 Create this. */
/*: DE  6/05/92 12:03 Chixified */
/*: JX  7/24/92 15:40 Fixify, use make_hinfo(). */
/*: CR 10/09/92 17:31 Remove unused subjsize. */
/*: CR 10/11/92 22:42 Remove CQ from add_dmess() call. */
/*: CR  2/27/93 14:15 Replace add_dmess() with add_caumess(). */
/*: JV  4/05/93 18:23 Add attach arg to add_dmess(). */

#include <stdio.h>
#include "caucus.h"
#include "handicap.h"
#include "mess.h"
#include "chixuse.h"
#include "xfp.h"

FUNCTION sendmgr (text, title)
   Chix text, title;
{
   int      myerror, success;
   int4     systime();
   Chix     temp, date, time, nochix;
   HOSTPTR  host, make_host();
   Hinfoptr make_hinfo();

   ENTRY ("sendmgr", "");

   host = make_host();
   host->info = make_hinfo();

   temp    = chxalloc (L(60), THIN, "sendmgr temp");
   date    = chxalloc (L(40), THIN, "sendmgr date");
   time    = chxalloc (L(20), THIN, "sendmgr time");
   success = 0;

   /*** First find out who to send to. */
   unit_lock (XSNF, READ, L(0), L(0), L(0), L(0), nullchix);
   if (NOT get_neighbor (0, host, XSNF)) {
      bug ("SENDMGR: can't get my hostinfo.", 0);
      unit_unlk (XSNF); FAIL; }
   unit_unlk (XSNF);

   sysdaytime  (date, time, 0, systime());

   nochix = chxalloc (L(0), THIN, "sendmgr nochix");
   chxcpy (temp, CQ("CaucusLink"));
   add_caumess (temp, host->info->mgrid, host->chn,
                title, MSEND, date, time, nochix, 0, -1,
                host->info->mgrid, text, (Attachment) NULL, &myerror);
 
   chxfree (nochix);  
   SUCCEED;

 done:
   chxfree (temp);
   chxfree (date);
   chxfree (time);
   free_host (host);

   RETURN  (success);
}   
