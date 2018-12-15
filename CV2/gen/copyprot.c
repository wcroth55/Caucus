
/*** COPYPROTECT.  Check license & expiration protection.
/
/    Caucus is usually licensed by a unique CPU identifier, or
/    with an expiration date.  COPYPROTECT returns 1 if this
/    is a proper license that hasn't expired.  Otherwise it
/    complains to the user and returns 0. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: CW  5/18/89 14:25 Add XW unitcodes. */
/*: DE  3/31/92 12:32 Chixified */
/*: JX  5/27/92 15:19 De-Chixified license. */
/*: CR  1/26/93 15:49 Use PLAIN and strplain() to decrypt license info. */

#include "caucus.h"

extern union  null_t     null;
extern struct license_t  license;

FUNCTION  copyprotect()  { return 1; }
