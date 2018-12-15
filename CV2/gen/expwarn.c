
/*** EXPWARN.  Warn user, if desired, about expiration date.
/
/    If this license expires, and either Caucus was invoked with
/    the -w option, or the user had SET EXPIRATION ON, display
/    a warning about when Caucus expires.  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: CR  1/17/89 12:16 New function. */
/*: CW  5/18/89 14:25 Add XW unitcodes. */
/*: JV 10/31/91 15:13 Declare sprintf */
/*: DE  3/31/92 12:41 Chixified */
/*: JX  6/06/92 19:50 Add FORMAT arg to sysdaytime(). */
/*: CR  1/26/93 15:49 Use PLAIN and strplain() to decrypt license info. */


expwarn() { return 0; }
