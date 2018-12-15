/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** ERRCHK.H.   ERRCHK macro definition for VC calls. */

/*: CR 11/16/88  0:17 Initial release of server code. */
/*: CR  7/01/91 20:25 Add #ifndef stuff. */

#ifndef ERRCHK_H
#define ERRCHK_H

/*** VC Error checking macro. */
#define ERRCHK(x) { if (vcernum) printf ("%s: %d %s\n", x, vcernum, vcernam); }

#endif 
