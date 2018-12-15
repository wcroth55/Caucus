
/*** GETIDHOST.   Get userid and hostname out of a response header.
/
/    getidhost (userid, host, resp);
/
/    Parameters:
/       Chix userid;  (where to put userid)
/       Chix host;    (where to put host name)
/       Chix resp;    (line 0 of a response)
/
/    Purpose:
/       Pluck the userid and hostname out of line 0 of a response.
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
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  resp/getidhos.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/19/91 14:26 New function. */
/*: CX 10/17/91 13:05 Chixify. */
/*: CX  6/18/92 16:04 if (chxtoken... >= 0)... */

#include <stdio.h>
#include "caucus.h"

FUNCTION  getidhost (userid, host, resp)
   Chix   userid, host, resp;
{
   Chix   temp;

   ENTRY ("is_author", "");

   chxclear (userid);
   chxclear (host);
   if (resp == nullchix)  RETURN (1);

   temp = chxalloc (L(40), THIN, "getidhost temp");
   if (chxtoken (temp, nullchix, 2, resp) >= 0) 
       chxbreak (temp, userid, host, CQ("@"));

   chxfree (temp);
   RETURN  (1);
}
