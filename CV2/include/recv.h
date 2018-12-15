/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** REC.H.  */

/*: JX  7/31/92 Chixify. */

#include "chixuse.h"

typedef struct trans_t *  Trans;

/*** Values for trans_t.acktype field. */
#define  NOTACK  -1
#define  NAK      0
#define  ACK      1

#define  EMPTYTRANS  -999L   /* Empty node. */

/*** Each transport file gets a node in a linked list. */
struct trans_t {
   int4    seq;              /* Sequence number of this file. */
   Chix    chn;              /* Originating CHN of this file. */
   int4    first_receipt;    /* When first chunk was first received. */
   int     highchunk;        /* Highest chunk number seen. */
   int     gotlast;          /* Did we get the last chunk? */
   int4   *xnums;            /* Array of Xnnnnnnn's of chunk files. */
   int     xsize;            /* Size of XNUMS. */
   int     acktype;          /* ACK or NAK? */
   int4    ackseq;           /* If ACK, seq number of file being ack'd. */
   struct  trans_t *next;    /* Next node in linked list. */
};
