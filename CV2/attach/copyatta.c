/*** COPY_ATTACH      Copy an Attachment object.
/
/    ok = copy_attach(dest, source)
/
/    Parameters:
/       Attachment      dest;    destination of copy
/       Attachment      source;  sourc of copy
/
/    Purpose:
/       Copy info about an attachment from one structure to another.
/
/    How it works:
/
/    Returns: 1 on success
/             0 on failure
/
/    Error Conditions: insufficient heap memory, or heap catastrophe.
/  
/    Side effects: 
/
/    Related functions:  
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  attach/copyatta.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  3/03/93 13:37 Create this. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

FUNCTION    copy_attach (dest, source)
Attachment dest, source;
{
   if (dest == (Attachment) NULL) 
      if ((dest = make_attach()) == (Attachment) NULL) return (0);

   chxcpy (dest->db_name, source->db_name);
   chxcpy (dest->name, source->name);
   dest->format  = source->format;
   dest->size    = source->size;
   dest->rec_len = source->rec_len;

   return (1);
}

