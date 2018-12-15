/*** A_CHG_VARTAB.   Write a new conference or user variable table.
/
/    ok = a_chg_vartab (var, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Vartab    var;      (object with new variable info)
/       int4      mask;     (mask of properties)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Change the value of a user/conf variable table.
/
/    How it works:
/       Caller must fill in one of the following combinations:
/          var->id1   var->id2   var->owner
/               0        0        userid         user variable
/             cnum       0                       conf variable
/             cnum       inum                    item variable
/              -1                                manager list
/              -2                                site variable
/       and the names and values of the variables.
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
/    Home:  api/achgvart.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/09/96 20:39 New function. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"

#define  nullint   ( (int *) NULL)

extern int             debug;
extern union  null_t   null;

FUNCTION  a_chg_vartab (Vartab var, int4 mask, int wait)
{
   static Chix blank = nullchix;
   static Chix nl, line;
   int4        pos;
   int         unit, slot, error;

   if (disk_failure (0, &error, NULL))                  return (A_DBERR);

   if (var == (Vartab) NULL  ||  var->tag != T_VARTAB)  return (A_BADARG);
   if (EMPTYCHX(var->owner)  &&  var->id1 == 0)         return (A_BADARG);

   if (blank == nullchix) {
      blank = chxalsub (CQ(" " ), L(0), 1);
      nl    = chxalsub (CQ("\n"), L(0), 1);
      line  = chxalloc (L(200), THIN, "a_chg_vartab line");
   }

   if      (var->id1 == -1)  unit = XSMG;
   else if (var->id1 == -2)  unit = XSVA;
   else                      unit = (var->id1 > 0 ? XCVA : XUVA);
   if (NOT unit_lock (unit, WRITE, L(var->id1), L(var->id2), 
                                  L(0), L(0), var->owner))
      return (unit==XCVA ? A_NOCONF : A_NOPERSON);

   if (NOT unit_make (unit)) {
      unit_unlk (unit);
      return (A_DBERR);
   }

   
   for (slot=0;   slot < var->used;   ++slot) {
      if (chxvalue (var->name [slot], L(0)) == 0)  continue;
      if (chxvalue (var->value[slot], L(0)) == 0)  continue;

      unit_write (unit, var->name[slot]);
      unit_write (unit, nl);

      for (pos=0;   chxnextline (line, var->value[slot], &pos); ) {
         unit_write (unit, blank);
         chxcatval  (line, THIN, L('\n'));
         unit_write (unit, line);
      }
   }

   unit_close (unit);
   unit_unlk  (unit);

   return  (A_OK);
}

FUNCTION  vartab_print (char *text, Vartab vt)
{
   int    slot;

   fprintf (stderr, "\n===%s===\n", text);
   for (slot=0;   slot < vt->used;   ++slot) {
      if (chxvalue (vt->name[slot], L(0)) == 0)  continue;

      fprintf (stderr, "%d: '%s'\n", slot,      ascquick(vt->name [slot]));
      fprintf (stderr, "-------------\n%s\n\n", ascquick(vt->value[slot]));
   }
}
