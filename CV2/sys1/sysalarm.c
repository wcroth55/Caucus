/*** SYSALARM.  This file contains the alarm initialization routine,
/    the alarm handler, and the alarm setter routines.  The purpose
/    of the alarm timer is operating system dependent.
/
/    For AOS, we need to send a NULL message to the server every
/    SPEAKINT/2 seconds, because the Server forgets about any
/    client who it hasn't heard from in SPEAKINT seconds.
/    SPEAKINT is generally set to about 10 minutes, though the
/    actual value isn't important.  It should be low enough to
/    not put a burden on the IPC mechnaism.
/
/    sysalminit() sets up the alarm handler the first time around.
/
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  2/05/91 12:10 Created sysalarm(). */
/*: CR  4/25/91  0:51 Add include files so this compiles on Unix! */
/*: DE  5/27/92 15:12 Chixified */
/*: JX 10/08/92 11:00 Fixify. */
/*: CR 10/09/92 17:52 Comment out unused sysalarminit(), etc. */
/*: JV  3/29/93 16:20 Comment in  unused code so ranlib will run. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#if UNIX | NUT40 | WNT40

/*** Define stubs for all others, even though only sysalarmset()
/    is called for now (in sys/sysread.c). */

FUNCTION  sysalarminit()
{}

FUNCTION  sysalarm_handler()
{}

FUNCTION  sysalarmset()
{}

#endif




