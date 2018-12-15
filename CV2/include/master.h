/*** MASTER.H.  Definition of "master_template" structure for
/    in-memory record of XCMA "masteres" conference record,
/    and related objects.  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/01/91 23:16 New file.*/
/*: CR  9/16/91 13:58 Make responses, origitem, orighost ptrs. */
/*: CR  1/02/92 14:23 Add master_template.chgsubj. */
/*: CX 10/20/91 16:40 Chixify master_template. */
/*: CI 10/04/92 23:35 Chix Integration. */
/*: JV 11/17/92 17:04 Change orighost from Chix to short. */
/*: CR  2/28/93 23:05 Add 'lastused' member, typedef 'Master'. */
/*: JV  2/24/92 16:56 Add allow_attach field. */

#ifndef MASTER_H
#define MASTER_H

#include "maxparms.h"
#include "chixuse.h"

/*** MASTER file information: */
struct master_template {
   short  items;               /* # of items on conference */
   short  nochange;            /* Let users change their responses? */
   short  noenter;             /* Let users enter their own items? */
   Chix   organizerid;         /* userid of primary organizer. */
   short *responses;           /* # of responses on each item */
   short *origitem;            /* number of this item on original host. */
   short *orighost;            /* index into array original hostnames. */
/* Chix  *orighost;               name of original host. */
   int    cnum;                /* master contains XCMA info for this conf. */
   short  chgsubj;             /* Let users CHANGE OSUBJECT? */
   int4   lastused;            /* Last use count, for caching. */
   short  allow_attach;        /* Let users Add Attachments? */
}; 
 
typedef  struct master_template *  Master;

#define DELETED(i)   (master.responses[i] < 0)

/*** Record of things FRESH at the beginning of the session. */
struct fresh_template  {
   short int items;               /* # of items user had seen at start. */
   short int responses[MAXITEMS]; /* # of responses user had seen per item */
   short     backresp [MAXITEMS]; /* Go back this # responses on viewing. */
};

#endif
