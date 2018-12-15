/*** CALCSTATUS.  Calculate one-line status information. 
/
/  calcstatus (newitems, newresp, actitems, newmess)
/
/  Parameters:
/     int *newitems     (number of new items)
/         *newresp      (number of new responses (total))
/         *actitems     (number of "active" items (items with new responses))
/         *newmess      (number of new responses)
/
/  Purpose:
/
/  How it works:
/
/  Returns:
/
/  Error Conditions:
/
/  Side Effects:
/
/  Related to:
/
/  Called by: 
/
/  Home: conf/calcstat.c
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/20/90 15:51 New function. */
/*: JX  3/04/92 12:05 Add header. */

#include <stdio.h>
#include "caucus.h"

extern struct master_template  master;
extern struct message_t        mess;
extern struct partic_t         thisconf;

FUNCTION  calcstatus (newitems, newresp, actitems, newmess)
   int   *newitems, *newresp, *actitems, *newmess;
{
   int    i;

   ENTRY ("calcstatus", "");

   /*** Calculate number of new items. */
   for (*newitems=0, i=thisconf.items+1;   i<=master.items;     ++i)
      if (master.responses[i] >= 0)  ++(*newitems);

   for (*newresp = *actitems = 0, i=1;     i<=thisconf.items;   ++i)
      if (thisconf.responses[i] >= 0  && 
          thisconf.responses[i] <  master.responses[i]) {
         ++(*actitems);
         *newresp += (master.responses[i] - thisconf.responses[i]);
      }

   /*** Count the number of new messages. */
   for (i=1, *newmess=0;   i<=mess.highest;   ++i)
      if (mess.num[i] & MSG_NEW)  ++(*newmess);

   RETURN (1);
}
