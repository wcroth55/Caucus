/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** PRINT_ADEF.   Display an Attach_Def linked list.
/
/    print_adef (def);
/
/    Parameters:
/       Attach_Def def;       (linked list)
/
/    Purpose:
/       Display the members of each node of an Attach_Def linked list
/       of file attachment types.
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
/    Home: attach/printade.c
/ */

/*: CP  8/11/93 13:28 New function. */

#include <stdio.h>
#include "handicap.h"
#include "unitcode.h"
#include "attach.h"

static char *type[2] = {"ASC", "BIN"};

FUNCTION  print_adef (def)
   Attach_Def  def;
{
   Attach_Def ptr;

   for (ptr=def;   ptr != NULL;   ptr = ptr->next)  {
      printf ("format='%-20s'  magic=%2d type=%s, ext=%s\n",
          ascquick(ptr->format), ptr->magic_size, type[ptr->data_type], 
          ptr->file_ext);
   }

   return (1);
}
