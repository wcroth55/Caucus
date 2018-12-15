/*** CHG_TITLE.  Change the title of an item.  Prompt the user
/    for the item number and the new title. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:34 Source code master starting point */
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: CR  9/15/89 17:15 Check thisuser.read_only. */
/*: CR  7/13/90 14:26 Use thisuser.is_organizer. */
/*: CR  4/08/91 18:48 Add new unit_lock argument. */
/*: CR  6/20/91 17:15 Use new equal_id() function. */
/*: CR  7/01/91 17:47 Add chg_dtitle(), chgxtitle() calls. */
/*: CR  7/23/91 15:00 Add 3rd argument to equal_id() call. */
/*: CR  7/30/91 13:51 Replace license check with is_distributed(). */
/*: CR 10/29/91 11:45 Add confnum arg to getline2() call. */
/*: DE  3/16/92 14:20 Chixified */
/*: JX  5/19/92 20:34 Add tbuf arg to getline2() call. */
/*: JV  6/08/93 16:26 Don't need to get old title here. */
/*: CP  9/10/93 10:14 Replace whichitem() with parse_or_ask(). */
/*: CK  9/15/93 14:04 Add 'mod1st' arg to parse_or_ask(). */
/*: CK 10/07/93 17:05 Use &thisitem in parse_or_ask() call. */

#include <stdio.h>
#include "caucus.h"
#include "modifier.h"
#include "api.h"
#define  RLNULL   ( (Rlist) NULL )

extern struct master_template master;
extern Textbuf                tbuf;
extern Userreg                thisuser;
extern struct this_template   thisitem;
extern union  null_t          null;
extern short                  confnum;

FUNCTION  chg_title (command)
   Chix   command;
{
   Chix   str, header, newtitle, ohost, instance;
   Chix   getline2();
   short  am_author;
   char   modifier[MODIFIERS+4];
   int    item, resp, error1, error2, ok, oitem, success;
   Rlist  result;

   ENTRY ("chgtitle", "");

   instance = chxalloc (L(82), THIN, "chgtitle instance");
   str      = chxalloc (L(82), THIN, "chgtitle str");
   ohost    = chxalloc (L(80), THIN, "chgtitle ohost");
   header   = chxalloc (L(80), THIN, "chgtitle header");
   newtitle = chxalloc (L(80), THIN, "chgtitle newtitle");
   result   = RLNULL;
   success = 0;     

   if (thisuser->read_only) {
      mdwrite (XWER, "ent_Ereadonly", null.md);
      FAIL;
   }

   /*** Get the item number, and make sure it's valid. */
   chxtoken  (nullchix, instance, 3, command);
   if (NOT parse_or_ask (instance, &result, modifier, nullchix, confnum, 
                                   &thisitem, 1))  FAIL;
   one_response (&item, &resp, result, &master);
   if (item < 0)  FAIL;

   /*** Make sure this item belongs to the user. */
   chxcpy   (header,   getline2 (XCRD, XCRF, confnum, nullchix, item, 0, 0,
                                (Textbuf) NULL));
   chxtoken (str, nullchix, 2, header);
   am_author = equal_id (str, thisuser->id, nullchix);

   if (NOT am_author  &&  NOT thisuser->is_organizer) {
      mdwrite (XWER, "chg_Tnotyours", null.md);
      FAIL;
   }
   show_header (XWTX, item, nullchix, nullchix, 0);

   /*** If the Organizer is changing someone else's item, get confirmation
   /    that pern really wants to do it. */
   if (NOT am_author  &&  NOT org_confirm()) FAIL;
   
   /*** Get the new title.  <CANCEL> or <RETURN> leaves it alone. */
   if (mdprompter ("chg_Pnewtitle", nullchix, 0, "chg_Hnewtitle", 80,
        newtitle, nullchix) < -1)  FAIL;

   /*** Actually change the title, and log the transaction. */
   ok = chg_dtitle (confnum, item, newtitle, &error1);
   if (ok  &&  is_distributed (confnum)) {
      getoitem  (&oitem, ohost, item, &master);
      chgxtitle (confnum, oitem, ohost, newtitle, &error2);
   }

   SUCCEED;

 done:
   a_fre_rlist (result);
   chxfree (instance); 
   chxfree (str);
   chxfree (ohost);
   chxfree (header);
   chxfree (newtitle);

   RETURN (success);
}
