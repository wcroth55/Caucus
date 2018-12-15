
/*** OBJECT.H.   Include file for Caucus "objects". */
 
/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/25/98 13:44 New file. */

#ifndef  OBJECT
#define  OBJECT  1

#include "api.h"

#ifndef  FUNCTION
#define  FUNCTION
#endif

#ifndef  NOT
#define  NOT  !
#endif

typedef  struct object_t * Object;
struct object_t {
   char  name [100];    /* Object name (reference, includes Del). */
   char  class[100];    /* Class name. */
   char *memnames;      /* List of member names (not counting properties). */
   int   members;       /* Number of members (ditto). */
   int   persists;      /* Persistence. */
   int   wait;          /* Waiting for update? (See $ob_wait().) */
   int4  updated;       /* Time last written to database. */
   long  lockfd;        /* lock file descriptor, 0 => not locked */
   Chix *value;         /* Array of member values. */
   char *taint;         /* Array of tainted (updated but not stored) values. */
};

typedef  struct obtab_t  * Obtab;
struct   obtab_t {
   int     max;       /* Max size of array. */
   int     used;      /* Slots used in the array. */
   char    err[8];    /* Error object. */
   Vartab  class;     /* Class list. */
   Object *obj;       /* Array of objects. */
};

#endif
