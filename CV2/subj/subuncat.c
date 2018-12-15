
/*** SUBUNCAT.  Modify UNCATEGORIZED ITEMS subject, locally & remotely.
/
/    ok = subuncat (remove, range, other);
/
/    Parameters:
/       int    ok;          (success?)
/       int    remove;      (1=>remove RANGE from subject, 0=>add to subject)
/       Chix   range;       (item range, e.g. "1-5")
/       int    other;       (number of "other" subject)
/    
/    Purpose:
/       When a new item is created, it is "automatically" added to
/       the UNCATEGORIZED ITEMS subject.  Similarly, when an item
/       is added to a subject it is "automatically" removed from
/       UNCATEGORIZED ITEMS.
/
/       Subuncat() provides a convenient tool for the Caucus code that
/       has to implement said "automatic" changes.  Subuncat() makes
/       the requisite change to UNCATEGORIZED ITEMS, both locally
/       and across the network (by generating the proper transactions).
/
/    How it works:
/       RANGE is a character-string representation of an item range,
/       such as "1-5".  If REMOVE is true, the items represented by RANGE
/       are removed from UNCATEGORIZED ITEMS.  Otherwise, they are added
/       to that subject.
/
/       OTHER is a subject number.  If the subject number of UNCATEGORIZED
/       items is the same as OTHER, do nothing.  (This is provided in case
/       an organizer is specifically modifying the UNCATEGORIZED ITEMS
/       subject.)
/
/       Subuncat() creates the UNCATEGORIZED ITEMS subject if it does
/       not already exist.
/
/    Returns: 1 on success, 0 on Caucus database error.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions: subchange()
/
/    Called by: Caucus
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  subj/subuncat.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:56 Source code master starting point */
/*: CR  7/23/91 15:52 Replace subcreate() with chg_dsubject(). */
/*: CR  7/30/91 13:51 Replace license check with is_distributed(). */
/*: CR  7/30/91 22:17 Add header comments. */
/*: JV 10/31/91 15:31 Add dest_chn arg to addxosubject() call. */
/*: CR 11/07/91 13:53 Add confname to chg_dsubj(), subchange() calls. */
/*: DE  3/24/92 12:13 Chixified*/
/*: JX  5/19/92 15:39 Finish Chixifying. */
/*: JX  8/09/92 16:42 Fixify. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"

extern union  null_t    null;
extern short            confnum;
extern Chix             confname;

FUNCTION  subuncat (remove, range, other)
   int    remove, other;
   Chix   range;
{
   Chix   uncat;
   int    sub, error1, error2, success;

   ENTRY ("subuncat", "");

   uncat     = chxalloc (L(100), THIN, "subuncat uncat");
   success   = 0; 

   /*** Load name of uncategorized items subject from dictionary. */
   if (NOT mdstr (uncat, "sub_Tuncat", null.md))  FAIL;

   /*** Find number of this subject.  If it doesn't exist, create it! */
   if ( (sub = submatch (XCND,XCNN, XCSD,XCSF, nullchix, 
                         confnum, confname, uncat)) <= 0) {
      chg_dsubject (XT_NEW, confnum, confname, nullchix, uncat, nullchix,
                    &error1);
      if (is_distributed (confnum)) {
         addxosubject (XT_NEW, confnum, uncat, nullchix, nullchix, &error2);
      }
      if (remove)  SUCCEED;
      sub = submatch (XCND,XCNN, XCSD,XCSF, nullchix, confnum, confname, uncat);
   }

   /*** Modify the subject. */
   if (other == sub)  SUCCEED;
   success = subchange (uncat, confnum, confname, nullchix, remove, range);

done:

   chxfree ( uncat );

   RETURN ( success );   
}
