
/*** ENTERTITLE.  Get the TITLE for an item from the user.
/    Returns 1 if a title is entered, and 0 if the user wants to
/    cancel the item. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:51 Source code master starting point */
/*: CX 10/17/91  0:13 Chixify. */

#include <stdio.h>
#include "caucus.h"

extern Chix yesnotable;

FUNCTION  entertitle (title)
   Chix   title;
{
   int    choice, success;
   Chix   answer, no;
   
   ENTRY ("entertitle", "");

   no     = chxalloc (L(20), THIN, "entertitle no");
   answer = chxalloc (L(20), THIN, "entertitle answer");

   chxtoken (no, nullchix, 1, yesnotable);

   success = 1;
   do {
      choice = mdprompter ("ent_Ptitle", nullchix, 0, "ent_Htitle", 
                                         80, title, nullchix);

      if (choice <= -3  &&  mdprompter ("ent_Ptitqui", yesnotable, 2,
                                        "ent_Htitqui", 20, answer, no) != 0) 
         { success = 0;   break; }

   } while (choice < -1);

   chxfree (no);
   chxfree (answer);
   RETURN  (success);
}
