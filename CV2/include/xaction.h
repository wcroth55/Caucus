/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** XACTION.H   This is the include file for data and #defines
/
/   Ideas for CaucusLink attachment transport:
/   Add fields:
/     Content-Type=text/plain; charset=US_ASCII
/     Content-Transfer-Encoding: 7bit
/     Content-Length: 1507
/
/    related to xactions. */

/*: JV  6/01/91 12:00 Created file. */
/*: JV  6/26/91 12:58 Added MAILCTRL field to xaction_t. */
/*: JV  7/01/91 15:13 Change macros for tables. */
/*: JV  7/05/91 09:51 Add entryex.h. */
/*: JV  7/12/91 15:08 Add mtype[], mtypenum, m_oparts to XACTION struct. */
/*: JV  7/15/91 16:32 Add DATA2 to XACTION struct, add MNOCHN. */
/*: CR  7/27/91 16:23 Add customize attributes. */
/*: CR  8/01/91 11:23 Add chunk/lastchunk fields to XPORTHDR. */
/*: CR  8/02/91 11:43 Remove chunk, lastchunk fields. */
/*: CR  8/13/91 14:29 Add SEQ_MOD. */
/*: JV 10/27/91 18:13 Add TT_SUBREQ. */
/*: JV 10/29/91 16:30 Add TT_SUBNAK. */
/*: JV 11/07/91 10:57 Add confochn to XACTION. */
/*: JV 11/11/91 14:02 Add confochn to XPORTHDR. */
/*: CR 11/22/91 13:17 Raise SEQ_MOD to 10000. */
/*: JV 12/14/91 13:53 Add SUBFREEZE, SUBTHAW, SUBTEXT, SUBTITLE. */
/*: JV 12/12/91 13:53 Add TT_HOST, XT_ATTREQ, XT_ATTACK. */
/*: JV 12/23/91 12:04 Add NOCONF, NOPERM. */
/*: CR  1/02/92 14:57 Add cus subject stuff. */
/*: JV  1/02/92 16:16 Add TT_SUBDEL. */
/*: JV  1/08/92 15:27 Add TT_ROUTES. */
/*: JX  5/29/92 16:50 Chixify. */
/*: DE  6/04/92 13:05 Change *data[] in XACTION ot data[]. */
/*: JV  2/24/93 12:48 Add fields for File Attachments, SUBATTACH, CUS_ATTACH. */
/*: CR  6/16/95 16:12 Add text properties. */

#include "entryex.h"
#include "chixuse.h"
#include "attach.h"

/* The structure for a xaction. */
/* Size of and maximum number of blocks of DATA in a XACTION structure. */
#define XACTBLKSIZ  2048
#define MAXXACTBLKS 400

/*** Sequence modulus (all sequence numbers are < SEQ_MOD). */
#define SEQ_MOD   10000L

/* Number of text fields in a xaction. */
#define TEXTFIELDS 2

typedef struct xaction_t {
   char  type[16];
   short typenum;
   char  action[12];
   short actnum;
   char  subtype[12];
   short subnum;
   Chix  conftname;     /* ConfTrueName */
   Chix  confochn;      /* Original CHN for conf */
   int4  conflocnum;
   Chix  uid;
   Chix  uname;
   Chix  unewname;
   Chix  phone;
   int4  itemid;
   Chix  itemchn;
   int4  respid;
   Chix  respchn;
   Chix  rechost;
   Chix  recid;
   Chix  recname;
   Chix  title;
   Chix  date;
   char  mtype[12];
   short mtypenum;
   short m_oparts;
   short rrlength;
   short mailctrl;
   Chix  daterecvd;
   int4  datasize[TEXTFIELDS];
   int   textprop;
   Chix  data[TEXTFIELDS];
   Attachment attach;
   char  transit;
} XACTION, *XACTIONPTR, *Xactionptr;

/* Define's for each of the known xaction field types.
/  (See xfpgloba.c for the definition of fieldtable[].) */
#define FT_TYPE     1
#define FT_SUBTYPE  2
#define FT_CONF     3
#define FT_UID      4
#define FT_NAME     5
#define FT_NEWNAME  6
#define FT_PHONE    7
#define FT_IID      8
#define FT_RID      9
#define FT_RECHOST 10
#define FT_RECID   11
#define FT_RECNAME 12
#define FT_TITLE   13
#define FT_DATE    14
#define FT_MTYPE   15
#define FT_M_OPARTS  16
#define FT_RRLEN     17
#define FT_MAILCTRL  18
#define FT_DATERECVD 19
#define FT_DATALEN1  20
#define FT_DATALEN2  21
#define FT_DATA1     22
#define FT_DATA2     23
#define FT_ATTACH_FORMAT    24
#define FT_ATTACH_NAME      25
#define FT_ATTACH_SIZE      26
#define FT_ATTACH_DATA_TYPE 27
#define FT_ATTACH_REC_LEN   28
#define FT_ATTACH_DATA      29
#define FT_TEXTPROP         30

/*  The #defines for typetable[];
/   (See xfpgloba.c for the definition.) */
#define TT_ITEM     1
#define TT_RESPONSE 2
#define TT_MESSAGE  3
#define TT_PERSON   4
#define TT_OSUBJECT 5
#define TT_CONFERENCE 6
#define TT_TITLE    7
#define TT_SUBREQ   8
#define TT_SUBACK   9
#define TT_SUBNAK  10
#define TT_HOST    11
#define TT_SUBDEL  12
#define TT_ROUTES  13

/*  The #defines for subtable[];
/   (See xfpgloba.c for the definition.) */
#define SUBUSER     1
#define SUBPORG     2
#define SUBADD      3
#define SUBCHANGE   4
#define SUBVIS      5
#define SUBGREET    6
#define SUBINTRO    7
#define SUBFREEZE   8
#define SUBTHAW     9
#define SUBTEXT    10
#define SUBTITLE   11
#define SUBSUBJECT 12
#define SUBATTACH  13

/* The defines for all the known types of actions.
/  (See xfpgloba.c for the defintion of actiontable[].) */
#define XT_NEW     1
#define XT_CHANGE  2
#define XT_DELETE  3
#define XT_REJECT  4
#define XT_TRANSIT 5
#define XT_INITIAL 6
#define XT_ATTREQ  7
#define XT_ATTACK  8

/* The defines for the MAILCTRL types. 
/  (See xfpgloba.c for the definition of mailctrltable[].) */
#define MNONE   1
#define MONE    2
#define MMANY   3
#define MFULL   4
#define MNOCHN  5

/* The defines for the error keywords.
/  (See xfpgloba.c for the definition of errtable[].) */
#define NOCONF 1
#define NOPERM 2

/*** Conference Customize attributes. */
#define  CUS_ADD        SUBADD
#define  CUS_CHANGE     SUBCHANGE
#define  CUS_SUBJECT    SUBSUBJECT
#define  CUS_VISIBILITY SUBVIS
#define  CUS_GREET      SUBGREET
#define  CUS_INTRO      SUBINTRO
#define  CUS_ATTACH     SUBATTACH

/*** Structure for xport file headers. */
typedef struct xportheader_t {
   char version[12];
   Chix origchn;
   Chix destchn;
   char nayseq[16];
   char type[16];
   int  typenum;
   char broadcntr[24];
   Chix trueconf;
   Chix confochn;
} XPORTHDR;

/* The types of transport files. */
#define XCONF   1
#define XMSGS   2
#define XACK    3
#define XBROAD  4    /* Broadcast */
