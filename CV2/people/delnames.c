
/*** DELNAMES.   Delete a namelist file set.
/
/    ok = delnames (ud, uf, cnum, item);
/
/    Parameters:
/       int       ok;        (success?)
/       int       ud, uf;    (namelist file unit numbers)
/       int       cnum;      (conference number)
/       int4      item;      (item number, if relevant)
/
/    Purpose:
/       Delete an entire set of namelist files, as specified by
/       UD, UF, CNUM, and ITEM.
/
/    How it works:
/
/    Returns: 1 on success
/             0 on error
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  itemdel()
/
/    Operating system dependencies: None.
/
/    Known bugs:
/
/    Home:  people/delnames.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  5/11/93  9:39 New function. */

#include <stdio.h>
#include "caucus.h"

extern union  null_t    null;

FUNCTION  delnames (ud, uf, cnum, item)
   int    ud, uf, cnum;
   int4   item;
{
   Namelist namdir, this, nlnode();
   int      success;

   ENTRY ("delnames", "");

   /*** Get the relevant namepart directory. */
   nlclear();
   if (NOT unit_lock (ud, WRITE, L(cnum), item, L(0), L(0), nullchix))  RETURN(0);
   namdir   = nlnode(4);
   if (NOT loadnmdir (ud, namdir))  FAIL;

   /*** Loop through the namdir, and delete each partfile. */
   for (this=namdir->next;   this!=null.nl;   this=this->next) {
      if (unit_lock (uf, WRITE, L(cnum), L(this->num), item, L(0), nullchix)) {
         unit_kill  (uf);
         unit_unlk  (uf);
      }
   }

   unit_kill (ud);
   SUCCEED;

 done:
   unit_unlk (ud);
   nlfree    (namdir);
   
   RETURN ( success );
}
