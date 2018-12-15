
/*** UNIT_REWIND.  Rewind a unit to the beginning.
/
/  n = unit_rewind (unit);
/
/  Parameters:
/     int unit;
/
/  Purpose:
/    Unit_rewind() tells the operating system that the next unit_read()
/    from this unit should start at the beginning of the file.
/
/    Assumes that the caller has already locked and opened the
/    file of type UNIT.
/
/  How it works:
/    On most systems it calls lseek().
/    Other systems just call unit_close() and unit_view().
/    NOTE that on these systems you shouldn't call unit_rewind()
/    if you're writing.
/
/    
/    Returns 1 on success and 0 otherwise. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  6/03/91 16:57 Created function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "handicap.h"
#include "common.h"
#include "systype.h"
#include "units.h"

extern struct unit_template units[];

FUNCTION  unit_rewind (n)
   int  n;
{
   char *bugtell();
#if UNIX | NUT40 | WNT40
   int  stat;

   ENTRY  ("unit_rewind", "");
   if (units[n].access != LOCKED && units[n].access != READ)
      buglist (bugtell ("Unit_rewind: ", n, L(0),"file not locked & open\n"));

#if UNIX | NUT40
   stat = fseek (units[n].fp, L(0), 0);
#endif
#if WNT40
   stat = fseek (units[n].fp, L(0), SEEK_SET);
#endif
   RETURN (stat==0);

#endif

}
