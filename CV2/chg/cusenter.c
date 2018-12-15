
/*** CUS_ENTER.  CUSTOMIZE command, ENTER option.
/
/    Let the organizer control whether users can enter new
/    items all by themselves. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:34 Source code master starting point */
/*: CR  4/28/89 13:06 Check return from unit_lock (XCMA... */
/*: CR  4/08/91 18:48 Add new unit_lock argument. */
/*: CR  6/25/91 15:27 Add 'confnum' arg to srvldmas() calls. */
/*: CR  7/27/91 17:44 Use cus_dattr(). */
/*: CR  7/30/91 13:51 Replace license check with is_distributed(). */
/*: JV 11/13/91 22:55 Add dest_chn arg to cusxattr(). */
/*: DE  3/16/92 16:03 Chixified */
/*: JX  8/03/92 10:08 CHN's are chix. */
/*: CR  7/13/92 17:46 Add ANSWER argument. */
/*: CI 10/05/92 12:16 Chix Integration. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"

extern struct master_template  master;
extern struct license_t        license;
extern union  null_t           null;
extern Chix                    yesnotable;
extern short                   confnum;
extern Chix                    confname;

FUNCTION  cus_enter (answer)
   Chix   answer;
{
   Chix   def;
   short  result;
   int    success, error1, error2, got_answer;

   ENTRY ("cus_enter", "");

   got_answer = (EMPTYCHX(answer) ? 0 : 3);
   def      = chxalloc (L(20), THIN, "cusenter def");
   chxtoken (def, null.chx, (master.noenter ? 1 : 2), yesnotable);
   result = mdprompter ("chg_Pcusent", yesnotable, 2 + got_answer,
                        "chg_Hcusent", 10, answer, def);
   if (result < 0)  FAIL;

   success = cus_dattr (confnum, confname, CUS_ADD, result, &error1);
   if (success  &&  is_distributed (confnum))
      cusxattr (confnum, CUS_ADD, result, nullchix, &error2);

 done:

   chxfree ( def );
   RETURN  (success);
}
