/*** A_MAK_TEXT.   Make (allocate) an Atext object.
/
/    text = a_mak_text();
/
/    Parameters:
/       Atext   text;    (returned/created object)
/
/    Purpose:
/       Allocate and initialize a Atext object.
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
/    Related functions:  a_fre_text()
/
/    Called by:  api functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/amaktext.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/15/92 17:47 New function. */
/*: CR 11/30/92 17:20 Replace Text with Atext. */
/*: CP  8/01/93 10:22 Change header comments. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

FUNCTION  Atext  a_mak_text()
{
   Atext  text;

   text = (Atext) sysmem (sizeof (struct Atext_t), "Atext_t");
   if (text == (Atext) NULL)  return (text);

   text->data  = chxalloc (L(500), THIN, "a_mak_te data");
   text->lines = 0;

   return (text);
}
