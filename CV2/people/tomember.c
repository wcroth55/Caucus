
/*** TOMEMBER.  Convert a (fullname, lastname) pair to a membership string.
/
/    A membership string is used to keep an alphabetized listing of the
/    members of a conference in the XUMD/UMC files.  It has the form:
/      "alphaname#Firstname#Middlenames#Lastname id"   */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: CR  8/12/90 18:21 Change calling sequence. */
/*: DE  4/29/92 14:05 Chixified */
/*: CL 12/10/92 14:52 Long chxalter args. */
/*: CR 11/27/95 16:56 Complete rewrite so double-width blanks are handled. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  tomember (str, last, full, id)
   Chix  str, last, full, id;
{
   Chix   word, pound;
   int4   wi;

   ENTRY  ("tomember", "");

   word = chxalloc (L(60), THIN, "tomember word");

   chxcpy (str, last);
   pound = chxquick ("#",  5);
   for (wi=1;   chxtoken (word, nullchix, wi, full) != -1;   ++wi) {
      chxcat (str, pound);
      chxcat (str, word);
   }
   chxcat (str, CQ(" "));
   chxcat (str, id);

   chxfree (word);
   RETURN  (1);
}
