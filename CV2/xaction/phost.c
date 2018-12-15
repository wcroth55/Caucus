/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** PHOST: Print out host information.
/
/    Purpose: CaucusLink trouble-shooting frequently requires knowing
/       sequence #'s
/
/    Invocation:
/       phost confid hostnum
/
/    Arguments:
/       confid       The usual: home of the Caucus account
/       hostnum      CH#  (get this from chn_x)
/
/    Options:
/ */

/*: JV  6/16/93 17:37 Create this. */
/*: CP  7/26/93 12:50 Declare strito36(). */
/*: JZ 10/18/93 10:33 Remove dust bunnies. */

#include <stdio.h>
#include <chixuse.h>
#include "handicap.h"
#include "systype.h"
#include "xaction.h"
/*#include "sysinit.h"*/
#include "unitcode.h"
#include "xfp.h"

extern Chix                    confid;

FUNCTION  main (argc, argv)
   int    argc;
   char  *argv[];
{
   char  temp[8];
   int   lhnum;
   int4  chnum;
   Chix  chn, userid;
   Hinfoptr make_hinfo();
   HOSTPTR  host, make_host();
   char    *strito36();

   chn      = chxalloc (L(4),  THIN, "phost choice");
   confid   = chxalloc (L(40), THIN, "phost confid");
   userid   = chxalloc (L(40), THIN, "phost userid");

/* SYSINIT; */

   /*** Get the confid. */
   if (argc <  3) {
      printf ("Usage: %s confid hostnum.\n", argv[0]);
      exit (1);
   }
   chxcpy (confid, CQ(argv[1]));

   /*** Get CH#. */
   chxcpy (chn, CQ(argv[2]));
   if (NOT chxnum (chn, &chnum)) {
      printf ("Usage: %s confid hostnum.\n", argv[0]);
      exit (1);
   }
   lhnum = chnum;

   utilglobals();
   optchk ();

   /*** Miscellaneous initialization. */
   unitglobals();
   sysbrkinit();
   sysglobals();
   sysuserid (userid);
   sysdirchk (userid);

   host =       make_host();
   host->info = make_hinfo();

   if (NOT unit_lock (XHI1, WRITE, L(lhnum), L(0), L(0), L(0), nullchix)) {
      printf ("Host number %d does not exist.\n", chnum);
      exit (1);
   }

   if (NOT get_neighbor (lhnum, host, XHI1)) {
      printf ("Host number %d does not exist.\n", chnum);
      exit (1);
   }

   printf ("\n");
   printf ("Caucus Host Name:  %s\n", ascquick (host->chn));
   printf ("Local Host Number: %d\n", host->lhnumber);
   printf ("Remote Caucus Mgr: %s\n", ascquick (host->info->mgrid));
   printf ("Shipping Method:   %d\n", host->info->shipmeth);
   printf ("\n");
   printf ("ReShip Time: %d\n", host->info->reshiptime);
   printf ("Toss Time:   %d\n", host->info->tosstime);
   printf ("Chunk Keep Time: %d\n", host->info->keep_chunks);
   printf ("\n");
   printf ("Low  outbound file: %d (%s)\n", host->info->seqnum_to_low,
           strito36 (temp, host->info->seqnum_to_low, 4));
   printf ("High outbound file: %d (%s)\n", host->info->seqnum_to_shipd, 
           strito36 (temp, host->info->seqnum_to_shipd, 4));
   printf ("Next outbound file: %d (%s)\n", host->info->seqnum_to_hi, 
           strito36 (temp, host->info->seqnum_to_hi, 4));
   printf ("Next inbound file:  %d (%s)\n", host->info->seqnum_from, 
           strito36 (temp, host->info->seqnum_from, 4));
   printf ("Next ACK: %d (%s)\n", host->info->ackseq,
           strito36 (temp, host->info->ackseq, 4));
   printf ("\n");
   printf ("Low thru file:  %d (%s)\n", host->info->seqnum_thru_low,
           strito36 (temp, host->info->seqnum_thru_low, 4));
   printf ("Next thru file: %d (%s)\n", host->info->seqnum_thru_hi,
           strito36 (temp, host->info->seqnum_thru_hi, 4));

   unit_unlk (XHI1);
   return  (0); /* Make Lint happy. */
}
