
/*** SYSEXIT.H.   Definition of error codes for sysexit() function
/    (and all its callers).  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/01/91 21:19 New file. */
/*: CR  7/29/91 23:10 Add EXIT_STACK. */
/*: WC 11/30/91 15:14 Add EXIT_MEM. */
/*: CR 12/30/91 11:38 Add EXIT_NOUID. */

#ifndef  SYSEXIT_H
#define  SYSEXIT_H

#define  EXIT_INIT          -1  /* Initialize sysexit. */
#define  EXIT_OK             0  /* Normal exit. */
#define  EXIT_ERR            1  /* Undifferentiated error, causing exit. */
#define  EXIT_MEM            2  /* Out of memory in heap. */
#define  EXIT_LOGIN        100  /* Bad name/password for -v automatic login. */
#define  EXIT_NOUID        101  /* No userid from cuserid()! */
#define  EXIT_CHX_NORESV   200  /* chxalloc cannot reserve memory */
#define  EXIT_CHX_NOMEM    201  /* chxalloc completely out of memory */
#define  EXIT_STACK        300  /* cstack overflow in entry_test! */

#endif
