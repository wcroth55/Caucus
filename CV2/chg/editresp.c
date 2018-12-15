/*** EDITRESP.  Allow the user to EDIT a RESPonse.
/
/    Edits response number RESP to ITEM, using the editor specified
/    in EDITOR.  Returns 1 unless editing could not be done for
/    some catastrophic reason, in which case it returns 0. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:34 Source code master starting point */
/*: CW  5/18/89 13:25 Add XW unitcodes. */
/*: CR  8/12/90 20:24 Cast textedit arg to make Lint happy. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR  4/09/91 13:51 Add new confnum argument to updatetext() call. */
/*: CR  6/26/91 17:30 Use new chg_dresp() function. */
/*: CR  7/02/91 17:37 Remove unused vars. */
/*: CR  7/08/91 12:16 Add 3rd arg to chixfile(). */
/*: CR  7/24/91 21:40 Remove unused variables. */
/*: CR  7/27/91 13:58 Replace NULL with null.str. */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: CR  7/30/91 13:51 Replace license check with is_distributed(). */
/*: CR 10/29/91 11:45 Add confnum arg to getline2() call. */
/*: CR 10/29/91 12:50 Add confnum arg to getoresp() call. */
/*: JV 10/31/91 14:35 Add dest_chn arg to chgxresp() call. */
/*: UL 12/11/91 18:41 Make unit_lock args 3-6 int4.*/
/*: DE  3/16/92 18:01 Chixified */
/*: JX  5/19/92 20:51 Fix Chix. */
/*: JX  8/03/92 10:13 CHN's are chix. */
/*: JV  3/03/93 14:55 Add attach arg to chg_dresp(). */
/*: JV  9/10/93 11:51 Set thisitem.rnum for $(thisresp). */
/*: CR  5/06/95 16:43 Use limit_10k to limit # lines of text. */
/*: CR  5/10/95 17:04 Include 'attach' info when changing resp! */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"
#include "attach.h"
#include "api.h"
#include "xaction.h"
#include "derr.h"

extern struct master_template master;
extern Textbuf                tbuf;
extern Userreg                thisuser;
extern struct this_template   thisitem;
extern union  null_t          null;
extern short                  confnum;

FUNCTION  editresp (item, resp, editor)
   int  item, resp;
   Chix editor;
{
   int    i, error1, error2, oitem, oresp, success;
   Chix   p, ctext, itemchn, respchn, getline2();
   Attachment attach = (Attachment) NULL;
   Resp   oldresp = (Resp) NULL;
   int4   pos;

   ENTRY ("editresp", "");

   itemchn   = chxalloc (L(80),  THIN, "editresp itemchn");
   respchn   = chxalloc (L(80),  THIN, "editresp respchn");
   ctext     = chxalloc (L(500), THIN, "editresp ctext");
   success   = 0;

   oldresp = a_mak_resp ("enterresp");
   oldresp->cnum = confnum;
   oldresp->inum = item;
   oldresp->rnum = resp;
   if (a_get_resp (oldresp, P_EVERY, A_WAIT) != A_OK)  FAIL;
   attach = oldresp->attach;

   /*** Write the text of the response out to XITX. */
   unit_lock (XITX, WRITE, L(0), L(0), L(0), L(0), nullchix);
   if (NOT unit_make (XITX)) { unit_unlk (XITX);   FAIL; }

   p = oldresp->text->data;
   for (pos=0L;   chxnextline (ctext, p, &pos);   ) {
      unit_write (XITX, ctext);
      unit_write (XITX, CQ("\n"));
   }
   unit_close(XITX);

   textedit (XITX, editor, (int) thisuser->truncate);

   /*** Load the text from XITX into chix CTEXT... */
   chixfile (XITX, ctext, null.str);
   if (limit_10k (ctext))  mdwrite (XWER, "ced_Elines", null.md);

   /*** Make the change to the Caucus database... */
   thisitem.rnum = resp;
   chg_dresp (XT_CHANGE, confnum, item, -1, &resp, nullchix, nullchix,
          nullchix, ctext, nullchix, nullchix, attach, 0, nullchix, &error1);

   /*** And if distributed, write out the change-response transaction. */
   if (is_distributed (confnum)) {
      getoitem (&oitem, itemchn, item, &master);
      getoresp (&oresp, respchn, confnum, item, resp);
      chgxresp (XT_CHANGE, confnum, oitem, itemchn, oresp, respchn, nullchix,
                nullchix, ctext, nullchix, nullchix, nullchix, 0, &error2);
   }

   unit_unlk (XITX);
 
   SUCCEED;

 done:
   chxfree (itemchn);
   chxfree (respchn); 
   chxfree (ctext);
   a_fre_resp (oldresp);

   RETURN (success);
}
