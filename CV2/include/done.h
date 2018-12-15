/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** DONE.H.   Definition of the FAIL and SUCCEED macros. */

/*: CR 12/26/91 13:01 New include file. */
/*: DE  3/18/92 15:50 Add DONE macro */

#ifndef DONE_H
#define DONE_H

#define  FAIL      { success = 0;     goto done; }
#define  SUCCEED   { success = 1;     goto done; }
#define  DONE(val) { success = val;   goto done; }

#endif
