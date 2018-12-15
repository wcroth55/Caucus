/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** SET_XPHEADER.  Writes out (Sets) the header of a xport file.
/
/    int n = set_xpheader (unit, pheader)
/
/    Parameters:
/       int unit;
/       XPORTHDR *pheader;
/
/    Purpose:
/       This function writes out a header for a xport file from PHEADER.
/
/    How it works:
/       Setxpheader writes to UNIT.  Set_xpheader() assumes that UNIT
/       has been locked and opened by the caller, and will be
/       closed and unlocked by the caller.
/
/       If get_xpheader returns successfully, it must leave the file in
/       a state so that later reads will get any xactions after the
/       header.
/
/   Returns:
/      1 on success
/      0 on failure.  In this case it logs the error in the bugfile.
/
/   Error Conditions:
/      Can't write to UNIT.
/      Bad format for a xport file header.
/
/   Related functions: validxphead()
/
/   Called by: crexport()
/
/   Operating System dependencies: none
/
/   Home: xaction/setxphea.c
/ */

/*: JV  5/03/91 11:48 Create function. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/01/91 18:00 Clean. */
/*: JV  7/22/91 16:37 Add ENTRY */
/*: CR  8/01/91 11:34 Add CHUNK, LASTCHUNK fields. */
/*: CR  8/02/91 11:42 Remove CHUNK, LASTCHUNK fields. */
/*: JV 11/06/91 10:29 Changed common.h to null.h. */
/*: JV 11/11/91 13:55 Add confochn to header. */
/*: DE  6/05/92 13:17 Chixified */
/*: JX  9/01/92 16:46 Fixify. */

#include <stdio.h>
#include "caucus.h"
#include "handicap.h"
#include "null.h"
#include "xaction.h"
#define BADWRITE  bug ("SETXPHEAD: can't write header.", unit); success = 0; goto done

extern union null_t null;
extern Chix fieldtable;
extern Chix cnetvers;

FUNCTION set_xpheader (unit, pheader)
   int  unit;
   XPORTHDR *pheader;
{
   Chix line, p;
   int  success;

   ENTRY ("set_xpheader", "");

   line    = chxalloc (L(100), THIN, "setxphea line");
   p       = chxalloc (L(10),  THIN, "setxphea p");
   success = 0;

   /*** Write lines to UNIT.  Check first one to make sure all is well. */
   chxformat (line, chxquick("CVN=%s\n", 0), L(0), L(0), 
                    chxquick(pheader->version, 1), null.chx);
                                                       /* CaucusNet Version */
   if      (NOT unit_write (unit, line)) { BADWRITE; }

   chxclear  (line);
   chxformat (line, CQ("OCHN=%s\n"), L(0), L(0), pheader->origchn, null.chx);
                                                          /* Original CHN */
   unit_write (unit, line);

   chxclear  (line);
   chxformat (line, CQ("DCHN=%s\n"), L(0), L(0), pheader->destchn, null.chx);
   unit_write (unit, line);

   chxclear  (line);
   chxformat (line, chxquick("NSEQ=%s\n",0), L(0), L(0), 
                    chxquick(pheader->nayseq,1), null.chx);
   unit_write (unit, line);

   switch (pheader->typenum) {
      case (XCONF): chxofascii (p, "CONF");  break;
      case (XMSGS): chxofascii (p, "MSGS");  break;
      case (XACK):  chxofascii (p, "ACK");   break;
      case (XBROAD):chxofascii (p, "BROAD"); break;
      default: bug ("SET_XPHEADER: header has bad TYPE value: ", 
                 pheader->typenum);
               FAIL;
   }
   chxclear  (line);
   chxformat (line, CQ("TYPE=%s\n"), L(0), L(0), p, null.chx);
   unit_write (unit, line);

   /*** Optional fields: type of "CONF" has CONF field and maybe BROAD field.
   /    Note that BROAD field *must* precede CONF field by one line!  */
   if (pheader->typenum == XCONF) {
      chxclear  (line);
      chxformat (line, CQ("CONF=%s@%s\n"), L(0), L(0), pheader->trueconf, 
                 pheader->confochn);
      unit_write (unit, line);
      if (pheader->broadcntr[0] != L(0)) {
         chxclear  (line);
         chxformat (line, chxquick("BROADCNTR=%s\n",1 ), L(0), L(0), 
                          chxquick(pheader->broadcntr,0),
                    null.chx);
         unit_write (unit, line);
      }
   }
   SUCCEED;
 done:
   chxfree (line);
   chxfree (p);

   RETURN (success);
}
