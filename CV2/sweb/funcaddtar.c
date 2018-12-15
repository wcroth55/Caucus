
/*** FUNC_ADDTARGET.  Add target to <A HREF> links if necessary.
/
/    func_addtarget (arg, conf);
/   
/    Parameters:
/       Chix     arg;     (function argument)
/       SWconf   conf;    (server configuration info)
/
/    Purpose:
/       Detect <A HREF...> links, and add a target window to them if there
/       isn't one and file is something viewable that might need a window.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  funchaddtar.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/09/04 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  func_addtarget (
   Chix     arg, 
   SWconf   conf)
{
   static Chix realurl = nullchix;
   static Chix testurl, target1, target2, href, quote, upper, final;
   int         len, state, copyfrom, scan, c1, c2, pos, cls;
   char        temp[2000];
   ENTRY ("func_addtarget", "");

   if (realurl == nullchix) {
      realurl  = chxalloc (L( 100),  THIN, "func_addtarget realurl");
      testurl  = chxalloc (L( 100),  THIN, "func_addtarget testurl");
      upper    = chxalloc (L(1000),  THIN, "func_addtarget upper");
      final    = chxalloc (L(1000),  THIN, "func_addtarget final");
      quote    = chxalloc (L(  10),  THIN, "func_addtarget quote");
      target1  = chxalsub (CQ("%_TARGET_FROMCAU"),    L(0), L(40));
      target2  = chxalsub (CQ("TARGET"),              L(0), L(40));
      href     = chxalsub (CQ("HREF="),               L(0), L(10));
   }

#define ST_TEXT 0
#define ST_HTML 1
#define ST_Q1   2
#define ST_Q2   3
   
   chxcpy   (upper, arg);
   chxupper (upper);
   len = chxlen (upper);

   /*** Scan for <A... > tags, and look inside them.   Use a finite-state
   /    machine to pluck them out into 'realurl', and then examine that
   /    further.   Copy entire resulting string to 'final'. */
   chxclear (final);
   state = ST_TEXT;
   copyfrom = 0;
   for (scan=0;   scan < len;   ++scan) {
      c1 = chxvalue (upper, scan);
      c2 = chxvalue (upper, scan+1);

      if (state == ST_TEXT) {
         if (c1 == '<'  &&  c2 == 'A') {
            state = ST_HTML;
            chxclear  (realurl);
            chxclear  (testurl);
            chxcatval (realurl, THIN, c1);
            chxcatval (testurl, THIN, c1);
         }
      }

      else if (state == ST_HTML) {
         chxcatval (testurl, THIN, c1);
         chxcatval (realurl, THIN, c1);

         if      (c1 == '"' )   state = ST_Q2;
         else if (c1 == '\'')   state = ST_Q1;

         /*** Here's where the real work is done. */
         else if (c1 == '>' ) {
            state = ST_TEXT;
            chxupper (testurl);
            chxupper (realurl);

            /*** Is there already a target window for this link?  Then
            /    we're done. */
            if (chxindex (testurl, 0L, target1) >= 0  ||
                chxindex (testurl, 0L, target2) >= 0)          continue;

            /*** If there's no HREF=, we're done. */
            if ( (pos = chxindex (realurl, 0L,    href)) < 0)  continue;

            /*** Find the stuff inside the HREF="...".  If none, we're done. */
            chxclear  (quote);
            chxcatsub (quote, realurl, pos+5, 1);
            if ( (cls = chxindex (realurl, pos+6, quote)) < 0) continue;
            chxclear  (testurl);
            chxcatsub (testurl, realurl, pos+6, (cls - pos - 6));

            /*** Based on the actual URL, should it get its own window? */
            if (url_window (testurl, conf)) {
               chxcatsub (final, arg, copyfrom, scan - copyfrom);
               chxcatval (final, THIN, L(' '));
               sprintf   (temp, 
                  " target=\"fromcau\" onClick=\"%%_target_fromcau (%d %d)\"", 
                    conf->new_win_x, conf->new_win_y);
               chxcat    (final, CQ(temp));
               copyfrom = scan;
            }
         }
      }

      else if (state == ST_Q1) {
         chxcatval (realurl, THIN, c1);
         if      (c1 == '\'')                  state = ST_HTML;
         else if (c1 == '\\'  &&  c2 == '\'')  {
            chxcatval (realurl, THIN, c2);
            ++scan;
         }
      }

      else if (state == ST_Q2) {
         chxcatval (realurl, THIN, c1);
         if      (c1 == '"')                   state = ST_HTML;
         else if (c1 == '\\'  &&  c2 == '"')   {
            chxcatval (realurl, THIN, c2);
            ++scan;
         }
      }
   }
   chxcatsub (final, arg, copyfrom, scan - copyfrom);
   chxcpy    (arg, final);

   RETURN (1);
}
