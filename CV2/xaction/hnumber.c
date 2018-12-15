/*** HNUMBER.  Find the number of a given CHN.
/
/   int hnumber (chn);
/
/   Parameters:
/      Chix chn;        (CHN or initial substring)
/
/   Purpose:
/      This is just like cnumber(), which finds the conference number
/      of a particular conference name.  In this case the caller passes
/      in a *complete* CHN, and hnumber looks for the name in the XSHD/XSHN
/      host name tables.
/
/   How it works:
/       HNUMBER takes CHN, an initial substring of a Caucus Host Name,
/       and looks for a match in the system XSHD/XSHN host name tables.
/       If it finds a unique match, it copies the full name into CHN
/       and returns the Local Host Number.
/  
/       Format of files (XSHN) is:
/       !
/       !hosts #002
/       briton.aule-tek.com #002
/       khazad.aule-tek.com #001
/       ~~~~~~~~~~~~~~~~~~~~~~~~
/
/   Returns:
/      -1 if there are no matches.
/       0 if there are no exact matches.
/       LHN if there is a match.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  7/09/91 18:57 Create function. */
/*: CR  8/02/91 14:00 Add punctuation argument to matchnames() call. */
/*: JV 11/01/91 14:02 Change header. */
/*: CR 11/07/91 14:19 Add confnum arg to matchnames() call. */
/*: WC 11/29/91 23:33 Cache results for faster execution on next call. */
/*: JV 12/03/91 12:39 Allow initial substring matches. */
/*: JV 12/19/91 17:31 Don't cache "!hosts" as its value *changes*. */
/*: DE  6/04/92 15:02 Chixified */
/*: JX  7/31/92 13:53 Chixify arg. */
/*: JX  8/09/92 16:44 Fixify. */

#include <stdio.h>
#include "caucus.h"
#include "unitcode.h"

extern union null_t null;

FUNCTION  hnumber (chn)
   Chix   chn;
{
   static Chix lastchn;
   static int  lastnum = -1;
   struct namlist_t *list, *owners, *this, *maybe, *nlnode();
   Chix   fullname, code, open, number, hname;
   short  ok, size;
   int4   hnum=0;
   int    success = 0;

   ENTRY ("hnumber", "");

   if (lastchn == nullchix) lastchn = chxalloc (L(40), THIN, "hnumber lastchn");

   /*** Is CHN in our cache? */
   if (chxeq (chn, lastchn))   RETURN (lastnum);

   /*** Get namelist of hosts matching CHN. */
   list = nlnode (1);
   nladd (list, chn, 0);
   ok = matchnames (XSHD, XSHN, 0, null.chx, list, AND, &owners, PUNCT);
   nlfree (list);
   if (ok != 1)   RETURN (-1);

   /*** If no matches, return and complain. */
   if ( (size = nlsize (owners)) == 0) {
      nlfree (owners);
      RETURN (-1);
   }

   fullname  = chxalloc (L(80), THIN, "hnumber fullname");
   code      = chxalloc (L(20), THIN, "hnumber code");
   open      = chxalloc (L(20), THIN, "hnumber open");
   number    = chxalloc (L(20), THIN, "hnumber number");
   hname     = chxalloc (L(80), THIN, "hnumber hname");

   /*** Scan the list of matches and find the one exact match. */
   maybe = null.nl;
   for (this=owners->next;   this!=null.nl;   this = this->next) {
      chxtoken (fullname, null.chx, 1, this->str);
      if (NOT chxeq (chn, fullname)) { maybe = this; continue; }
      chxcpy   (hname, fullname);
      chxtoken (code, null.chx, 2, this->str);
      chxbreak (code, open, number, CQ("#")); 
      chxnum   (number, &hnum);
   }
   nlfree (owners);

   /*** Use initial substring match? */
   if (hnum == 0 && size == 1 && maybe != null.nl) {
      chxtoken (hname, null.chx, 1, maybe->str);
      chxtoken (code,  null.chx, 2, maybe->str);
      chxbreak (code, open, number, CQ("#"));
      chxnum   (number, &hnum);
   }
      
   if (hnum) chxcpy (chn, hname);
   
   /*** Cache the result. */
   if (NOT chxeq (chn, CQ("!hosts"))) {
      chxcpy (lastchn, chn);
      lastnum = hnum;
   }

   DONE (hnum);

 done:
   chxfree (fullname);
   chxfree (code);
   chxfree (open);
   chxfree (number);
   chxfree (hname);

   RETURN (success);
}
