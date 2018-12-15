/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** GET_XPHEADER.  Gets the header of a xport file.
/
/    int n = get_xpheader (unit, pheader)
/
/    Parameters:
/       int unit;
/       XPORTHDR *pheader;
/
/    Purpose:
/       This function gets the header for a xport file and 
/       stores it in PHEADER.
/
/    How it works:
/       Get_xpheader reads from UNIT.  Get_xpheader() assumes that UNIT
/       has been locked and opened by the caller, and will be
/       closed and unlocked by the caller.
/       It parses the field name for each line and stores the value
/       associated with it into the PHEADER structure.
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
/      Can't lock, open, or read UNIT.
/      Bad format for a xport file header.
/
/   Related functions: validxphead()
/
/   Called by: 
/
/   Operating System dependencies: none
/
/   Home: xaction/getxphea.c
/ */

/*: JV  4/17/91 12:53 Create function. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/01/91 18:00 Clean. */
/*: JV  7/04/91 17:57 Remove lint. */
/*: JV  7/22/91 16:36 Add ENTRY */
/*: CR  8/01/91 11:32 Add CHUNK, LASTCHUNK fields. */
/*: CR  8/02/91 11:42 Remove CHUNK, LASTCHUNK fields. */
/*: JV 11/06/91 10:29 Change common.h to null.h. */
/*: JV 11/11/91 13:55 Add confochn to header. */
/*: JV 11/25/91 10:08 Create empty header. */
/*: DE  6/04/92 13:25 Being chixified... */
/*: JX  6/04/92 15:11 Finish chixification. */
/*: DE  6/05/92 14:54 Add extern XPORTHDR empty_xpthdr; */
/*: JX  9/14/92 13:53 Empty header struct with empty_xpheader(). */
/*: JX 10/05/92 09:40 Change empty_xpheader() to clear_xpheader(). */
/*: CL 12/10/92 17:07 Long ascofchx args. */
/*: CR 12/15/92 14:07 Replace ALLCHARS in ascofchx with size of char array. */

#include <stdio.h>
#include "caucus.h"
#include "handicap.h"
#include "goodlint.h"
#include "null.h"
#include "xaction.h"
#define BADREAD  bug ("GETXPHEAD: can't read header.", unit); success = 0; goto done;

extern union null_t null;
extern Chix fieldtable;

FUNCTION get_xpheader (unit, pheader)
   int  unit;
   XPORTHDR *pheader;
{
   Chix line, value, name;
   int  success;

   ENTRY ("get_xpheader", "");

   line    = chxalloc (L(100), THIN, "getxphea line");
   value   = chxalloc (L(80),  THIN, "getxphea value");
   name    = chxalloc (L(16),  THIN, "getxphea name");
   success = 0;

   clear_xpheader (pheader);

   /*** Read first line from UNIT. It must be CVN. */
   if (NOT unit_read (unit, line, L(0))) { BADREAD; }
   if (NOT chxbreak (line, name, value, CQ("="))) { BADREAD; }
   if (NOT chxeq (name, CQ("CVN"))) { BADREAD; }
   ascofchx (pheader->version, value, L(0), SIZE(pheader->version));

   /* Next is Originating CHN */
   if (NOT unit_read (unit, line, L(0))) { BADREAD; }
   if (NOT chxbreak (line, name, value, CQ("="))) { BADREAD; }
   if (NOT chxeq (name, CQ("OCHN"))) { BADREAD; }
   chxcpy (pheader->origchn, value);
                                                      /* Destination CHN */
   if (NOT unit_read (unit, line, L(0))) { BADREAD; }     
   if (NOT chxbreak (line, name, value, CQ("="))) { BADREAD; }
   if (NOT chxeq (name, CQ("DCHN"))) { BADREAD; }
   chxcpy (pheader->destchn, value);
                                                      /* Naybor SeqNum */
   if (NOT unit_read (unit, line, L(0))) { BADREAD; }
   if (NOT chxbreak (line, name, value, CQ("="))) { BADREAD; }
   if (NOT chxeq (name, CQ("NSEQ"))) { BADREAD; }
   ascofchx (pheader->nayseq,  value, L(0), SIZE(pheader->nayseq));
                                                   /* Type of xport file */
   if (NOT unit_read (unit, line, L(0))) { BADREAD; }
   if (NOT chxbreak (line, name, value, CQ("="))) { BADREAD; }
   if (NOT chxeq (name, CQ("TYPE"))) { BADREAD; }

   if (chxeq (value, CQ("CONF")))  pheader->typenum = XCONF;
   if (chxeq (value, CQ("MSGS")))  pheader->typenum = XMSGS;
   if (chxeq (value, CQ("ACK")))   pheader->typenum = XACK;
   if (chxeq (value, CQ("BROAD"))) pheader->typenum = XBROAD;
   ascofchx (pheader->type, value, L(0), SIZE(pheader->type));

   /*** Optional fields: type "CONF" has CONF field,
   /    type "BROAD" has BROAD field.  */
   if (pheader->typenum == XCONF) {
      if (NOT unit_read (unit, line, L(0))) { BADREAD; }
      if (NOT chxbreak (line, name, value, CQ("="))) { BADREAD; }
      if (chxeq (name, CQ("CONF")))
         chxbreak (value, pheader->trueconf, pheader->confochn, CQ("@"));
      else  { BADREAD }
      strcpy (pheader->broadcntr, "");
   }
   if (pheader->typenum == XBROAD) {
      if (NOT unit_read (unit, line, L(0))) { BADREAD; }
      if (NOT chxbreak (line, name, value, CQ("="))) { BADREAD; }
      if (chxeq (name, CQ("BROADCNTR")))
         ascofchx (pheader->broadcntr, value, L(0), SIZE(pheader->broadcntr));
      else { BADREAD; }
      chxcpy (pheader->trueconf, CQ(""));
   }
   SUCCEED;

 done:

   chxfree ( line );
   chxfree ( value );
   chxfree ( name );

   RETURN ( success );
}

