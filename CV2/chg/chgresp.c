
/*** CHG_RESPONSE.  Change the text of a set of responses.
/
/    ok = chg_response (red, range);
/
/    Parameters:
/       int       ok;       (success?)
/       Redirect *red;      (redirection information, source of input)
/       Chix      range;    (range of items/resp's selected on command line)
/
/    Purpose:
/       Handling for Caucus CLI: interpret user request to change one
/       or more items or responses in a given range.
/
/    How it works:
/       Calls parse_or_ask() to parse RANGE into an Rlist.  For each
/       entry in the Rlist:
/          (a) If the RESP modifier was not found, *and* the Rlist entry
/              includes response 0, change the item text.
/
/          (b) Otherwise, offer the chance to change each response
/              (excluding response 0) in the entry.
/ 
/       If RED->UNIT is other than XKEY, automatically use the
/       contents of that file to REPLACE the
/       first (and only the first) selected response or item.  If more
/       than one item or response is selected, the remainder will be
/       treated as though no file redirection was supplied.
/
/    Returns: 1 on success
/             0 if user cannot change *any* items
/             0 if no RANGE was specified
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions: chg_rloop(), chg_item()
/
/    Called by:  change()
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  chg/chgresp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:33 Source code master starting point */
/*: CR 11/09/88 13:05 Handle <CANCEL> while changing another's response. */
/*: JV  8/22/89 18:50 Tell user that there is no item. */
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: CR  9/15/89 17:14 Check thisuser->read_only. */
/*: CR 10/11/89 16:11 Revert to "no items selected" if parserange fails. */
/*: CR  7/13/90 14:24 Use thisuser->is_organizer. */
/*: CX 11/10/91 18:37 Chixify. */
/*: CR  1/20/92 16:43 Add cnum arg to load_header() call. */
/*: CI 10/05/92 11:54 Chix Integration. */
/*: JV  4/02/93 11:22 Add attach arg to load_header(). */
/*: JV  6/08/93 16:17 Don't need to get header or title here. */
/*: CN  5/15/93 15:20 Replace parse_range with new_parse(). */
/*: CT  5/28/93 13:47 Replace SELECT with Rlist. */
/*: CP  6/25/93 13:54 Add new args to new_parse() call. */
/*: CP  6/27/93 18:46 Use new chg_rloop() call. */
/*: CP  7/14/93 15:24 Integrate 2.5 changes. */
/*: CP  8/09/93 11:41 Add 'attach' arg to new_parse() call. */
/*: CK  8/13/93 17:28 Begin attachment work. */
/*: CP  8/16/93 13:57 Finish attachment handling. */
/*: JV  9/13/93 16:18 Don't show_header if input from file. */
/*: CP  9/09/93 17:06 Replace new_parse() with parse_or_ask(). */
/*: CK  9/15/93 14:04 Add 'mod1st' arg to parse_or_ask(). */
/*: CK 10/07/93 17:05 Use &thisitem in parse_or_ask() call. */
/*: CK 10/14/93 14:24 modifier[M_ITEMS] -> (! modifier[M_RESPONSES]). */
/*: CP 11/13/93 15:22 Apply r_bound() to response range. */
/*: CP 12/10/93 21:31 Add 'final' arg to r_bound() call. */
/*: CP 12/15/93 16:08 Add all new args to r_bound() call. */

#include <stdio.h>
#include "caucus.h"
#include "modifier.h"
#include "api.h"
#include "attach.h"
#include "redirect.h"
#define   RLNULL   ( (Rlist) NULL )

extern struct master_template master;
extern struct this_template   thisitem;
extern Userreg                thisuser;
extern Textbuf                tbuf;
extern union  null_t          null;
extern short                  confnum;

FUNCTION  chg_response (red, range)
   Redirect *red;
   Chix      range;
{
   Chix   title, attach, errformat, str;
   short  i;
   char   modifier[MODIFIERS+5];
   char   errstr[40];
   int    success, unit, error, item, resp, first, last, final;
   int4   arg;
   Rlist  result, rptr;

   ENTRY ("chgresp", "");

   if (thisuser->read_only) {
      mdwrite (XWER, "ent_Ereadonly", null.md);
      RETURN  (0);
   }

   if (NOT thisuser->is_organizer  &&  master.nochange) {
      mdwrite (XWER, "chg_Tnochgr", null.md);
      RETURN  (0);
   }

   title   = chxalloc (L(80), THIN, "chgresp title");
   attach  = chxalloc (L(80), THIN, "chgresp attach");
   str     = chxalloc (L(80), THIN, "chgresp str");
   result  = RLNULL;
   success = 0;

   /*** Parse the RANGE, and then invoke 'chg_rloop' for each selected item. */
   if (NOT parse_or_ask (range, &result, modifier, attach, confnum,
                                &thisitem, 1))  FAIL;

   /*** Handle changing an attachment to an existing response.  The real
   /    work is done by hook_attachment(). */
   if (modifier[M_ATTACH]) {

      /*** Use the first item:resp instance given. */
      if (NOT one_response (&item, &resp, result, &master)) {
         mdwrite (XWER, "res_Tnoitem", null.md);
         FAIL;
      }

      /*** Add the attachment to that item:response. */
      if (hook_attachment (confnum, item, resp, attach, red, 0, &error))
         SUCCEED;
  
      /*** If the attachment failed, explain why. */
      if (error < 0)  { sprintf (errstr, "add_Eat%d", -error);  arg = item; }
      else            { strcpy  (errstr, "add_Eat0");           arg = error;}
      sprintf (errstr, "add_Eat%d",  (error < 0 ? -error : 0));
      errformat = chxalloc (L(200), THIN, "addresp errformat");
      if (NOT mdstr (errformat, errstr, null.md))
         chxofascii (errformat, "\nUnknown attachment error!\n");
      chxclear   (str);
      chxformat  (str, errformat, arg, L(resp), attach, nullchix);
      unit_write (XWER, str);
      chxfree    (errformat);
      FAIL;
   }

   apply_redirect (red);
   unit = red->unit;
   for (rptr=result->next;   rptr!=RLNULL;   rptr=rptr->next) {
      for (i=rptr->i0;   i<=rptr->i1;   ++i, unit=XKEY) {
         if (i == 0)   continue;
         sysbrkclear();

         /*** Change just the item? */
         if ( (NOT modifier[M_RESPONSES])  &&  rptr->r0 == 0) {
            chg_item (unit, i);
            continue;
         }
  
         /*** Change a set of responses. */
         if (NOT r_bound (&first, &last, &final, rptr, confnum, i,
                           master.responses[i]))   continue;
         if (NOT load_header (confnum, i, range, title, (Attachment) NULL))
                                                   continue;
         if (unit == XKEY)  show_header (XWTX, i, range, title, 1);
         if (sysbrktest())  break;
         chg_rloop (i, first, last, 0, unit);
      }
   }
   SUCCEED;

done:
   a_fre_rlist (result);
   chxfree (title);
   chxfree (attach);
   chxfree (str);
   RETURN  (success);
}
