/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** NO.H.   Definition of "no_t" structure, used in server code *only*,
/    as a collection of null pointers used to keep Lint happy. */

/*: CR 11/16/88  0:17 Initial release of server code. */
/*: CR  7/01/91 20:33 Add comments, #ifndef stuff. */

#ifndef NO_H
#define NO_H

struct no_t {
   file            good;
   struct queue_t *queue;
   struct pdir_t  *pdir;
   struct ready_t *ready;
};

#endif
