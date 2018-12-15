/*** A_FRE_TEXT.   Free (deallocate) an Atext object.
/
/    a_fre_text (text);
/
/    Parameters:
/       Atext     text;   (object to be freed)
/
/    Purpose:
/       Deallocate and release an object created by a_mak_text().
/
/    How it works:
/
/    Returns: 1 if proper object type
/             0 if text is null or not an Atext.
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:  a_mak_text()
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/afretext.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/15/92 17:47 New function. */
/*: CR 11/30/92 17:20 Replace Text with Atext. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"

FUNCTION  a_fre_text (Atext text)
{
   if (text == (Atext) NULL)  return (0);

   chxfree (text->data);
   sysfree ( (char *) text);

   return  (1);
}
