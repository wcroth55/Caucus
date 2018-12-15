/*** A_EXIT.   Close the Caucus API.
/
/    a_exit();
/
/    Parameters:
/
/    Purpose:
/       "Close" (i.e., finish using) the Caucus API.
/       Cleans up, closes, and deallocates any internal API objects.
/
/       Should be the last api function called before the application
/       exits.
/
/    How it works:
/
/    Returns: 1
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
/    Home:  api/aexit.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/12/92 17:47 New function. */
/*: CR 12/10/92 20:43 Add sysainit() calls. */
/*: CR  2/28/93 21:58 Deallocate cache'd tbufs. */
/*: CK  3/01/93 16:50 Comment out a_cch_master() for now. */
/*: CP  8/01/93 10:37 Add a_debug (A_CLOSE... */
/*: CK  8/02/93 12:32 Make 2nd arg to a_debug() NULL. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "textbuf.h"
#include "entryex.h"

extern Chix     confid, confname;
extern Textbuf  tbuf;

FUNCTION  a_exit()
{
   Textbuf  a_cch_tbuf();

   ENTRY ("a_exit", "");

   last_on (0);
   a_debug (A_CLOSE, (Ageneric) NULL);

   a_cch_tbuf   (V_FREE, 0, 0, 0, 0, 0, nullchix);
/* a_cch_master (V_FREE, 0); */
   free_textbuf (tbuf);   /* allocated by utilglobals(). */

   sysainit (1);

   chxfree (confid);
   chxfree (confname);

   RETURN (1);
}
