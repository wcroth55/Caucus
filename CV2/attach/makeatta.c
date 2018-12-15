/*** MAKE_ATTACH      Make (allocate) an Attachment object.
/    MAKE_DEF_ATTACH  Make (allocate) an Attach_Def object.
/    MAKE_MAP_ATTACH  Make (allocate) an Attach_Mapping object.
/
/    attachment = make_attach();
/    attach_def = make_def_attach();
/    attach_map = make_map_attach();
/
/    Parameters:
/       Attachment      attachment;    (returned/created object)
/       Attach_Def      attach_def;    (returned/created object)
/       Attach_Mapping  attach_map;    (returned/created object)
/
/    Purpose:
/       Allocate and initialize an Attachment object, Attach_Def object,
/       or Attach_Mapping object.
/
/    How it works:
/       All code which uses attachments should make the following
/       assumption: an "empty" attachment is signified by a SIZE of zero.
/
/    Returns: newly created object
/             NULL on error
/
/    Error Conditions: insufficient heap memory, or heap catastrophe.
/  
/    Side effects: 
/
/    Related functions:  free_attach()    clear_attach()
/                        free_def_attach()
/                        free_map_attach()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  attach/makeatta.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  2/24/93 13:37 Create this. */
/*: CP  8/18/93 17:32 Change Attach_Mapping_t.ui_type to Chix. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

FUNCTION  Attachment  make_attach()
{
   Attachment attach;
   char      *sysmem();

   attach = (Attachment) sysmem (sizeof (struct Attachment_t), "make_attach");
   if (attach == (Attachment) NULL)  return (attach);

   attach->db_name  = chxalloc (L(80), THIN, "makeatta dbname");
   attach->name     = chxalloc (L(40), THIN, "makeatta name");
   attach->format   = (Attach_Def) NULL;
   attach->size     = L(0);
   attach->rec_len  = 0;

   return (attach);
}

FUNCTION  Attach_Def  make_def_attach()
{
   Attach_Def def_attach;
   char      *sysmem();

   def_attach = (Attach_Def) sysmem (sizeof (struct Attach_Def_t), "attach_df");
   if (def_attach == (Attach_Def) NULL)  return (def_attach);

   def_attach->format      = chxalloc (L(16), THIN, "makeatdef format");
   def_attach->magic_size  = 0;
   def_attach->magic[0]    = '\0';
   def_attach->data_type   = 0;
   def_attach->file_ext[0] = '\0';
   def_attach->maps        = make_map_attach();     /* Head of linked-list */
   def_attach->next        = (Attach_Def)     NULL;
   
   return (def_attach);
}

FUNCTION  Attach_Mapping  make_map_attach()
{
   Attach_Mapping attach_map;
   char      *sysmem();

   attach_map = (Attach_Mapping) 
                sysmem (sizeof (struct Attach_Mapping_t), "amap");
   if (attach_map == (Attach_Mapping) NULL)  return (attach_map);

   attach_map->map_type  = 0;
   attach_map->ui_type   = chxalloc (L(80), THIN, "makeatmap ui_type");
   attach_map->command   = chxalloc (L(80), THIN, "makeatmap command");
   attach_map->next      = (Attach_Mapping) NULL;

   return (attach_map);
}
