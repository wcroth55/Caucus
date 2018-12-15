/*** A_CHG_VAR.   Change the value of a conference or user variable.
/
/    ok = a_chg_var (var, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Var       var;      (object with new variable info)
/       int4      mask;     (mask of properties)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Change the value of a user/conf variable.
/
/    How it works:
/       Caller must fill in var->cnum *or* var->owner (one or the
/       other), var->vname (the variable name), and var->value
/       (the new value of the variable).
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
/    Home:  api/achgvar.c
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

FUNCTION  a_chg_var (Var var, int4 mask, int wait)
{
   int4   pos;
   int    unit, skip, success, error;
   Chix   line, blank;

   if (disk_failure (0, &error, NULL))            return (A_DBERR);

   if (var == (Var) NULL  ||  var->tag != T_VAR)  return (A_BADARG);
   if (EMPTYCHX(var->owner)  &&  var->cnum <= 0)  return (A_BADARG);
   if (EMPTYCHX(var->vname))                      return (A_BADARG);

   unit = (var->cnum > 0 ? XCVA : XUVA);
   if (NOT unit_lock (unit, WRITE, L(var->cnum), L(0), L(0), L(0), var->owner))
      return (unit==XCVA ? A_NOCONF : A_NOPERSON);


   /*** Open temp file to put new version of variable file into. */
   if (NOT unit_lock (XUXT, WRITE, L(0), L(0), L(0), L(0), nullchix)) {
      unit_unlk (unit);
      return (A_DBERR);
   }
   if (NOT unit_make (XUXT)) {
      unit_unlk (unit);
      unit_unlk (XUXT);
      return (A_DBERR);
   }

   line  = chxalloc (L(100), THIN, "a_chg_var line");
   blank = chxalloc ( L(10), THIN, "a_chg_var blank");
   chxcpy (blank, CQ(" "));

   /*** Write the new version of the variable at the top of the file. */
   if (chxvalue (var->value, L(0)) != 0) {
      unit_write (XUXT, var->vname);
      unit_write (XUXT, CQ("\n"));
      for (pos=0;   chxnextline (line, var->value, &pos); ) {
         unit_write (XUXT, blank);
         chxcatval  (line, THIN, L('\n'));
         unit_write (XUXT, line);
      }
   }

   /*** Read the old variable file and write it to XUXT, skipping the
   /    old definition of the selected variable. */
   if (unit_view (unit)) {
      while (unit_read (unit, line, L(0))) {
         if      (chxeq    (line, var->vname))  skip = 1;
         else if (chxvalue (line, L(0)) != ' ' )  skip = 0;

         if (skip)  continue;

         chxcatval  (line, THIN, L('\n'));
         unit_write (XUXT, line);
      }
      unit_close (unit);
   }

   unit_close (XUXT);
   unit_kill  (unit);
   success = unit_move (XUXT, unit);
   unit_unlk  (unit);
   unit_unlk  (XUXT);

   chxfree (line);
   chxfree (blank);
   return  (success ? A_OK : A_DBERR);
}
