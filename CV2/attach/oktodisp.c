/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** OK_TO_DISPLAY_ATTACH.  Is it reasonable to display this attachment
/    on this terminal?  (or, more accurately: "Do we have a mapping for
/    this attachment format to this terminal type?"
/
/    map = ok_to_display_attach (attach);
/
/    Parameters:
/       Attach_Mapping   map;     (returned terminal type mapping)
/       Attachment       attach;  (attachment)
/
/    Purpose:
/       So the UI can decide to go ahead and attempt to display the
/       attachment, or tell the user that this won't work well.
/
/    How it works:
/
/    Returns: pointer to the first mapping which will display the attach
/             NULL, if none
/
/    Error Conditions: attachment map doesn't exist or is corrupted
/
/    Side effects:
/
/    Related Functions: parse_attach_map()
/                       analyze_file()
/
/    Called by: UI
/
/    Operating System Dependencies:
/
/    Known Bugs: none
/
/    Home: attach/oktodisp.c
/
/ */

/*: JV  2/25/93 10:26 Create this. */
/*: CP  8/18/93 17:32 Change Attach_Mapping_t.ui_type to Chix. */
/*: CP  8/19/93 12:54 Change header comments to match function return value! */
/*: CP  8/25/93  0:16 Remove debugging printf's. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "attach.h"

extern Chix att_ui_type;

FUNCTION Attach_Mapping ok_to_display_attach (attach)
Attachment attach;
{
   Attach_Mapping map;

   for (map=attach->format->maps;   map != NULL;   map = map->next) {
      if (chxeq (map->ui_type, att_ui_type)  &&
          map->map_type & MAP_DISP)            return (map);
   }

   return ((Attach_Mapping) NULL);
}
