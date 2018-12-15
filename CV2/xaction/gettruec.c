/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** GET_TRUECONF.  Get the names of a conference & its original host's CHN 
/        based on the local confnum.
/
/   int n = get_trueconf (confnum, lname, tname, orig_host)
/
/   Parameters:
/      int   confnum;    (desired conference number)
/      Chix  lname;      (fill in local name of conf 'confnum')
/      Chix  tname;      (fill in true  name of conf 'confnum')
/      Chix  orig_host;  (fill in name of original host for conf 'confnum')
/
/   Purpose:
/      Caucus_x sometimes needs to figure out the name of a conf from its
/      confnum.  Further, for distributed both caucus_x and the XFP/U need
/      to get the "local" and "true" names of a conf from its local confnum.
/      They also need to know who the "original" host is.
/
/   How it works:
/      It looks in unit XCTC, whose only purpose is to store these names.
/      For stand-alone Caucus, there is only one line in the file.  This
/      line is the name of the conference.
/
/      For distributed, there are also second and third lines.  These are
/      the "true" conference name and the CHN of the "original" host, 
/      respectively.
/
/      Get_trueconf puts the first line from XCTC into the string LNAME.
/      If there are more lines and the caller has specified strings,
/      get_trueconf() puts the second line into the string pointed
/      to by TNAME and the third into the string pointed to by ORIG_HOST.
/
/      Call get_trueconf (-1, ...) to clear its cache.  This is important
/      in Caucus 4.0, where conference numbers can be re-used for different
/      confs, which messes up the assumption of the caching algorithm
/      in get_trueconf.
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
/   Home: xaction/gettruec.c
/ */

/*: JV  5/23/91 17:51 Create this. */
/*: JV  6/11/91 19:09 Expand to include ORIG_HOST. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/01/91 18:00 Clean. */
/*: JV  7/23/91 14:03 Fix unlocking. */
/*: JV  7/27/91 18:12 Add local confname arg. */
/*: CR  7/28/91 14:48 Include goodlint.h. */
/*: JV 11/06/91 10:29 Change common.h to null.h, remove error on no data. */
/*: WC 11/30/91  0:10 Cache results & use when possible. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: DE  6/04/92 13:21 Chixified */
/*: JX  9/28/92 15:26 Fixify. */
/*: CR 10/28/93 13:24 Rewrite caching logic to fix empty tname bug. */
/*: CK 12/01/93 14:54 Add 3rd arg to unit_read() calls. */
/*: CR  7/01/98 15:52 Add cache-clearing call. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "null.h"
#include "unitcode.h"
#include "goodlint.h"

FUNCTION   get_trueconf (confnum, lname, tname, orig_host)
   int  confnum;
   Chix lname, tname, orig_host;
{
   static Chix lastlname, lasttname, lasthost;
   static int  lastcnum = -1;
   Chix   line;
   ENTRY ("get_trueconf", "");

   /*** First time through, allocate the 'cache' to hold the info. */
   if (lastcnum == (-1)) {
      lastlname = chxalloc (L(100), THIN, "gettruec lastlname");
      lasttname = chxalloc (L(100), THIN, "gettruec lasttname");
      lasthost  = chxalloc (L(100), THIN, "gettruec lasthost");
   }

   if (confnum < 0)  { lastcnum = -1;  RETURN (1); }

   /*** If the answers are not in our cache, go get them. */
   if (confnum != lastcnum) {

      /*** Open the XCTC, checking for errors. */
      if (NOT unit_lock (XCTC, READ, L(confnum), L(0), L(0), L(0), nullchix)) {
         bug ("GET_TRUECONF: couldn't lock XCTC, conf#", confnum);
         RETURN (0); 
      }
      if (NOT unit_view (XCTC)) {
         bug ("GET_TRUECONF: couldn't open XCTC, conf#", confnum); 
         unit_unlk (XCTC);
         RETURN (0);
      }
   
      /*** Load the answers from the XCTC into the cache. */
      unit_read (XCTC, lastlname, L(0));
      if (NOT unit_read (XCTC, lasttname, L(0)))  chxclear (lasttname);
      if (NOT unit_read (XCTC, lasthost,  L(0)))  chxclear (lasthost);
      lastcnum = confnum;
   
      unit_close (XCTC);
      unit_unlk  (XCTC);
   }

   /*** Answers are guaranteed to be in the cache now, pull them out. */
   chxcpy (lname, lastlname);
   if (tname     != NULL)  chxcpy (tname,     lasttname);
   if (orig_host != NULL)  chxcpy (orig_host, lasthost);

   RETURN (1);
}   
