/*** FREE_ATTACH     Free (deallocate) an Attachment object.
/    FREE_DEF_ATTACH Free (deallocate) an Attach_Def object.
/    FREE_MAP_ATTACH Free (deallocate) an Attach_Mapping object.
/
/    free_attach     (attach);
/    free_def_attach (attach_def);
/    free_map_attach (attach_map);
/
/    Parameters:
/       Attachment    *attach;      (object to be freed)
/       Attach_Def     attach_def;  (object to be freed)
/       Attach_Mapping attach_map;  (object to be freed)
/
/    Purpose:
/       Deallocate and release an object created by make_attach(),
/       make_def_attach(), or make_map_attach().
/
/    How it works:
/       Free_def_attach() frees only the one object handed to it.
/       It does *not* free the linked list of descendant objects.
/
/       Free_map_attach() frees only the one object handed to it.
/       It does *not* free the linked list of descendant objects.
/
/    Returns: 1 if proper object type
/             0 if attach is null
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:  make_attach()   clear_attach()
/                        make_def_attach()
/                        make_map_attach()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  attach/freeatta.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  2/24/93 13:49 Create this. */
/*: CP  8/01/93 13:42 Free_attach must free FORMAT; fix free_def_at loop. */
/*: CK  8/12/93 16:20 I was wrong, should NOT free FORMAT.  *Sigh*. */
/*: CP  8/18/93 17:32 Change Attach_Mapping_t.ui_type to Chix. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

FUNCTION  free_attach (attach)
   Attachment  *attach;
{

   if ( attach == (Attachment *) NULL ||
       *attach == (Attachment)   NULL)  return (0);

   chxfree ((*attach)->db_name);
   chxfree ((*attach)->name);


   /*** Finally, free the attachment itself, and mark it NULL. */
   sysfree ( (char *) *attach);
   *attach = (Attachment) NULL;

   return  (1);
}


FUNCTION  free_def_attach (attach_def)
   Attach_Def  attach_def;
{
   Attach_Mapping map, next;

   if (attach_def == (Attach_Def) NULL)  return (0);

   chxfree (attach_def->format);

   /* Free all maps associated with this format */
   for (map = attach_def->maps;       map != (Attach_Mapping) NULL;  ) {
      next = map->next;
      free_map_attach (map);              /* Free node */
      map  = next;
   }
   sysfree ( (char *) attach_def);

   return (1);
}

FUNCTION  free_map_attach (attach_map)
   Attach_Mapping  attach_map;
{
   if (attach_map == (Attach_Mapping) NULL)  return (0);

   chxfree (attach_map->command);
   chxfree (attach_map->ui_type);
   sysfree ( (char *) attach_map);

   return (1);
}
