
/*** TYPES.H: definitions of special (system) data types. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/31/98 16:39 New file. */

#ifndef TYPES_H
#define TYPES_H

#define SIZE_T  size_t

/*** Some systems do not typedef mode_t.  If this is one of them,
/    change "#if 0" to "#if 1". */
#if 0
typedef int  mode_t;
#endif

#endif
