/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** ADDRESS.H.   Definition of the "address" of a person. */

/*: CR  7/10/91 16:39 New file. */
/*: CR  7/16/91 12:17 Add NULLADDRESS definition. */
/*: CX 11/12/91  0:04 Chixify. */
/*: CR 12/20/91 13:01 Add NOPERSON error code. */
/*: CI 10/04/92 23:29 Chix Integration. */

#ifndef ADDRESS_H
#define ADDRESS_H

struct address_t {
   Chix   name;
   Chix   id;
   Chix   host;
};

typedef  struct address_t *  Address;

/*** Empty address? */
#define  NULLADDRESS(x)  (NULL_A1(x) && NULL_A2(x) && NULL_A3(x))
#define  NULL_A1(x)       EMPTYCHX(x->name)
#define  NULL_A2(x)       EMPTYCHX(x->id)
#define  NULL_A3(x)       EMPTYCHX(x->host)

/*** Error code returned by choose_name. */
#define  NOPERSON   1

#endif
