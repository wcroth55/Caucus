/*** A_DEL_RESP.   Delete a response or item from a conference.
/
/    ok = a_del_resp (resp, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Resp      resp;     (object selecting response to be deleted)
/       int4      mask;     (P_EVERY or P_ATTACH)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Delete:
/          P_ATTACH: an attachment to a resp or item
/          P_EVERY:  a response (or, if the response number is 0,
/                    an entire item), including any attachment(s)
/
/    How it works:
/       Caller fills in CNUM, INUM, and RNUM.
/
/       A_del_resp calls chg_ditem(), chgxitem(), chg_dresp(), 
/       and chgxresp() as needed to do the dirty work.
/
/       Note that there are 4 cases:                             mask   rnum
/       1) Delete an attachment to a response                  P_ATTACH  >0
/       2) Delete an attachment to an item                     P_ATTACH  =0
/       3) Delete a  response (and its attachment)             P_EVERY   >0
/       4) Delete an item and all its responses (and attach's) P_EVERY   =0
/
/    Returns: A_OK        success
/             A_NOCONF    no such conference
/             A_NOWRITE   user not allowed to write in conf or to item
/             A_NORESP    no such item/resp
/             A_NOTOWNER  user not owner or not organizer
/             A_NOATTACH  caller specified P_ATTACH and
/                         there wasn't any attachment
/             A_DBERR     unknown error
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/adelresp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/04/92 18:08 New function. */
/*: CR 12/06/92 22:12 Make it compile. */
/*: CR  2/22/93 23:31 Add new args to a_cache() calls. */
/*: CR  2/28/93 22:21 Replace global 'tbuf' with a_cch_tbuf call. */
/*: JV  2/26/93 21:27 Add attachments. */
/*: JV  5/25/93 18:31 Add support for item attachments. */
/*: JV  6/01/93 17:30 Clear tbuf cache on exit. */
/*: CP  7/16/93 13:50 Remove unused 'attach'. */
/*: CK  9/13/93 16:18 Delete attach code: DONE only if *just* attachment. */
/*: CR  6/16/95 16:12 Add text properties. */
/*: CR  5/14/04 Use co_priv_str() to check access levels. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"
#include "xaction.h"

extern struct   master_template  master;
extern Textbuf                   tbuf;

FUNCTION  a_del_resp (Resp resp, int4 mask, int wait)
{
   int     access, delitem, chgresp, item, oitem, rnum, inum, success,
           can_delete, ok, error, oresp, chgsubj, b;
   Chix    hdr, getline2();
   Chix    author, userid, itemchn, respchn, date, time;
   Textbuf tbuf, a_cch_tbuf();

   if (disk_failure (0, &error, NULL))                    return (A_DBERR);

   if (resp == (Resp) NULL  ||  resp->tag != T_RESP)      return (A_BADARG);
   if (resp->cnum <= 0)                                   return (A_NOCONF);

   /*** Make sure this user can access this conference. */
   if (NOT a_cache (V_GET, resp->cnum, &access, &delitem, &chgresp,
                           &chgsubj, nullchix))           return (A_NOCONF);
/* if (access < 2)                                        return (A_NOWRITE);*/
   if (access < co_priv_str("include"))                   return (A_NOWRITE);

   /*** Does this response exist? */
   tbuf = a_cch_tbuf (V_GET, XCRD, XCRF, resp->cnum, resp->inum, resp->rnum, 
                             nullchix);
   hdr  = getline2 (XCRD, XCRF, resp->cnum, nullchix, resp->inum, resp->rnum, 
                    0, tbuf);
   if (hdr == nullchix)  return (A_NORESP);

   /*** Can the user delete this response or its attachment?
   /    (Is the user the owner, the copier, or an organizer?) */
   if (access < co_priv_str("instructor")) {
      /*** Owner? */
      author = chxalloc (L(20), THIN, "a_del_re author");
      userid = chxalloc (L(20), THIN, "a_del_re userid");
      sysuserid (userid);
      chxtoken  (author, nullchix, 2, hdr);
      can_delete = equal_id (author, userid, nullchix);

      /*** Copier? */
      b = resp->rnum - tbuf->r_first;
      if (NOT can_delete  &&  b >= 0  &&  resp->rnum <= tbuf->r_last  &&
          resp->rnum != 0) {
         chxtoken (author, nullchix, 1, tbuf->copied[b]);
         can_delete = equal_id (author, userid, nullchix);
      }
      chxfree (author);
      chxfree (userid);
      if (NOT can_delete)  DONE (A_NOTOWNER);
   }

   /*** Handle deleting an attachment.  (Includes cases 1, 2, and 3). */
   if ( ((mask &  P_ATTACH) && resp->rnum >  0)   ||
        ((mask == P_ATTACH) && resp->rnum == 0) ) {
      date     = chxalloc (L(20), THIN, "a_del_re date");
      time     = chxalloc (L(20), THIN, "a_del_re time");
      respchn  = chxalloc (L(20), THIN, "a_del_re orighost");

      /* First get all the info about the response, including the text.
      /  Then we put the resp back without the attachment info, and
      /  finally delete the attachment file. */
      a_get_resp (resp, P_TITLE | P_TEXT | P_FROZEN, A_WAIT);
      free_attach (&resp->attach);
      chxtoken (date, nullchix, 1, resp->date);
      chxtoken (time, nullchix, 2, resp->date);
      if (resp->rnum != 0)  {
         rnum = resp->rnum;
         chg_dresp (XT_CHANGE, resp->cnum, resp->inum, -1, &rnum,
                    nullchix, nullchix, nullchix, resp->text->data,
                    nullchix, nullchix, (Attachment) NULL, 0, nullchix, &error);
      } else {
         inum = resp->inum;
         chg_ditem (XT_CHANGE, resp->cnum, -1, &inum, nullchix,
                    nullchix, nullchix, resp->text->data, nullchix,
                    nullchix, nullchix, (Attachment) NULL, 0, nullchix, &error);
      }

      unit_lock (XARF, WRITE, L(resp->cnum), L(resp->inum), L(resp->rnum),
                 L(0), nullchix);
      unit_kill (XARF);
      unit_unlk (XARF);
      chxfree (date);
      chxfree (time);
      chxfree (respchn);

      /*** If *only* the attachment is being deleted, we're finished. */
      if (mask == P_ATTACH)  DONE (A_OK);
   }

   /*** Delete an entire item (chg_ditem() just calls itemdel(),
   /    which deletes the item, its responses, and any attachments). */
   /*   Case 4. */
   if (resp->rnum == 0) {
      item = resp->inum;

      chg_ditem (XT_DELETE, resp->cnum, -1, &item,
                 nullchix, nullchix, nullchix, nullchix, nullchix,
                 nullchix, nullchix, resp->attach, 0, nullchix, &error);
      tbuf->type = 0;
      if (error != 0)  DONE (A_DBERR);
   }

   else {
      /*** Delete a single response, with or w/o any attachment. */
      /*   Case 3. */
      if (mask == P_EVERY) {
         rnum = resp->rnum;
         chg_dresp (XT_DELETE, resp->cnum, resp->inum, -1, &rnum, nullchix, 
              nullchix, nullchix, nullchix, nullchix, nullchix,
              (Attachment) NULL, 0, nullchix, &error);
         if (tbuf  != (Textbuf) NULL)  tbuf->type = 0;
         if (error != 0)  DONE (A_DBERR);
      }
   }

   resp->ready = 1;
   DONE (A_OK);

done:
   a_cch_tbuf (V_CLR, XCRD, XCRF, resp->cnum, resp->inum, 0, nullchix);
   return  (success);
}
