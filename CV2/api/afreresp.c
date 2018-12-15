/*** A_FRE_RESP.   Free (deallocate) a Resp object.
/
/    a_fre_resp (resp);
/
/    Parameters:
/       Resp     resp;   (object to be freed)
/
/    Purpose:
/       Deallocate and release an object created by a_mak_resp().
/
/    How it works:
/
/    Returns: 1 if proper object type
/             0 if resp is null or not an Resp.
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:  a_mak_resp()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/afreresp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/03/92 14:32 New function. */
/*: CR 12/04/92 15:42 Remove unused vars. */
/*: JV  2/24/93 17:06 Add ATTACH. */
/*: CP  8/01/93  9:37 Add debugging stuff. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  a_fre_resp (Resp resp)
{

   if (resp == (Resp) NULL)  return (0);
   if (resp->tag != T_RESP)  return (0);

   if (api_debug) a_debug (A_FREE, (Ageneric) resp);
   resp->tag = 0;

   a_fre_text (resp->text);
   chxfree (resp->title);
   chxfree (resp->author);
   chxfree (resp->date);
   chxfree (resp->owner);
   chxfree (resp->copied);
   if (resp->attach != (Attachment) NULL) free_attach (&resp->attach);

   sysfree ( (char *) resp);

   return (1);
}
