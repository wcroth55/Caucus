/*** CLEAR_ATTACH    Clear an Attachment object.
/
/    clear_attach (attachment);
/
/    Parameters:
/       Attachment      attachment;    (object to clear)
/
/    Purpose:
/       Initialize an Attachment object.
/
/    How it works:
/
/    Returns: 1 on success
/             0 if object didn't exist
/
/    Error Conditions: 
/  
/    Side effects: 
/
/    Related functions:  make_attach()  free_attach() 
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  attach/clearatt.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  2/24/93 15:45 Create this. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

FUNCTION  clear_attach (attach)
   Attachment attach;
{
   if (attach == (Attachment) NULL)  return (0);

   chxclear (attach->db_name);
   chxclear (attach->name);
   attach->format  = (Attach_Def) NULL;
   attach->size    = L(0);
   attach->rec_len = 0;

   return (1);
}

