/*** A_MAK_RESP.   Make (allocate) a Resp object.
/
/    resp = a_mak_resp (name);
/
/    Parameters:
/       Resp   resp   (returned/created object)
/       char  *name;  (name for debugging purposes)
/
/    Purpose:
/       Allocate and initialize an Resp object.
/
/    How it works:
/
/    Returns: newly created object
/             NULL on error
/
/    Error Conditions: insufficient heap memory, or heap catastrophe.
/  
/    Side effects: 
/
/    Related functions:  a_fre_resp()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/amakresp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/03/92 14:26 New function. */
/*: JV  2/24/93 17:06 Add ATTACH. */
/*: CP  8/01/93  9:37 Add 'name' argument, debugging stuff. */
/*: CR  6/16/95 16:12 Add text properties. */
/*: CR  4/14/00 16:07 Add exists and lastcall members. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

extern int api_debug;

FUNCTION  Resp  a_mak_resp (char *name)
{
   Resp  resp;

   resp = (Resp) sysmem (sizeof (struct Resp_t), "Resp_t");
   if (resp == (Resp) NULL)  return (resp);

   resp->tag    = T_RESP;
   A_SET_NAME (resp->dname, name);
   resp->cnum   =  0;
   resp->inum   = -1;
   resp->rnum   = -1;
   resp->text   = a_mak_text();
   resp->frozen =  0;
   resp->title  = chxalloc (L(20), THIN, "a_mak_re title");
   resp->author = chxalloc (L(20), THIN, "a_mak_re author");
   resp->date   = chxalloc (L(20), THIN, "a_mak_re date");
   resp->owner  = chxalloc (L(20), THIN, "a_mak_re owner");
   resp->copied = chxalloc (L( 0), THIN, "a_mak_re copied");
   resp->textprop = 0;
   resp->ready    = 0;
   resp->attach   = make_attach();
   resp->exists   = 0;
   resp->lastcall = systime();

   if (api_debug) a_debug (A_ALLOC, (Ageneric) resp);

   return (resp);
}
