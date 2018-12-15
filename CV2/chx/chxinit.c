/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** CHX_INIT.  Set up internal text structures, types, and methods.
/
/    chx_init (type)
/
/  Parameters: 
/       int type;     // Type of character set we're using.
/
/  Purpose:
/     CHX_init defines and sets up various data structures that the
/     chx functions need.  These should be things that need to exist
/     before *any* strings are processed.
/
/  How it works:
/     TYPE must be one of:  (defined in TEXT.H).
/        0: ASCII
/        1: JIX
/        2: ISO8859
/
/  Returns: 1.
/
/  Error Conditions: none.
/
/  Side Effects:
/     Sets default collating order.
/     Defines which characters are "blanks" (word separators).
/
/  Related to:
/
/  Called by:  main functions.
/
/  Home: chx/chxinit.c
*/

/*: JX 07/13/92 10:47 Create function. */
/*: JX 10/09/92 17:42 Fixify. */
/*: CR 12/11/92 18:22 Replace ALLCHARS with LASTVALUE. */
/*: CR 12/18/92 13:47 Make LASTVALUE larger than any 16 bit value. */
/*: JV  9/08/93 10:22 Add ENTRY. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "text.h"

#define  LASTVALUE  100000L

FUNCTION chx_init (type)
int type;
{
   Chix blanks;
   char *bugtell();

   ENTRY ("chx_init", "");

   blanks = chxalloc (8L, THIN, "chxinit blanks");

   /*** Set initial collation rules before any arguments. */
   chxcollate (CLEAR_CT, 0L, 0L, 0L, 0L);
   chxcollate (ADDRULE_CT, L('!'), L('!'), 0L, 0L);
   chxcollate (ADDRULE_CT, L('~'), L('~'), LASTVALUE, 0L);

   /*** Define word separators. */
   switch (type) {
      case JIX:
         chxcatval  (blanks, THIN,    32L);
         chxcatval  (blanks, WIDE, 41377L);    /* Kanji DW space. */
         chxcatval  (blanks, THIN,    10L);
         chxcatval  (blanks, THIN,     9L);
         chxspecial (SC_SET, SC_BLANK, blanks);
         chxfree    (blanks);
         break;
      case ASCII:
      case ISO8859:
         chxcatval  (blanks, THIN, 32L);
         chxcatval  (blanks, THIN, 10L);
         chxcatval  (blanks, THIN,  9L);
         chxspecial (SC_SET, SC_BLANK, blanks);
         break;
      default:
         buglist (bugtell ("CHX_INIT:", 0, 0L, "Bad language type.\n"));
         break;
   }

   RETURN (1);
}
