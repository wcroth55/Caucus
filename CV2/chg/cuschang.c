
/*** CUS_CHANGE.  CUSTOMIZE command, Change option.
/
/    Let the organizer control whether users can change the text
/    of responses that they've entered. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:34 Source code master starting point */
/*: CR  4/28/89 13:06 Check return from unit_lock (XCMA... */
/*: CR  4/08/91 18:48 Add new unit_lock argument. */
/*: CR  6/25/91 15:27 Add 'confnum' arg to srvldmas() calls. */
/*: CR  7/27/91 17:42 Use cus_dattr(). */
/*: CR  7/30/91 13:51 Replace license check with is_distributed(). */
/*: JV 11/13/91 22:53 Add dest_chn arg to cusxattr(). */
/*: DE  3/16/92 14:36 Chixified */
/*: JX  8/03/92 10:07 CHN's are CHix. */
/*: CR  7/13/92 17:45 Add ANSWER argument. */
/*: CI 10/05/92 12:09 Chix Integration. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"

extern struct master_template  master;
extern struct license_t        license;
extern union  null_t           null;
extern Chix                    yesnotable;
extern short                   confnum;
extern Chix                    confname;

FUNCTION  cus_change (answer)
   Chix   answer;
{
   Chix  def;
   short result;
   int   success, error1, error2, got_answer;

   ENTRY ("cus_change", "");

   got_answer = (EMPTYCHX(answer) ? 0 : 3);
   def        = chxalloc (L(20), THIN, "cuschang def");
   chxtoken (def, null.chx, (master.nochange ? 1 : 2), yesnotable);

   result = mdprompter ("chg_Pcuschg", yesnotable, 2 + got_answer,
                        "chg_Hcuschg", 10, answer, def);
   if (result < 0)  FAIL;

   success = cus_dattr (confnum, confname, CUS_CHANGE, result, &error1);
   if (success  &&  is_distributed (confnum))
      cusxattr (confnum, CUS_CHANGE, result, nullchix, &error2);

 done:

   chxfree ( def );
   RETURN  (success);
}
