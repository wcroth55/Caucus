/*** SRVLDMAS.   Server Load Master file.
/
/    Load master file for this conference into the MASTER structure.
/    If LOCK is 1, lock the file first, and unlock it when done.  Otherwise,
/    the locking has been (and unlocking will be) done by the caller.
/
/    If this is a server-based machine, ask the server to actually do
/    the work.
/
/    As of 11/17/92, srvldmas() loads original host names into the global
/    array "hostnames".  Only one instance of each host name on this
/    CaucusLink network can occur in this array.  The short
/    master->orighost[i] refers to an element in the array.
/    This can save up to ~.5MB on some systems.
/
/    Returns 1 normally, 0 if it failed. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: JV  8/24/88 10:39 Added AOS code.*/
/*: CR  9/21/88 15:23 Use #if UNIX instead of #if A3 | LA | SX ... */
/*: CR 11/02/88 16:03 Add SRV code. */
/*: CR 11/09/88 15:30 Fix SRV bugs. */
/*: JV  2/11/89 17:25 Fixed vcsend. */
/*: JV  2/11/89 19:23 Fixed the rest of vcsend! */
/*: CR  2/16/89 10:44 Add checking for 3B2 bug. */
/*: CR  3/16/89 15:45 (int) sizeof(), to make Lint happy. */
/*: CR  3/21/89 15:15 Add use of servername to #if SRV code. */
/*: CR  3/27/89 13:57 Corrupted master.items also reported by buglist(). */
/*: CW  5/08/89 15:17 Add PX system code. */
/*: JV 10/04/89 15:34 Stripped out AOS non-server stuff. */
/*: CR  9/20/90 17:03 Add PS system type. */
/*: CJ 11/26/90 21:50 Add LA system type code. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  6/17/91 17:58 Use new master_template with origitem[] & orighost[].*/
/*: CR  6/25/91 15:06 Add 'confnum' argument. */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: CR  1/02/92 14:24 Add master.chgsubj. */
/*: CR  2/24/92 15:18 Clear orighost for each item not having an orig host. */
/*: DE  5/27/92 14:43 Chixified */
/*: JX  6/04/92 13:35 ascofchx(buf)->ascquick(buf). */
/*: CI 10/07/92 13:39 Chix Integration. */
/*: JV 10/14/92 10:06 Change sanity limit on master.items to MAXITEMS. */
/*: JV 11/17/92 16:49 Change orighost to array of shorts; add HOSTNAMES. */
/*: CL 12/10/92 17:06 Long ascofchx args. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

extern Chix *hostnames;
extern int   numhosts;

FUNCTION  srvldmas (lock, master, confnum)
   int    lock, confnum;
   struct master_template *master;
{

/*---------------------------"Normal" systems------------------------*/
#if UNIX | NUT40 | WNT40
   short  parms[4], i, j;
   short  open, found;
   Chix   buf, hostname;

   ENTRY ("srvldmas", "");

   if (lock  &&  NOT unit_lock (XCMA, READ, L(confnum), L(0), L(0), L(0), CQ("")))
                            RETURN (0);
   open = unit_view  (XCMA);
   if (NOT open  &&  lock)  unit_unlk (XCMA);
   if (NOT open)            RETURN (0);

   unit_read  (XCMA, master->organizerid, L(0));
   unit_short (XCMA, parms, 4);
   master->items    = parms[0];
   master->nochange = parms[1];
   master->noenter  = parms[2];
   master->chgsubj  = parms[3];
   master->cnum     = confnum;

   /*** Check for 3B2 bug here. */
   if (master->items < 0  ||  master->items > MAXITEMS) {
      buglist ("SRVLDMAS: corrupted master.items: fatal error.");
      printf  ("SRVLDMAS: corrupted master.items: fatal error.\n");
      exit   (1);
   }

   buf      = chxalloc (L(160), THIN, "srvldmas buf");
   hostname = chxalloc (L(160), THIN, "srvldmas hostname");

   for (i=1;  i<=master->items;  ++i)  {
      unit_read (XCMA,  buf, L(0));
      strshort  (parms, ascquick(buf), 2);
      master->responses[i] = parms[0];
      master->origitem [i] = parms[1];

      /*** Was item added on another host? */
      if (chxtoken (hostname, nullchix, 3, buf) != -1) {
         found = 0;
         /*** Is original hostname already in table? */
         for (j = 1; j <= numhosts; j++)
            if (chxeq (hostnames[j], hostname)) { found = 1; break; }
         /*** No, add it to the end. */
         if (NOT found) {
            if (hostnames[++numhosts] == nullchix)
               hostnames[numhosts] = chxalloc (L(32), THIN, "hostnam [i]");
            chxcpy (hostnames[numhosts], hostname);
         }
               
         /*** Link orighost[i] to hostnames table. */
         if (found) master->orighost[i] = j;
         else       master->orighost[i] = numhosts;
      }

/* Old code:
      if (chxtoken (hostname, nullchix, 3, buf) != -1) {
         if (master->orighost[i] == nullchix)
             master->orighost[i] = chxalloc (L(20), THIN, "orighost[i]");
         chxcpy (master->orighost[i], hostname);
      }
      else if (master->orighost[i] != nullchix) {
         chxfree (master->orighost[i]);
         master->orighost[i] = nullchix;
      }
*/
   }
   unit_close (XCMA);
   if (lock) unit_unlk (XCMA);

   chxfree (buf);
   chxfree (hostname);

   RETURN (1);

#endif


}
