/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** AUDIT_XACTION. Write out a one-line summary of a xaction to a log file.
/
/   int n = audit_xaction (pxaction, number, direction)
/
/   Parameters:
/      XACTIONPTR pxaction;
/      int   number;
/      int   direction;
/
/   Purpose:
/      Crexport() calls this function to log every xaction that this
/      host sends out.  Somebody from the XFU also calls this function
/      to log every xaction that this host receives.  This is mostly
/      for billing and diagnostic purposes.
/
/   How it works:
/      The meaning of NAME depends on what file we're writing the audit to.
/
/      The meaning of NUMBER depends on what file we're writing the audit
/      record to.  For host audits, it's the LHNUM of the host.  For
/      conference audits, it's the CONFNUM of the conference. (?)
/
/      DIRECTION is either AUDIT_IN or AUDIT_OUT, to distinguish between
/      xactions being received or shipped..
/
/      This table shows which program (XFP or XFU) actually logs an entry
/      for each of the types of audits:
/
/      Audit Type                  Logger
/      ----------                  ------
/      HOST_MAIL_IN                unpack
/      HOST_MAIL_OUT               pack
/      HOST_CONF_IN                unpack
/      HOST_CONF_OUT               pack
/      HOST_PEOPLE_IN              unpack
/      HOST_PEOPLE_OUT             pack
/
/      USER_CONF                   pack
/      USER_MAIL_IN                unpack
/      USER_MAIL_OUT               pack
/
/      CONF_OUT                    pack
/      CONF_IN                     unpack
/      XFP_TIME                    pack
/      XFU_TIME                    unpack
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
/   Home: xaction/auditxac.c
/ */

/*: JV 10/17/91 18:34 Create this. */
/*: JV 10/30/91 11:52 Changed name of variable audit to audit_flag. */
/*: CR 10/29/91 13:43 Add ENTRY for audit_line(). */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: JV  1/03/92 12:26 Add TT_SUB[REQ ACK NAK] and TT_HOST, add tbl to docs. */
/*: DE  6/02/92 14:27 Chixified */

#include "handicap.h"
#include <stdio.h>
#include "chixuse.h"
#include "done.h"
#include "null.h"
#include "entryex.h"
#include "derr.h"
#include "debug.h"
#include "unitcode.h"
#include "xaction.h"
#include "audit.h"

extern int debug;
extern Audit audit_flag;
extern union null_t null;

FUNCTION audit_xaction (pxact, number, direction)
   XACTIONPTR pxact;
   int  number, direction;
{
   int  datasize, i;
   Chix uid, chn;

   ENTRY ("audit_xaction", "");

   uid     = chxalloc (L(40), THIN, "auditxac uid");
   chn     = chxalloc (L(80), THIN, "auditxac chn");

   if (debug | DEB_SPC1) logx ("Audit trail: ", pxact);

   for (i=0, datasize=0; i<TEXTFIELDS; datasize += pxact->datasize[i++]);

   switch (pxact->typenum) {
   case TT_ITEM:
   case TT_RESPONSE:
   case TT_OSUBJECT:
   case TT_CONFERENCE:
      if (audit_flag.host_conf_in && direction == AUDIT_IN)
         audit_line (XAHCI, datasize, null.chx, number, direction);

      if (audit_flag.host_conf_out && direction == AUDIT_OUT)
         audit_line (XAHCO, datasize, null.chx, number, direction);

      if (audit_flag.user_conf && direction == AUDIT_OUT) {
         chxbreak (pxact->uid, uid, chn, CQ("@"));
         audit_line (XAUCO, datasize, uid, number, direction);
      }

      if (audit_flag.conf_in && direction == AUDIT_IN)
         audit_line (XACCI, datasize, null.chx, (int) pxact->conflocnum,
          direction);

      if (audit_flag.conf_out && direction == AUDIT_OUT)
         audit_line (XACCO, datasize, null.chx, (int) pxact->conflocnum,
          direction);

      break;
   case TT_MESSAGE:
      if (audit_flag.host_mail_out && direction == AUDIT_OUT)
         audit_line (XAHMO, datasize, null.chx, number, direction);

      if (audit_flag.host_mail_in && direction == AUDIT_IN)
         audit_line (XAHMI, datasize, null.chx, number, direction);

      if (audit_flag.user_mail_out && direction == AUDIT_OUT) { 
         chxbreak (pxact->uid, uid, chn, CQ("@"));
         audit_line (XAUMO, datasize, uid, number, direction);
      }

      if (audit_flag.user_mail_in && direction == AUDIT_IN) {
         chxbreak (pxact->recid, uid, chn, CQ("@"));
         audit_line (XAUMI, datasize, uid, number, direction);
      }

      break;
   case TT_PERSON:
      break;
   case TT_SUBREQ:
   case TT_SUBACK:
   case TT_SUBNAK:
   case TT_HOST:
      if (audit_flag.host_conf_in && direction == AUDIT_IN)
         audit_line (XAHCI, datasize, null.chx, number, direction);

      if (audit_flag.host_conf_out && direction == AUDIT_OUT)
         audit_line (XAHCO, datasize, null.chx, number, direction);
      
      break;
   default:
      logx ("Audit trail error: ", pxact);
   }

   chxfree ( uid );
   chxfree ( chn );

   RETURN (0);
}   


FUNCTION audit_line (unit, size, name, number, direction)
   int unit, size, number, direction;
   Chix  name;
{
   Chix line, stamp, day, time;
   int4 systime();
   int  success;

   ENTRY ("audit_line", "");

   line    = chxalloc (L(100), THIN, "audit_line line");
   stamp   = chxalloc (L(80),  THIN, "audit_line stamp");
   day     = chxalloc (L(30),  THIN, "audit_line day");
   time    = chxalloc (L(30),  THIN, "audit_line time");
   success = 0;

   switch (unit) {
   case XAHMI:
   case XAHMO:
   case XAHCI:
   case XAHCO:
   case XAHPI:
   case XAHPO:
   case XACCO:
   case XACCI:
      if (NOT unit_lock (unit, WRITE, L(number), L(0), L(0), L(0), null.chx)) {
         bug ("AUDIT_LINE: can't lock Audit file: ", unit);
         FAIL; }
      break;
   case XAUCO:
   case XAUMO:
   case XAUMI:
      if (NOT unit_lock (unit, WRITE, L(0), L(0), L(0), L(0), name)) {
      bug ("AUDIT_LINE: can't lock User Audit file: ", unit);
      FAIL; }
      break;
   default:
      bug ("AUDIT_LINE: bad unit number: ", unit);
   }

   if (NOT unit_append (unit)) {
      bug ("AUDIT_LINE: can't open Audit file: ", unit);
      FAIL; }

   sysdaytime (day, time, 0, systime());
   chxformat (stamp, CQ("%s %s"), L(0), L(0), null.chx, null.chx);

   chxformat (line, chxquick("%d characters %s at %s.\n",0), L(size), L(0), 
            direction == AUDIT_IN ? chxquick("received",1) :
                                    chxquick("shipped", 2), stamp);
   unit_write (unit, line);

   unit_close (unit);
   unit_unlk  (unit);

 done:
   chxfree ( line );
   chxfree ( stamp );
   chxfree ( day );
   chxfree ( time );

   RETURN ( success );
}
