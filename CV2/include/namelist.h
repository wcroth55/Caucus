/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** Name list structures. */

/*: CR  7/01/91 18:37 Put in carvmark header. */
/*: CR  7/09/91 18:33 Define typedef "Namelist". */
/*: CR  7/10/91 14:19 Add NLE_... error codes. */
/*: CR  8/02/91 12:38 Add PUNCT/NOPUNCT. */
/*: CX 10/17/91 13:15 Chixify Namelist's. */

#ifndef NAMELIST_H
#define NAMELIST_H

struct namlist_t {
   Chix   str;
   short  num;
   struct namlist_t *next;
};
#define NLNULL   ((struct namlist_t *) NULL)

typedef struct namlist_t *  Namelist;

#define  PUNCT     0    /* Keep punctuation in modnames()/matchnames(). */
#define  NOPUNCT   1    /* Remove punctuation in modnames()/matchnames(). */

/*** Namelist error codes. */
#define  NLE_NOERR    0     /* No error */
#define  NLE_EMPTY    1     /* Too many names were empty. */
#define  NLE_DBERR    2     /* Error in database--can't lock or open files. */

#endif
