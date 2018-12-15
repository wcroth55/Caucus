
/*** MAKE_ADDRESS.   Create an empty "Address" data object.
/
/    addr = make_address();
/
/    Parameters:
/       Address    addr;        (newly created Address object)
/
/    Purpose:
/       Create a new, empty "Address" data object.
/       When the caller is finished with the object, it must
/       be freed with free_address().
/
/    How it works:
/
/    Returns: Address object
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions: free_address(), into_address()
/
/    Called by:  Caucus (addmess()...)
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  people/makeaddr.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/11/91 13:15 New function. */
/*: CX 11/11/91 23:53 New role for this function. */
/*: CR  1/02/92 10:06 Trim leading & trailing blanks from name and host. */
/*: JX  5/15/92 16:52 Chixify, rm sysmem decl. */
/*: JX  5/15/92 15:10 Always return an Address, never (1)! */
/*: CX  6/01/92 11:27 Split into make_address, into_address, free_address. */
/*: CL 12/11/92 15:52 Long sysmem arg. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  Address  make_address()
{
   Address  addr;
   char    *sysmem();

   ENTRY ("make_address", "");

   addr = (Address) sysmem (L(sizeof (struct address_t)), "addr");
   addr->name = chxalloc (L(80), THIN, "make_address name");
   addr->id   = chxalloc (L(80), THIN, "make_address id");
   addr->host = chxalloc (L(80), THIN, "make_address host");

   RETURN (addr);
}
