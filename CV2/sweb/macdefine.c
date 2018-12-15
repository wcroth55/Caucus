
/*** MAC_DEFINE.   Macro definition.
/
/    mac_define (macs, arg);
/   
/    Parameters:
/       Vartab              macs;    (CML macro list)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Process $mac_define() macro definition.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/macdefine.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/11/98 11:54 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  mac_define (
   Vartab macs,
   Chix   arg)
{
   static Chix name = nullchix;
   static Chix defn;

   if (name == nullchix) {
      name = chxalloc (L( 20), THIN, "mac_define name");
      defn = chxalloc (L(100), THIN, "mac_define defn");
   }

   chxtoken (name, nullchix, 1, arg);
   chxtoken (nullchix, defn, 2, arg);

   vartab_add (macs, name, defn);

   return (1);
}
