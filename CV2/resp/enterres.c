/*** ENTERRESP.  Enter a response from unit IN as response RNUM.
/
/    ok = enterresp (item, rnum, in);
/
/    Parameters:
/       int   ok;      (success)
/       int   item;    (add resp to this item)
/       int   rnum;    (add as this resp #, 0 => new resp)
/       int   in;      (unit to read text from)
/                          
/    Purpose:
/       Read text from unit IN, and enter it as response RNUM to ITEM.
/       If RNUM is 0, make this a new response.
/
/    How it works:
/       The caller is responsible for locking/unlocking unit IN.
/
/    Returns: 1 on success.
/             0 on error, or cancellation by user.
/
/    Side effects:      none
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies: none
/
/    Known bugs:      none
/
/    Home:  resp/enterres.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:51 Source code master starting point */
/*: CR  4/28/89 12:52 Check unit_lock status for failure. */
/*: CR  8/09/89 17:33 Unlock input file only if XITX. */
/*: CW  5/18/89 15:43 Add XW unit codes. */
/*: CR  3/04/90 22:39 Add CONTEXT argument, use it in textenter() call. */
/*: CR  5/09/90 16:28 Freeze an item with nearly MAXRESP responses. */
/*: CR  8/12/90 20:21 Cast textenter arg to make Lint happy. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  4/09/91 13:51 Add new confnum argument to updatetext() call. */
/*: CR  6/25/91 15:27 Add 'confnum' arg to srvldmas() calls. */
/*: CR  6/26/91 14:30 Add 'confnum' argument to itemfreeze(). */
/*: CR  6/26/91 15:39 Use new chg_dresp() to do most of the work. */
/*: CR  7/08/91 12:16 Add 3rd arg to chixfile(). */
/*: CR  7/10/91 14:38 Fix call to chgxresp() to use original item #. */
/*: CR  7/27/91 13:17 Use null.str instead of NULL. */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: CR  7/30/91 13:51 Replace license check with is_distributed(). */
/*: CR 10/04/91 17:47 XT_NEW or XT_CHANGE depends on rnum. */
/*: CX 10/15/91 19:31 Chixify, clean up header comments. */
/*: CR 10/29/91 12:49 Add confnum arg to getoresp() call. */
/*: JV 10/31/91 15:20 Add dest_chn arg to chgxresp() call. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: DE  4/20/92 14:12 Fixed missing & mistyped args, also day/time */
/*: CX  6/05/92 16:22 XKEY --> u_keybd. */
/*: JX  6/24/92 13:41 Don't write over day/time fields, don't use chxquick. */
/*: JX  8/09/92 16:38 Fixify. */
/*: TP 10/19/92 14:01 Replace u_keybd with input_control(). */
/*: CR 10/22/92 12:40 Only display ent_T... prompt if session input. */
/*: CR  1/12/93 17:54 Add chxwrap() call to wrap over-int4 lines. */
/*: JV  3/03/93 14:57 Add attach arg to chg_dresp(). */
/*: JV  9/10/93 11:53 Set thisitem.rnum for $(thisresp). */
/*: CK  9/15/93 15:37 Move prompting user for text to get_resp_text(). */
/*: JV  9/21/93 16:31 Use old attachment info if available. */
/*: CR  3/08/94 18:52 a_fre_resp (oldresp) only if not NULL! */
/*: CR  5/06/95 16:43 Use limit_10k to limit # lines of text. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"
#include "xaction.h"
#include "derr.h"
#include "api.h"

extern struct master_template  master;
extern struct this_template    thisitem;
extern union  null_t           null;
extern short                   confnum;
extern Userreg                 thisuser;

FUNCTION  enterresp (item, rnum, in)
   int    item, rnum, in;
{
   Chix   ctext, date, time, itemchn, respchn;
   int    error1, error2, oitem, action, oresp;
   int4   systime();
   Attachment attach = (Attachment) NULL;
   Resp   oldresp = (Resp) NULL;

   ENTRY ("enterresp", "");

   ctext = chxalloc (L(500), THIN, "enterres ctext");
   date  = chxalloc ( L(30), THIN, "enterres date");
   time  = chxalloc ( L(30), THIN, "enterres time");
  
   sysdaytime  (date, time, 0, systime());

   /*** Load the text of the item into the chix CTEXT. */
   chixfile (in, ctext, "");
   chxwrap  (ctext, 77);
   if (limit_10k (ctext))  mdwrite (XWER, "ced_Elines", null.md);

   if (rnum) {
      oldresp = a_mak_resp ("enterresp");
      oldresp->cnum = confnum;
      oldresp->inum = item;
      oldresp->rnum = rnum;
      if (a_get_resp (oldresp, P_TITLE, A_WAIT) == A_OK)
         attach = oldresp->attach;
   }

   /*** Add the response. */
   action  = (rnum==0 ? XT_NEW : XT_CHANGE);
   chg_dresp (action, confnum, item, -1, &rnum, nullchix, thisuser->name,
              thisuser->id, ctext, date, time, attach, 0, nullchix, &error1);
   thisitem.rnum = rnum;

   if (is_distributed (confnum)) {
      itemchn = chxalloc (L(40), THIN, "enterres itemchn");
      respchn = chxalloc (L(40), THIN, "enterres respchn");
      getoitem (&oitem, itemchn, item, &master);
      getoresp (&oresp, respchn, confnum, item, rnum);
      chgxresp (action, confnum, oitem, itemchn, oresp, respchn,
                thisuser->name, thisuser->id, ctext, date, time,
                nullchix, 0, &error2);
      chxfree  (itemchn);
      chxfree  (respchn);
   }

   chxfree (ctext);
   chxfree (date);
   chxfree (time);

   if (error1 == DINOITEM)  mdwrite (XWTX, "res_Tdelitem", null.md);

   if (oldresp != (Resp) NULL) a_fre_resp (oldresp);
   RETURN (error1 == NOERR);
}
