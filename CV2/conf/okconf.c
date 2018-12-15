
/*** OKCONF.  Is this a legal conference name?
/
/    ok = okconf (name);
/  
/    Parameters:
/       int   ok;     (ok name?)
/       Chix  name;   (name of new conference)
/  
/    Purpose:
/      Is this an OK name for a conference?
/  
/    How it works:
/  
/    Returns: 1 if conference name is ok
/             0 if no name
/            -1 name too long
/            -2 does not begin with a letter
/            -3 characters outside of [a-z][0-9]_.-
/            -4 reserved words: STOP, HELP, LIST
/  
/    Error Conditions:
/  
/    Side Effects:
/  
/    Related to:
/  
/    Home: conf/okconf.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/17/97 17:02 New function, based on old oknewconf */
/*: CR 08/25/07 Rule out + and & . */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "chixuse.h"

#define  MAX_CONFNAME  80

extern union null_t null;

FUNCTION  int okconf (Chix name)
{
   int    good, v, len, i;

   good = 1;

   /*** Is the name too long, or empty? */
   if (EMPTYCHX (name))                           return (0);
   len = chxlen (name);
   if (len > MAX_CONFNAME)                        return (-1);

   /*** Complain if any bad chars: non-printable, space, ' / \ *   */
   for (i=0;   i<len;   i++) {
      v = chxvalue (name, L(i));
      if (v < 040  ||  v > 0176 ||  v=='\''  ||  v=='/'   ||  v=='\\'  ||  
          v=='*'   ||  v == '&' ||  v=='+'   ||  v == ' '  )  { good = 0;  break; }
   }

   if (NOT good)                                  return (-3);

   return (1);
}
