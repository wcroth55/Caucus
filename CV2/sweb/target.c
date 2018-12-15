
/*** TARGET.   Make (allocate) a Target object. 
/
/    t = target (Hose cd, Chix po);
/   
/    Parameters:
/       Target  t;      (created Target)
/       Hose    cd;     (an output hose, or BADHOSE)
/       Chix    po;     (an output Chix, or nullchix)
/
/    Purpose:
/       Create an output Target.
/       A wide variety of caucus parsing functions need to be passed
/       a place to send the output.  Previous to version 4.23, this
/       was always a Hose; this has been replaced with a Target, which
/       can be either a Hose or a Chix.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/target.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/09/01 14:50 Add output Target, to replace 'cd'. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

static struct target_t failsafe;

FUNCTION Target target (Hose cd, Chix po) {
   Target t;
   char  *sysmem();

   t = (Target) sysmem (sizeof (struct target_t), "target");
   if (t == (Target) NULL)  t = &failsafe;

   t->cd = cd;
   t->po = po;

   return (t);
}
