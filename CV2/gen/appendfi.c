/*** APPENDFILE.   Append one file to another.
/
/  int n = append_file (u1, u2, u3)
/
/  Parameters:
/    int   u1, u2, u3  (units for destination, 1st source, 2nd source)
/
/  Purpose:
/    append_file() appends the contents of U3 to U2 and leaves the
/    leaves the result in U1.  Append_file assumes that all three
/    units have been locked.
/
/  How it works:
/    If U1 and U2 are the same, append_file() reads from U3 and appends
/    to U2.  Otherwise it first reads from U2 and writes to U1, then
/    reads from U3 and writes to U1.
/
/    If (U1 == U2) and U3 doesn't exist, append_file just returns 1.
/    If (U1 != U2) and U2 or U3 doesn't exist, append_file copies the file
/    that does exist to U1 and returns 1..
/    If (U1 != U2) and U2 and U3 don't exist, append_file creates U1 and 
/    returns 1.
/
/ ***The caller must abide by these rules:
/    1) If two of the unit numbers are the same, they must be U1 and U2.
/    2) Any subset of (U1, U2, U3) can exist, including the null set.
/    3) The caller is responsible for locking and unlocking the files.
/    4) If (U1 == U2) the caller must lock that unit for WRITE.
/    5) The caller does not need to lock files that don't exist.
/
/  Returns:
/    1 on success
/    0 on failure.  In this case it logs the error in the bugfile.
/
/  Error Conditions:
/
/  Related Functions:
/    copyfile()
/
/  Called by: xp_ship()
/
/  Home: gen/appendfi.c
/  
/  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  1/20/92 12:07 New function. */
/*: DE  3/30/92 15:59 Chixified */
/*: DE  6/02/92 11:17 Added maxlen arg to unit_read */

#include "handicap.h"
#include "unitcode.h"
#include "chixuse.h"
#include "done.h"

FUNCTION  append_file (u1, u2, u3)
   int  u1, u2, u3;
{
   Chix   line;
   int    success;

   ENTRY ("append_file", "");

   line       = chxalloc (L(250), THIN, "appendfi line");
   success    = 0;

   if (u1 == u2) {
      if (NOT unit_append (u1)) {
         bug ("APPEND_FILE: couldn't open destination.", u1); FAIL; }
      if (NOT unit_view (u3)) {
         unit_close (u1);
         bug ("APPEND_FILE: couldn't open source (3).", u3); FAIL; }
      while (unit_read (u3, line, L(0)))  {
         unit_write (u1, line);
         unit_write (u1, CQ("\n"));
      }
      unit_close (u1);
      unit_close (u3);

   } else {
      if (NOT unit_make (u1)) {
         unit_unlk (u1);
         bug ("APPEND_FILE: couldn't open destination.", u1); FAIL; }
      if (NOT unit_view (u2)) {
         unit_close (u1);
         bug ("APPEND_FILE: couldn't open source (2).", u2); FAIL; }
      if (NOT unit_view (u3)) {
         unit_close (u1);
         unit_close (u2);
         bug ("APPEND_FILE: couldn't open source (3).", u3); FAIL; }
      while (unit_read (u2, line, L(0)))  {
         unit_write (u1, line);
         unit_write (u1, CQ("\n"));
      }
      while (unit_read (u3, line, L(0)))  {
         unit_write (u1, line);
         unit_write (u1, CQ("\n"));
      }
      unit_close (u1);
      unit_close (u2);
      unit_close (u3);
   }

   SUCCEED;

 done:
   chxfree (line);

   RETURN (success);
}
