/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** PARTIC.H.   Definition of participant record. */

/*: CR  7/01/91 18:41 Separate partic definition to its own file. */
/*: JX  5/21/92 11:12 Chixify. */
/*: CR 10/27/92 15:11 Promote backresp from char to short. */
/*: CR  1/13/93 13:32 Change responses, backresp members to ptrs. */

#ifndef PARTIC_H
#define PARTIC_H

#include "maxparms.h"

/*** User participation record for a conference. */
struct partic_t {
   short  items;       /* # of items user has "seen" */
   short *responses;   /* array of # of responses user has seen, per item */
   short  messages;    /* Number of messages?????? */
   short *backresp;    /* array: go back this # resp's when viewing new. */
   Chix   lastin;
};

#endif
