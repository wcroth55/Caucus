/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** LASTNAME.   Get the user's Last Name.
/
/    Get the word of the user's name that we'll use for alphabetizing
/    the user.  In the U.S., this is typically the "last name", although
/    different conventions are used in different countries. */

/*: AA  7/29/88 17:49 Source code master starting point */
/*: AM  2/07/89 10:31 Jcaucus changes*/
/*: CR  4/03/89 17:20 Integrate, correct to use LAST ascii word. */
/*: CR  4/10/89  9:27 Still more jcaucus integration. */
/*: CW  5/18/89 15:36 Add XW unit codes. */
/*: DE  4/13/92 11:48 Chixified */
/*: JX  5/27/92 17:24 Convert stralpha & remove flag.japan. */
/*: JX  7/13/92 10:54 Assume sort by first name for Japan. */

#include "caucus.h"
#include <stdio.h>

extern union  null_t        null;
extern struct flag_template flag;

FUNCTION  lastname  (who)
   Userreg  who;
{
   Chix    word, lastword;
   short   i, c;
   int     success;

   ENTRY ("lastname", "");

   word      = chxalloc (L(60), THIN, "lastname word");
   lastword  = chxalloc (L(60), THIN, "lastname lastword");
   success   = 0;

   /*** Get the last (ascii) word from who->name into LASTWORD. */
   chxclear (lastword);

   if (flag.japan) chxtoken (lastword, null.chx, 1, who->name);
   else 
      for (i=1;   chxtoken (word, null.chx, i, who->name) != -1;   ++i)
         chxcpy (lastword, word);

   jixreduce (lastword);
   if  (i==2)  { chxcpy (who->lastname, lastword);   SUCCEED; }

   mdwrite (XWTX, "reg_Tlastnm", null.md);

   do {
      c = mdprompter ("reg_Tlistme", null.chx, 0, "reg_Hlistme", 40, 
                         who->lastname, lastword);
      jixreduce (who->lastname);
   } while (c >= -2 && EMPTYCHX (who->lastname));

   DONE(c >= -2);

 done:
   chxfree (word);
   chxfree (lastword);

   RETURN (success);
}
