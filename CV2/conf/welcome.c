/*** WELCOME.  Display the welcoming message. 
/
/  welcome (out)
/
/  Parameters:
/    int    out;
/
/  Purpose:
/    This is the (usually short!) message the user sees everytime he/she
/    signs on to this conference.  
/
/  How it works:
/
/  Returns: 1.
/
/  Error Conditions:
/
/  Side Effects:
/
/  Related to:
/
/  Called by:  interpret()
/
/  Home: conf/welcome.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: CR  1/30/89 12:38 Return if user hit <CANCEL>. */
/*: CW  5/18/89 14:25 Add XW unitcodes. */
/*: CR 11/03/89 11:41 Load XCGR file into memory, *then* display it. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: CR  1/03/92 15:05 Add OUT argument. */
/*: JX  3/23/92 12:45 Chixify, add header. */
/*: DE  5/26/92 12:36 Fix constant type in chxalloc */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"

extern short confnum;

FUNCTION  welcome (out)
   int    out;
{
   Chix  text;

   ENTRY ("welcome", "");

   text = chxalloc (L(200), THIN, "welcome line");

   unit_lock (XCGR, READ, L(confnum), L(0), L(0), L(0), nullchix);

   chixfile (XCGR, text, "");

   sysbrkclear();
  
   unit_unlk (XCGR);

   sysbrkclear();

   unit_write (out, text);
   chxfree (text);

   RETURN (1);
}
