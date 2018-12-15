/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/* XFPGLOBALS.  "The" place where all the globals hang out.
/
/  int n = xfpglobals()
/
/  Parameters: none
/
/  Purpose: To get them off the streets.
/
/  How it works: Provide them with a fun but challenging environment,
/     and they'll tend to congregate there.
/
/  Returns: Fewer violent bugs.  We hope.
/
/  Error conditions: If the number of bugs rises again.
/
/  Side Effects: Some of the globals may turn into responsible citizens.
/
/  Related functions: Ballroom dancing every Wednesday night.
/
/  Called by: The XFP Himself (The Main() Man).  The XFU, too.
/
/  Operating system dependencies: none
/
/  Known bugs: none
/
/  Home: xaction/xfpgloba.c
*/

/*: JV  5/01/91 00:00 Created code. */
/*: JV  6/26/91 13:22 Added mailctrl stuff. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/01/91 19:40 Add MYCHN. */
/*: JV  7/02/91 13:21 Remove TEXT buffer structurs, MYCHN, other globs. */
/*: JV  7/09/91 09:44 Move empty_xact here from getnextx.c */
/*: JV  7/12/91 15:08 Add MESSTYPE, O_PARTS to fieldtable, add messtable */
/*: JV  7/15/91 16:32 Add DATATEXT2 to fieldtable. */
/*: JV  7/17/91 12:51 Add DATALEN2 to fieldtable. */
/*: JV 10/22/91 15:10 Add audit, net_flag, onofftable. */
/*: JV 10/29/91 15:29 Add TITLE and SUBREQ to typetable. */
/*: JV 10/30/91 11:52 Changed name of variable audit to audit_flag. */
/*: JV 10/31/91 09:37 Make empty_xact readable. */
/*: JV 11/06/91 10:29 Change common.h to null.h, add SUBNAK. */
/*: JV 11/22/91 17:56 Switch MREPLY and MFORWARD, chg emtpy_xact.mtype#. */
/*: JV 12/04/91 13:56 Add FREEZE, THAW, TITLE, TEXT to subtable. */
/*: JV 12/12/91 11:14 Add Neighbor Status String, HOST, ATTREQ, ATTACK. */
/*: JV 12/23/91 11:59 Add errtable, NOCONF, NOPERM. */
/*: CR  1/02/92 15:08 Add SUBJECT to xfptable. */
/*: JV  1/03/92 10:28 Add SUBDEL to typetable. */
/*: CR  1/10/92 21:21 Remove onofftable. */
/*: JX  5/29/92 16:34 Chixify. */
/*: DE  6/05/92 14:22 Compile time str init -> run time Chix init */
/*: JX  6/15/92 15:13 Put globals back outside function. */
/*: JX  8/31/92 17:39 Fix Chix definitions. */
/*: CR 10/09/92 18:03 Comment out empty_x...  */
/*: JV  2/24/93 13:05 Add fields for File Attachments; add Allow Attach. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "attach.h"
#include "xaction.h"
#include "xfp.h"
#include "audit.h"
#include "caucus.h"
#include "null.h"


/* XACTION empty_xact = { */
/* "", 0, "", 0, "", 0,                              */ /* Type - SubNum */
/* nullchix, nullchix, L(0), nullchix, nullchix,       */ /* Conftname - Phone */
/* nullchix, nullchix, L(0), nullchix, L(0), nullchix,   */ /* ItemID - RespCHN */
/* nullchix, nullchix, nullchix, nullchix, nullchix, */ /* RecHost - Date */
/* "", -1, 0, -1, 0, nullchix,                       */ /* MType  - DateRecvd */
/* L(0), L(0), nullchix, nullchix, '\0'                  */ /* DataSize - Transit*/
/*}; */

/*
XPORTHDR empty_xpthdr = { "", nullchix, nullchix, "", "", 0, "", nullchix,
                          nullchix };
*/

Audit audit_flag;

struct net_flag_t net_flag;

extern struct screen_template screen, used;
extern union  null_t          null;
extern Userreg       thisuser;

Chix       cnetvers,      /* See below for explanations of these. */
           fieldtable,
           typetable,
           actiontable,
           subtable,
           messtable,
           mctrltable,
           errtable,
           naystat;

FUNCTION xfpglobals ()
{
   thisuser->dict = 0;
   screen.lines = 23;  used.lines = 0;
   screen.width = 80;  used.width = 0;

   cnetvers = chxalloc (L(5), THIN, "xfpgloba cnetvers");
   chxofascii (cnetvers, "1.1");

/* The table that contains all the known xaction fieldnames.
/  (See xaction.h for the #defines for these.) */
   fieldtable = chxalloc (L(220), THIN, "xfpgloba fieldtable");
   chxofascii (fieldtable, "TYPE SUBTYPE CONF UID NAME NEWNAME PHONE IID RID \
    RECHOST RECID RECNAME TITLE XACTDATE MESSTYPE O_PARTS RRLEN MAILCTRL \
    RECVDATE DATALEN1 DATALEN2 DATATEXT1 DATATEXT2 ATTACH_FORMAT ATTACH_NAME \
    ATTACH_SIZE ATTACH_DATA_TYPE ATTACH_REC_LEN ATTACH_DATA TEXTPROP");

/* The table that contains all the known types of xactions.
/  (See xaction.h for the #defines for these.) */
   typetable = chxalloc (L(120), THIN, "xfpgloba typetable");
   chxofascii (typetable, "ITEM RESPONSE MESSAGE PERSON OSUBJECT CONFERENCE \
    TITLE SUBREQ SUBACK SUBNAK HOST SUBDEL");

/* The table that contains all the known types of xactions.
/  (See xaction.h for the #defines for these.) */
   actiontable = chxalloc (L(80), THIN, "xfpgloba actiontable");
   chxofascii (actiontable, "NEW CHANGE DELETE REJECT TRANSIT INITIAL ATTREQ \
    ATTACK");

/* The table that contains all the known xaction subtypes. */
   subtable = chxalloc (L(120), THIN, "xfpgloba subtable");
   chxofascii (subtable, "USERLIST PORGANIZER ADD CHANGE VISIBILITY GREET \
    INTRO FREEZE THAW TEXT TITLE SUBJECT ATTACH");

/* The table that contains all the known types of messages. */
   messtable = chxalloc (L(40), THIN, "xfpgloba messtable");
   chxofascii (messtable, "MSEND MFORWARD MREPLY");

/* The table that contains all the known types of MAILCTRL keywords. */
   mctrltable = chxalloc (L(40), THIN, "xfpgloba mctrltable");
   chxofascii (mctrltable, "MNONE MONE MMANY MFULL MNOCHN");

/* The table that contains all the error keywords in xactions. */
   errtable = chxalloc (L(20), THIN, "xfpgloba errtable");
   chxofascii (errtable, "NOCONF NOPERM");

/* The table that contains the Neighbor Statuses. */
   naystat = chxalloc (L(60), THIN, "xfpgloba naystat");
   chxofascii (naystat, "UNKNOWN UNATTACHED PENDING ATTACHED DETACHED");
}
