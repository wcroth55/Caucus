/*** A_GET_VART.   Get a conference, user, or item variable table.
/
/    ok = a_get_vartab (var, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Vartab    var;      (object to fill with variable info)
/       int4      mask;     (mask of properties to get)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Load basic information about a variable table into the VAR object.
/
/    How it works:
/       Caller must fill in one of the following combinations:
/          var->id1   var->id2   var->owner
/               0        0        userid         user variable
/             cnum       0                       conf variable
/             cnum       inum                    item variable
/              -1                                manager list
/              -2                                site variable
/
/       Mask is ignored.
/
/    Returns: A_OK on success
/             A_DBERR on database error
/
/    Error Conditions:
/ 
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/agetvart.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/09/96 18:56 New function. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"

#define  nullint   ( (int *) NULL)

extern int             debug;
extern union  null_t   null;

FUNCTION  a_get_vartab (Vartab var, int4 mask, int wait)
{
   static Chix line = nullchix;
   int    unit, slot, this, start;

   if (var == (Vartab) NULL  ||  var->tag != T_VARTAB)  return (A_BADARG);
   if (EMPTYCHX(var->owner)  &&  var->id1 == 0)         return (A_BADARG);

   if  (line == nullchix)  line = chxalloc (L(100), THIN, "a_get_var line");

   for (slot=0;   slot<var->used;   ++slot)  chxclear (var->name[slot]);

   if      (var->id1 == -1)  unit = XSMG;
   else if (var->id1 == -2)  unit = XSVA;
   else                      unit = (var->id1 > 0 ? XCVA : XUVA);
   if (NOT unit_lock (unit, READ, L(var->id1), L(var->id2), L(0), L(0), var->owner))
      return (unit==XCVA ? A_NOCONF : A_NOPERSON);

   if (NOT unit_view (unit)) {
      unit_unlk (unit);
      return (unit==XCVA ? A_NOCONF : A_NOPERSON);
   }


   for (slot=0;   unit_read (unit, line, L(0));  ) {
      if (chxvalue (line, L(0)) != ' ') {
         this  = vartab_new (var, line, slot++);
         start = 1;
      }
      else {
         if (NOT start)  chxcatval (var->value[this], THIN, L('\n'));
         chxcatsub (var->value[this], line, L(1), ALLCHARS);
         start = 0;
      }
   }

   unit_close (unit);
   unit_unlk  (unit);

   return  (A_OK);
}
