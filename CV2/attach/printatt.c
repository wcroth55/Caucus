/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** PRINT_ATTACH.   Display an Attachment data structure.
/
/    print_attach (attach);
/
/    Parameters:
/       Attachment   attach;   (attachment to be displayed)
/
/    Purpose:
/       Debugging: display the members of an attachment. 
/
/    How it works:
/
/    Returns: 1
/
/    Error Conditions: 
/
/    Side effects:
/
/    Related Functions: 
/
/    Called by: UI
/
/    Operating System Dependencies:
/
/    Known Bugs: none
/
/    Home: attach/printatt.c
/ */

/*: CP  8/11/93 13:28 New function. */

#include <stdio.h>
#include "handicap.h"
#include "unitcode.h"
#include "attach.h"

FUNCTION  print_attach (attach)
   Attachment  attach;
{
   printf ("db_name='%s', ", ascquick(attach->db_name));
   printf ("name='%s', size=%d, rec_len=%d\n", 
            ascquick(attach->name), attach->size, attach->rec_len);
}
