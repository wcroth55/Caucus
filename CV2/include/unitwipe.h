/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */


/*** UNITWIPE.H.   I/O codes for unit_wipe() function. */

/*: CR  7/01/91 23:12 New file. */
/*: CR  9/04/91 16:21 Add LZZZ. */

#ifndef UNITWIPE_H
#define UNITWIPE_H

/*** Types of I/O for unit_wipe(). */
#define  LSYS  0
#define  LUSR  1
#define  LCNF  2
#define  LHST  3
#define  LSHP  4
#define  LRCV  5

#define  LZZZ  5   /* Always highest Lxxx number. */
#endif
