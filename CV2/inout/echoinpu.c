
/*** ECHOINPUT.  Echo input prompt and input text on terminal.
/
/    Systems that use the CWIN Caucus windows generally prompt for
/    input on the bottom line of the screen.  In some cases, we may
/    want this input echoed back up on the main screen.  ECHOINPUT
/    is very simple function that echoes the prompt and input text
/    back to the terminal.   */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CW  5/18/89 14:25 Add XW unitcodes. */
/*: CX 10/20/91 18:15 Chixify. */

#include <stdio.h>
#include "caucus.h"

FUNCTION  echoinput (prompt, str)
   Chix   prompt, str;
{
   ENTRY ("echoinput", "");

   unit_write (XKEY, prompt);
   unit_write (XKEY, str);
   unit_write (XKEY, NEWLINE);

   RETURN (1);
}
