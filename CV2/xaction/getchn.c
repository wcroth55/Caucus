/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** GETCHN.   Get the CHN (unique ID) for this machine or another one.
/
/   int n = get_chn (chnstr, unit, lhnum)
/
/   Parameters:
/      Chix  chnstr;    (returned CHN for selected machine)
/      int   unit;      (unit number of info file for selected machine)
/      int   lhnum;     (local host number if unit==XHI1/XHI2)
/
/   Purpose:
/      This figures out the unique CHN for either this host or another
/      CaucusNet host on this net.
/
/   How it works:
/      The caller can ask for either:
/      1) The CHN of this host, by specifying a UNIT of XSNF.
/         In this case get_chn() gets the CHN from unit XSNF.
/         The CHN is the first word of the first line of the file.
/         In this case get_chn only goes to disk the first time it's called.
/      2) The CHN of another host, by specifying a UNIT of XHI1 or XHI2
/         In this case get_chn() gets the CHN from unit UNIT, using LHNUM.
/         The CHN is the first word of the first line of the file.
/
/   Returns:
/      1 on success
/      0 on failure.  In this case it logs the error in the bugfile.
/        Clears CHNSTR in that case.
/
/   Error Conditions:
/
/   Related Functions:
/
/   Called by: crexport()
/
/   Home: xaction/getchn.c
/ */

/*: JV  4/02/91 11:51 Create this. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/01/91 15:24 Add LHNUM. */
/*: JV  7/04/91 17:55 Remove lint. */
/*: JV  8/04/91 08:07 Check UNIT before opening. */
/*: JV 11/06/91 10:29 Change common.h to null.h. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: DE  6/03/92 13:42 Chixified */
/*: CR 12/16/92 14:04 Comment out %%^$! view failure msg; clean up. */

#include <stdio.h>
#include "chixuse.h"
#include "handicap.h"
#include "goodlint.h"
#include "null.h"
#include "unitcode.h"

extern union null_t null;

FUNCTION get_chn (chnstr, unit, lhnum)
   int   unit, lhnum;
   Chix  chnstr;
{
   Chix   str;
   static Chix mychn = nullchix;
   static int  first=1;

   ENTRY ("get_chn", "");

   if (mychn == nullchix)  mychn = chxalloc (L(80), THIN, "getchn mychn");

   if (unit==XSNF  &&  NOT first) {
      chxcpy (chnstr, mychn);
      RETURN (1);
   }
   chxclear (chnstr);

   switch (unit) {
      case XSNF:
         if (NOT unit_lock (XSNF, READ, L(0), L(0), L(0), L(0), null.chx)) {
            bug ("GET_CHN: couldn't lock XSNF.", 0); RETURN (0); 
         }
         first=0; break;
      case XHI1:
      case XHI2:
         if (NOT unit_lock (unit, READ, L(lhnum), L(0), L(0), L(0), null.chx)) {
            bug ("GET_CHN: couldn't lock XHIx: ", lhnum); RETURN (0); 
         }
         break;
      default:
         bug ("GET_CHN: bad argument: unit: ", unit); RETURN (0); 
   }

   if (NOT unit_view (unit)) {
/*    bug ("GET_CHN: can't view unit: ", unit); */
      unit_unlk (unit); RETURN (0);
   }

   str = chxalloc (L(200), THIN, "getchn str");

   unit_read (unit, str, L(0));
   chxtoken (chnstr, null.chx, 1, str);
   unit_close (unit);
   unit_unlk  (unit);

   chxfree ( str );

   if (unit==XSNF) chxcpy (mychn, chnstr);
   RETURN (1);
}

