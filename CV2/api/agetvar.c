/*** A_GET_VAR.   Get a conference or user variable.
/
/    ok = a_get_var (var, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Var       var;      (object to fill with variable info)
/       int4      mask;     (mask of properties to get)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Load basic information about a variable into the VAR object.
/
/    How it works:
/       Caller must fill in var->cnum *or* var->owner (one or the
/       other), and var->vname (the variable name).
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
/    Home:  api/agetvar.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/06/95 12:36 New function. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"

#define  nullint   ( (int *) NULL)

extern int             debug;
extern union  null_t   null;

FUNCTION  a_get_var (Var var, int4 mask, int wait)
{
   int    unit, found;
   Chix   line;

   if (var == (Var) NULL  ||  var->tag != T_VAR)  return (A_BADARG);
   chxclear (var->value);
   if (EMPTYCHX(var->owner)  &&  var->cnum <= 0)  return (A_BADARG);
   if (EMPTYCHX(var->vname))                      return (A_BADARG);

   unit = (var->cnum > 0 ? XCVA : XUVA);
   if (NOT unit_lock (unit, READ, L(var->cnum), L(0), L(0), L(0), var->owner))
      return (unit==XCVA ? A_NOCONF : A_NOPERSON);

   if (NOT unit_view (unit)) {
      unit_unlk (unit);
      return (unit==XCVA ? A_NOCONF : A_NOPERSON);
   }

   line = chxalloc (L(100), THIN, "a_get_var line");

   for (found=0;   unit_read (unit, line, L(0));  ) {
      if (NOT found) {
         if (chxeq (line, var->vname))  found = 1;
      }
      else {
         if (chxvalue (line, L(0)) != ' ')   break;
         if (found == 2)   chxcatval (var->value, THIN, L('\n'));
         chxcatsub (var->value, line, L(1), ALLCHARS);
         found = 2;
      }
   }

   unit_close (unit);
   unit_unlk  (unit);

   chxfree (line);
   return  (A_OK);
}
