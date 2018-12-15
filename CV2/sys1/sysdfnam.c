/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SYSDFNAME.   Difference - Name.  
/
/    SYSDFNAME takes a user name and makes it "different", so that a
/    person entering Caucus via the Caucus login shell will have a 
/    slightly different name than anyone who entered Caucus from the
/    operating system. */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

FUNCTION sysdfname (name)
   Chix  name;
{

   ENTRY ("sysdfname", "");

   chxcat (name, CQ("_"));

   RETURN (1);
}
