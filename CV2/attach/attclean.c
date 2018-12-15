/*** ATT_CLEANUP_DISPLAY    Cleanup after displaying an Attachment object.
/
/    att_cleanup_display (u, name);
/
/    Parameters:
/       int  u;
/       Chix name;
/
/    Purpose:
/       After displaying a file attachment, there may be a file or other
/       detritus left around.  Here's where we sweep it all under the rug.
/
/    How it works:
/
/    Returns: 1 
/
/    Error Conditions: 
/  
/    Side effects: 
/
/    Related functions:  a_get_resp()  (makes the mess)
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  attach/attclean.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  3/10/93 17:45 Create this. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "unitcode.h"

FUNCTION  att_cleanup_display (u, name)
   int  u;
   Chix name;
{
   if (unit_lock (u, WRITE, L(0), L(0), L(0), L(0), name)) {
      unit_kill (u);
      unit_unlk (u);
   }

   return (1);
}

