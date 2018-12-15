
/*** CHIXDEF.H.  Include file used inside chix function definitions. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:31 New file. */
/*: CR  4/30/91 23:47 Split off from old "chix.h" file. */
/*: CR  6/19/91 13:10 Move nullchix def from chixdef.h to chixuse.h */
/*: CR  7/10/91 13:09 Make chxresize() a public function. */
/*: JX  6/05/91 22:51 Add L(). */
/*: CR 10/09/92 13:09 #ifndef RETURN. */
/*: JV 10/16/92 16:23 Add CXE_SELF. */
/*: CR 12/11/92 12:50 SIZE returns long. */
/*: CR 12/21/92 14:26 Redefine CHX_MAGIC, CHIX_FREED. */
/*: CR  1/13/93 15:00 Move SIZE to chixuse.h */
/*: JV  9/08/93 10:23 #ifndef ENTRY. */
/*: CR  3/15/95 20:27 Add USE_WIDE_CHIX. */

#include "chixuse.h"
#ifndef ENTRY
#include "entryex.h"
#endif

/*** Currently, we only allow for two sizes of Chix: 1 and 2 bytes.
/    However, the chix code will cheerfully handle N byte chix if we
/    expand WIDEST appropriately. */
#define  THINNEST  1   /* Smallest # of bytes ever used by a chix. */
#define  WIDEST    2   /* Largest  # of bytes ever used by a chix. */

/*** For some language sets, we may prefer to always force chix sizes. */
#define  USE_THIN_CHIX  1
#define  USE_WIDE_CHIX  0

#define  CHIX_MAGIC   0x4348583AL /* Magic number ('CHX:') means 'is a chix' */
#define  CHIX_FREED   0x454D543AL /* Magic number ('EMT:') means freed chix */
#define  ISCHIX(a)    (a->ischix == CHIX_MAGIC)
#define  FREED(a)     (a->ischix == CHIX_FREED)
#define  RESIZE_LIM   100        /* Warn if too many chxresizes on a chix. */

/*** Usual macros for nice reading. */
#define  FUNCTION
#define  NOT        !
#ifndef  L
#define  L(x)              ( (int4) x )
#endif
#ifndef  ENTRY
#define  ENTRY(a,b)
#endif
#ifndef  RETURN
#define  RETURN(x)  return(x)
#endif

/*** Structure for collation rules. */
struct rule_t {
   int4  a;
   int4  b;
   int4  c;
   int4  i;
   struct rule_t *next;
};

/*** Chix error codes, handled by chxerror(). */
#define  CXE_NOERROR   0     /* No error (should never be used!) */
#define  CXE_NORESV    1     /* Could not get "reserve" memory in chxmem! */
#define  CXE_NULL      2     /* chix arg is null ptr! */
#define  CXE_FREED     3     /* chix arg was already freed! */
#define  CXE_NOTCHX    4     /* chix arg is not a chix. */
#define  CXE_LOMEM     5     /* very low on heap memory! */
#define  CXE_NOMEM     6     /* completely out of heap memory. */
#define  CXE_RESIZE    7     /* chix resized > RESIZE_LIM times. */
#define  CXE_NOEND     8     /* endless chix discovered & fixed. */
#define  CXE_SELF      9     /* same chix used for both chix args in a call. */


/*** Declaration of return values of "private" chix functions, i.e. those
/    that should only be used inside other chix functions. */
int   chxcapc();
int   chxcheck();
int   chxerror();
char *chxmem();
int4  chxscan();
