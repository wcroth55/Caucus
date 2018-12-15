/*** FRZ_DITEM.  Freeze or Thaw an item.
/
/    ok = frz_ditem (cnum, item, freeze)
/
/    Parameters:
/       int cnum,     (Local Conference Number)
/           item,     (Item number)
/           freeze;   (1 for freeze, 0 for thaw)
/
/    Purpose:
/       This is the function that actually changes the database to show
/       that the item is frozen.
/
/    How it works:
/       Loads the response partfile into the text buffer, changes the
/       relevant data, and stores the text buffer back into the partfile.
/
/    Returns: 1 (always)
/
/    Error Conditions: sets value of ERROR accordingly:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  freezeitem(), item_parse()
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/frzditem.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV 12/05/91 11:00 New function. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: JV 12/16/91 18:48 Change 4th arg to loadtext to int4. */
/*: CR 12/20/91 15:05 Correct header (does NOT call chgxitem!)  */
/*: CR 12/26/91 11:48 Remove unused variables. */
/*: JX  5/15/91 10:11 Xlate block_template -> Textbuf. */
/*: CR 10/09/92 17:01 Remove unused 's'. */
/*: CR 12/07/92 13:37 Add CNUM arg to loadtext(). */
/*: CR 12/10/92 15:05 chxalter arg int4. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */

#include <stdio.h>
#include "caucus.h"

extern struct this_template   thisitem;
extern union  null_t          null;
extern Textbuf                tbuf;

FUNCTION  frz_ditem (cnum, item, freeze)
   int   cnum, item, freeze;
{
   Chix   status, hdr, getline2();
   int    success;
   int4   pos;

   ENTRY ("frz_ditem", "");

   status    = chxalloc (L(40), THIN, "frzditem status");
   success   = 0;

   unit_lock (XCRF, WRITE, L(cnum), L(item), L(0), L(0), null.chx);
   loadtext  (XCRF, cnum, item, 0, L(0), tbuf);

   hdr = getline2 (XCRD, XCRF, cnum, null.chx, item, 0, 0, (Textbuf) NULL);
   pos = chxtoken (status, nullchix, 3, hdr);

   if (freeze  &&  chxvalue(status,0L)=='0') {
      chxalter (hdr, pos, chxquick ("0",0), chxquick ("1",1));
      thisitem.frozen = 1;
      storetext (XCRF, 0, tbuf->r_used-1, tbuf);
   }
   if (NOT freeze  &&  chxvalue(status,0L)=='1') {
      chxalter (hdr, pos, chxquick ("1",0), chxquick("0",1));
      thisitem.frozen = 0;
      storetext (XCRF, 0, tbuf->r_used-1, tbuf);
   }
 
   unit_unlk (XCRF);

   SUCCEED;

 done:
   chxfree (status);

   RETURN (success);
}
