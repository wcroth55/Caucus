
/*** urlEncode (inputChix, outputChix);
/   
/    Purpose:
/       URL-encode all necessary characters.
/ */

/*** Copyright (C) 2010 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/08/10 New function. */

#include <stdio.h>
#include <ctype.h>
#include "chixuse.h"
#include "sweb.h"

FUNCTION  void urlEncode (Chix inChix, Chix outChix) {
   int pos, val, width;
   char temp[20];

   chxclear (outChix);
   width = chxtype (inChix);
   for (pos=0;   (val = chxvalue (inChix, pos)) > 0;   ++pos) {
      if (isalnum(val) || val == '-' || val == '_' || val == '.' || val == '~') {
         chxcatval (outChix, width, val);
      }
      else {
         sprintf (temp, "%c%02x", '%', val);
         chxcat (outChix, CQ(temp));
      }
   }

   return;
}
