/*** TABLEFIND.  Find WORD in TABLE.
/
/    TABLEFIND searches through TABLE to see if WORD is identical to
/    one of the tokens.  It returns:
/      -1  if it didn't find WORD
/       n  if WORD was identical to token N (0, 1, 2...) in TABLE. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  5/27/92 15:03 Move from str to glyph. */
/*: CR 12/02/92 14:35 Use ()'s in success=-1. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
 
FUNCTION  tablefind (table, word) 
   Chix  table, word;
{ 
   short  i;
   int    success;
   Chix   token;
 
   ENTRY ("tablefind", "");

   token = chxalloc (L(16), THIN, "tablefind token");

   if (table==nullchix  ||  word==nullchix  ||
       EMPTYCHX(table)   ||  EMPTYCHX(word))   { success=(-1); goto done; }

   for (i=1;   chxtoken (token, nullchix, i, table) != -1;   ++i)
      if (chxeq (word, token))  { success =  i-1; goto done; }

   success = -1;
done:
   chxfree (token);
   RETURN (success);
}
