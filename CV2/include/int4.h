
/*** INT4.H   Type Definition of a 4-byte int. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/26/96  9:21 New file. */

#ifndef INT4_H
#define INT4_H

#include "systype.h"

#if UNIX | NUT40 | WNT40
#define  INT4   1
typedef  int           int4;
typedef  unsigned int uint4;
#endif

#endif
