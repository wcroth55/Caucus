
/*** CUS_SUBJECT.  CUSTOMIZE command, Subject option.
/
/    Let the organizer control whether users can change the list
/    of items in a subject. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/02/92 14:48 New function. */
/*: DE  2/26/92 12:47 Chixified */
/*: DE  3/16/92 16:11 Patch a nullchix in for "" */
/*: JX  8/03/92 10:08 CHN's are chix. */
/*: CR  7/13/92 17:47 Add ANSWER argument. */
/*: CI 10/05/92 12:22 Chix Integration. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"

extern struct master_template  master;
extern struct license_t        license;
extern union  null_t           null;
extern Chix                    yesnotable;
extern short                   confnum;
extern Chix                    confname;

FUNCTION  cus_subject (answer)
   Chix   answer;
{
   Chix  def;
   short result;
   int   success, error1, error2, got_answer;

   ENTRY ("cus_subject", "");

   got_answer = (EMPTYCHX(answer) ? 0 : 3);
   def        = chxalloc (L(20), THIN, "cussubj def");
   chxtoken (def, nullchix, (master.chgsubj ? 2 : 1), yesnotable);
   result = mdprompter ("chg_Pcussub", yesnotable, 2 + got_answer,
                        "chg_Hcussub", 10, answer, def);
   if (result < 0)  FAIL;

   success = cus_dattr (confnum, confname, CUS_SUBJECT, result, &error1);
   if (success  &&  is_distributed (confnum))
      cusxattr (confnum, CUS_SUBJECT, result, nullchix, &error2);

   SUCCEED;

done:

   chxfree (def);
   RETURN  (success);
}
