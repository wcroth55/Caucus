
/*** CHANGE.  Handle all of the various things that may be changed by
/    the user: responses, items, titles, names, introductions,
/    phone numbers. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:33 Source code master starting point */
/*: CR 09/27/88 15:13 Lowercase COMMAND before processing it! */
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: CR 12/02/89 18:34 Display chg_Hnoarg on XWTX, not XWER. */
/*: CX 11/10/91 16:31 Chixify. */
/*: JX  5/29/92 16:30 chxreduce->jixreduce. */
/*: CR  8/07/92 12:47 Allow CHANGE INTRO|ITEM <file. */
/*: CI 10/04/92 23:27 Chix integration. */
/*: CP  6/27/93 20:08 Use chg_resp() for items *and* responses. */
/*: CP  8/16/93 13:49 Use new RED redirection. */

#include <stdio.h>
#include "caucus.h"
#include "redirect.h"

extern union null_t null;

FUNCTION  change (command)
   Chix   command;
{
   static   Chix  changetable = nullchix;
   Chix     what, restofline;
   short    found;
   Redirect red;

   ENTRY ("change", "");

   parse_rin (&red, command, XIRE);
   jixreduce (command);
   mdtable   ("chg_Atab", &changetable);

   what       = chxalloc (L(100), THIN, "change what");
   restofline = chxalloc (L(100), THIN, "change restofline");

   if (chxtoken (what, nullchix, 2, command) == -1) {
      mdwrite (XWTX, "chg_Hnoarg", null.md);
      goto done;
   }
   chxtoken (nullchix, restofline, 3, command);

   found = tablematch (changetable, what);
   switch (found) {

      case (-2):  ambiguous (what);           break;

      case (-1):  /* unknown, assume items. */
      case ( 0):  /* items. */
      case ( 1):  /* responses. */
         chxtoken (nullchix, restofline, 2, command);
         chg_response (&red, restofline);
         break;

      case ( 2):  chg_title (command);        break;
      case ( 3):  chg_name  ();               break;
      case ( 4):  chg_intro (&red);           break;
      case ( 5):  chg_phone();                break;
      case ( 6):  chg_pass();                 break;
      case ( 7):  chg_subj (restofline, 0);   break;  /* SUBJECT */
      case ( 8):  chg_subj (restofline, 1);   break;  /* MYSUBJECT */
   }

done:
   if (red.locked)  unit_unlk (red.unit);
   chxfree (restofline);
   chxfree (what);
   RETURN  (1);
}
