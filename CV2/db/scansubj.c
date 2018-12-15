
/*** SCAN_SUBJECT.   Convert transaction-style subject list to an item range.
/
/    scan_subject (csubj, sign, range, cnum);
/
/    Parameters:
/       Chix   csubj;       (supplied subject list)
/       char   sign;        ('+' or '-')
/       short *range;       (resulting item range array)
/       int    cnum;        (conference number)
/
/    Purpose:
/       Scan_subject() is given a chix containing a transaction-style
/       subject list (e.g., "+5@thor -12@odin").  It converts this list
/       to an item range of local item numbers.
/
/    How it works:
/       Scan_subject() scans each word in CSUBJ, and parses it into
/       a remote item number and a host name.  If the first character of
/       the word begins with SIGN, scan_subject() converts these to a local
/       item number I.  RANGE[I] is then set to 1.
/
/       The rest of RANGE is set to 0.
/
/    Returns: 1 if at least one item number was found & converted.
/             0 otherwise.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  chg_dsubject() only.
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/scansubj.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/17/91 15:06 New function. */
/*: CR  7/23/91 23:48 Correct 3rd arg in chxalloc() calls. */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: CR  7/29/91 23:57 Add missing ENTRY! */
/*: CR  1/14/92 12:47 Check local back from iorig_to_local(). */
/*: JX  5/13/92 16:45 Fix Flag use. */
/*: JX  6/25/92 10:48 Fix chxtoken return. */
/*: CR 12/11/92 13:12 Chxnextl last arg is int4 *. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CN  5/15/93 22:02 Make 'range' short. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  scan_subject (csubj, sign, range, cnum)
   Chix   csubj;
   char   sign;
   short  range[];
   int    cnum;
{
   int    i, local, any;
   int4   oitem, pos;
   Chix   line, word, atsign, citem, chost;

   ENTRY ("scan_subject", "");

   for (i=1;   i<MAXITEMS;   ++i)  range[i] = -1;

   line   = chxalloc (L(100), THIN, "line   scan_subj");
   word   = chxalloc (L(100), THIN, "word   scan_subj");
   citem  = chxalloc ( L(40), THIN, "citem  scan_subj");
   chost  = chxalloc ( L(40), THIN, "chost  scan_subj");
   atsign = chxalloc ( L(10), THIN, "atsign scan_subj");
   chxcpy (atsign, chxquick ("@", 0));

   /*** Scan each WORD in each LINE. */
   any = 0;
   for (pos=0;   chxnextline (line, csubj, &pos);   ) {
      for (i=1;   chxtoken (word, nullchix, i, line) != -1;   ++i) {

         /*** If WORD begins with SIGN, parse it, convert to a LOCAL
         /    item number, and mark that element of RANGE. */
         if (chxvalue (word, L(0)) != sign)  continue;
         chxbreak (word, citem, chost, atsign);
         chxnum   (citem, &oitem);
         if (oitem < 0)  oitem = -oitem;
         iorig_to_local (cnum, (int) oitem, chost, &local);
         if (local > 0) range[local] = any = 1;
      }
   }
   chxfree (word);   chxfree (line);   chxfree (citem);
   chxfree (chost);  chxfree (atsign);

   RETURN (any);
}


