/*** OK_TO_SEND_CONF.   OK to send conference CNUM to host TO_CHN?
/
/    ok = ok_to_send_conf (cnum, to_chn);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    cnum;        (local number of conference to be sent)
/       Chix   to_chn;      (CHN of requesting host)
/
/    Purpose:
/       Determine if it is valid to send conference CNUM to newly
/       subscribing remote host TO_CHN.
/
/    How it works:
/       Check the userlist file for conference CNUM to see if host
/       TO_CHN is included.
/
/    Returns: 1 on success
/            -1 if conference CNUM does not exist
/            -2 if TO_CHN is not allowed to subscribe to CNUM
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  XFU
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  xaction/oktosend.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/21/91 17:29 New function. */
/*: DE  3/25/92 14:53 Chixified */

#include <stdio.h>
#include "caucus.h"

FUNCTION  ok_to_send_conf (cnum, to_chn)
   int    cnum;
   Chix   to_chn;
{
   Namelist  groups, nlnode();
   Chix      atchn;
   int       access, success;

   ENTRY ("ok_to_send_conf", "");

   atchn     = chxalloc (L(100), THIN, "oktosend atchn");
   success   = 0;

   /*** Does this conference exist, and can TO_CHN subscribe to it? */
   chxconc (atchn, CQ("@"), to_chn);
   groups = nlnode (1);
   access = allowed_on (atchn, groups, cnum);
   nlfree (groups);
   if (access <  0)  DONE(-1);
   if (access == 0)  DONE(-2);

   SUCCEED;

 done:
   chxfree (atchn);

   RETURN (success);
}
