/*** GET_ATTACH_DEF    Find the definition for a particular attachment format.
/
/    def = get_attach_def (format);
/
/    Parameters:
/       Attach_Def def;    // Pointer to the definition, or NULL
/       Chix       format; // Name of the format we're looking for
/
/    Purpose:
/       Look through the linked list.
/
/    How it works:
/
/    Returns: pointer to the defintion, if found
/             NULL if that foramt isn't in the list
/
/    Error Conditions: 
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
/    Home:  attach/getattad.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  3/08/93 09:45 Create this. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern Attach_Def attach_def;

FUNCTION  Attach_Def get_attach_def (format)
   Chix format;
{
   Attach_Def def;

   if (EMPTYCHX (format)) return ((Attach_Def) NULL);

   for (def=attach_def->next; def != (Attach_Def) NULL; def = def->next)
      if (chxeq (def->format, format)) 
         break;

   return (def);
}

