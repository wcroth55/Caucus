
/*** LIMITUSERS.    Limit the number of simultaneous users.
/
/    ok = limitusers (getslot);
/
/    Parameters:
/       int    ok;          (success?)
/       int    getslot;     (1 => get a slot;  0 => free my slot)
/
/    Purpose:
/       Some versions of Caucus are sold with a license for a maximum
/       number of users.  If this is one of them, enforce that limit.
/   
/    How it works:
/       When GETSLOT is 1, LIMITUSERS attempts to assign a "slot" to
/       this user for the duration of the execution of the program.  If
/       a slot has already been assigned to this user, LIMITUSERS ensures
/       that no one can cheat by assigning one slot to multiple users.  If
/       the slot was successfully allocated (and no cheating has happened),
/       LIMITUSERS returns 1.  Otherwise, it returns 0.
/   
/       When GETSLOT is 0, LIMITUSERS frees the slot for use by another.
/       (Typically just before Caucus exits.)
/   
/       Each slot is associated with a file, usually called 'mxusr000 through
/       mxusrnnn.  The file is created with the time (returned by SYSTIME)
/       that the slot was allocated.
/
/    Returns: 1 on success
/             0 on no more slots, or detection of "cheating".
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  gen/limituse.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: JV 11/26/90 16:03 Just thought I'd define GETSLOT... */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: JV 10/31/91 15:14 Declare sprintf(). */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: JX  5/13/92 17:04 Fix nullchix. */
/*: JV  8/13/92 13:13 Declare other sprintf(). */
/*: CI 10/05/92 16:41 Chix Integration. */
/*: CR 12/11/92 15:33 Long chxint4 arg. */
/*: CR  1/26/93 15:49 Use PLAIN and strplain() to decrypt license info. */
/*: CK 10/14/93 15:14 Add hdr comments; use hash(uid) for start mxuser # */
/*: CK 10/15/93 15:24 Look for empty slot MAXTRY times; then look for reuse. */
/*: CR 10/17/93 17:54 Remove use of 'min()'. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#define   MAXTRY   10

limitusers (getslot)  { return 1; }
