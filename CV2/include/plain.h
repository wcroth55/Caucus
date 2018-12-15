
/*** PLAIN.H.   Definitions for en/decrypting license block information.
/
/    The Caucus license structure contains int4 integer and string
/    components which are kept "encrypted" by xor-ing with a mask.
/    This file contains the macros used in the process of decrypting
/    this information.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/25/93 15:56 New file. */

#ifndef PLAIN_H
#define PLAIN_H

#include "int4.h"

#if INT4
#define PLAIN_ENC_LONG  0xAA55AA55
#else
#define PLAIN_ENC_LONG  0xAA55AA55L
#endif

#define PLAIN_ENC_STR   \
   "\252\225\252\225\252\225\252\225\252\225\252\225\252\225\252\225"
#define PLAIN(x)  ( (x) ^ PLAIN_ENC_LONG )

#endif
