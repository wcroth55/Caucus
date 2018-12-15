
/*** FIX_DOLLAR.  Handle special case of "\$" in CML lines.
/
/    fix_dollar (a, encode);
/   
/    Parameters:
/       Chix   a;      (line of CML text)
/       int    encode; (encode or decode?)
/
/    Purpose:
/       fix_dollar() handles the special case of "\$" in CML text.
/
/       Since the "$" usually introduces a CML function, there needs
/       to be a way to "escape" it so as to actually get a "$", by
/       itself, in the final text.
/
/       The sequence "\$" is used to produce such an escaped "$".
/       But this sequence needs to be recognized (and encoded) in many 
/       places during the CML parsing, and only output (i.e., decoded)
/       just before the final result is output.
/
/    How it works:
/       If ENCODE is 1, A is scanned for "\$", and each instance
/       is translated into a hidden code.
/
/       If ENCODE is 0, A is scanned for the hidden code, and
/       each instance is replaced by the original "\$".
/
/       If ENCODE is -1, A is scanned for the hidden code, and
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  fixdollar.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/11/96 15:39 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  void fix_dollar (Chix a, int encode)
{
   static Chix backdol = nullchix;
   static Chix alpha, dollar;
   int         found;

/*
   if (backdol == nullchix) {
      backdol = chxalsub (CQ("\\$"),  L(0), L(4));
      alpha   = chxalsub (CQ("\003"), L(0), L(4));
      dollar  = chxalsub (CQ("$"),    L(0), L(4));
   }

   if (encode == 1) {
      found = chxreplace (a, L(0), backdol, alpha);
   }
   else if (encode == -1) {
      found = chxreplace (a, L(0), alpha, backdol);
   }
   else {
      found = chxreplace (a, L(0), alpha, dollar); 
   }
*/

   return;
}
