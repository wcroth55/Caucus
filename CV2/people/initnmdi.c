
/*** INITNMDIR.   Initialize an empty namelist directory and file.
/
/    ok = initnmdir (ud, uf, cnum, owner, item);
/
/    Parameters:
/       int       ok;        (success?)
/       int       ud, uf;    (namelist file unit numbers)
/       int       cnum;      (conference number)
/       Chix      owner;     (some ud/uf pairs require an owner id)
/       int4      item;      (some namelist files use an item number)
/
/    Purpose:
/       For a given name directory implied by the unit numbers UD, UF, CNUM,
/       owner OWNER, and item number ITEM, write an empty name directory
/       and the corresponding empty namepart file.
/
/    How it works:
/
/    Returns: 1 on success
/             0 on error
/
/    Error Conditions:
/       Can't lock/open/write namelist directory or file.
/  
/    Side effects:
/
/    Related functions:
/
/    Called by: makeconf(), subcreate()...
/
/    Operating system dependencies: None.
/
/    Known bugs:
/
/    Home:  people/initnmdi.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:49 Source code master starting point */
/*: AM  2/07/89 10:31 Jcaucus changes*/
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: DE  4/10/92 15:20 Chixified */
/*: CP  5/06/92 11:58 Add 'item' argument. */
/*: CN  5/16/93 13:27 Add 'cnum' arg to initnmdir(). */

#include <stdio.h>
#include "caucus.h"

extern Chix         sorthigh, sortlow;
extern union null_t null;

FUNCTION  initnmdir (ud, uf, cnum, owner, item)
   int    ud, uf;
   int    cnum;
   int4   item;
   Chix   owner;
{
   struct namlist_t *namdir, *nlnode();
   Chix   buffer;
   int    success;

   ENTRY ("initnmdir", "");

   buffer   = chxalloc (L(42), THIN, "initnmdi buffer");
   success  = 0;

   /*** Create and store the empty namepart directory. */
   if (NOT unit_lock (ud, WRITE, L(cnum), item, L(0), L(0), owner))  FAIL;
   namdir = nlnode(4);
   namdir->num = 2;
   nladd  (namdir, sortlow, 1);
   nladd  (namdir, sorthigh,0);
   stornmdir (ud, namdir);
   nlfree (namdir);
   unit_unlk (ud);


   /*** Now write the one and only "empty" namepart file. */
   if (NOT unit_lock (uf, WRITE, L(cnum), L(1), item, L(0), owner))      
          FAIL;
   if (NOT unit_make (uf))                   { unit_unlk (uf);   FAIL; }
   chxformat(buffer, CQ("%s\n"), L(0), L(0), sortlow, null.chx);
   unit_write (uf, buffer);
   chxclear (buffer);
   chxformat(buffer, CQ("%s\n"), L(0), L(0), sorthigh, null.chx);
   unit_write (uf, buffer);
   unit_close (uf);
   unit_unlk  (uf);

   SUCCEED;

 done:
   chxfree ( buffer );
   RETURN ( success );
}
