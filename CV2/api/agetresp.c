/*** A_GET_RESP.   Fetch information about an item/response.
/
/    ok = a_get_resp (resp, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Resp      resp;     (object to fill with response info)
/       int4      mask;     (mask of properties to get)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Fetch information (text, title, frozen status) about an
/       item or responses.
/
/    How it works:
/       Caller must fill in elements resp->cnum, resp->inum, resp->rnum.
/       MASK must be some OR of P_TITLE, P_TEXT, P_FROZEN, or the caller
/       can specify P_ATTACH.  Since P_ATTACH copies the file out to user
/       space (usually to /tmp), don't use P_ATTACH in a loop.
/       P_TITLE also gets attachment info, but does not copy the file to
/       temporary space.
/
/       a_get_resp() does not fail if P_ATTACH is specified and there's no
/       attachment.
/
/    Returns: A_OK on success
/             A_BADARG   bad arguments
/             A_NOCONF   no such conference
/             A_NORESP   no such response
/             A_NOATTACH no such attachment
/             A_NOREAD   user not allowed in this conf
/             A_DBERR    database error
/
/    Error Conditions:
/  
/    Side effects: 
/       For now, we use the global Textbuf tbuf.  We may eventually want
/       to replace this with a locally allocated tbuf; or share one
/       across the a_???_resp() functions.
/
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/agetresp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/03/92 14:44 New function. */
/*: CR 12/03/92 23:58 If a_cache() fails, assume A_NOCONF. */
/*: CR 12/04/92 14:51 Simplify title fetching. */
/*: CR 12/07/92 10:45 Use L() in chxcatval() calls. */
/*: CL 12/11/92 15:09 Long zero. */
/*: CR  1/03/93 23:28 Add "fast" access to item titles. */
/*: CR  2/22/93 23:31 Add new args to a_cache() calls. */
/*: CR  2/28/93 21:51 Replace global 'tbuf' with a_cch_tbuf call. */
/*: CK  3/01/93 14:30 Correct a_cch_tbuf() call. */
/*: JV  3/08/93 09:04 Add File Attachments. */
/*: JV  6/08/93 18:18 Add text length info. */
/*: CP  8/19/93 12:26 In attach code, use attach->name, not attach->db_name. */
/*: CK  9/07/93 15:24 Use SLOT (rnum-r_first) instead of rnum! */
/*: CR  6/16/95 16:12 Add text properties. */
/*: CR  4/13/00 16:08 Set exists and lastcall members. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"

#define GETLINE(r,l)  getline2 (XCRD, XCRF, r->cnum, nullchix, r->inum, \
                               r->rnum, l, tbuf)

extern Chix     att_copydir;
extern Attach_Def attach_def;

FUNCTION  a_get_resp (Resp resp, int4 mask, int wait)
{
   int     access, additem, chgresp, fast, chgsubj, lnum, slot;
   int4    zero;
   Chix    header, line, title, getline2();
   Chix    fast_head, fast_title;
   Chix    frozen, temp;
   Textbuf tbuf, a_cch_tbuf();
   Attachment attach;

   if (resp == (Resp) NULL  ||  resp->tag != T_RESP)  return (A_BADARG);

   resp->lastcall  = systime();
   resp->exists    = 0;
   if (resp->cnum <= 0)                               return (A_NORESP);

   /*** Make sure this user can read this conference. */
   if (NOT a_cache (V_GET, resp->cnum, &access, &additem, &chgresp,
                           &chgsubj, nullchix))   return (A_NOCONF);
   if (access < 1)                                return (A_NOREAD);

   /*** If we're just getting the title or frozen property of an item, we
   /    can use "fast" access. */
   tbuf = (Textbuf) NULL;
   fast = (resp->rnum == 0  &&  NOT (mask & (P_TEXT | P_ATTACH)));

   /*** "Fast" access means read the first few lines of the partfile directly.*/
   if (fast) {
      fast_head  = chxalloc (L(80), THIN, "a_get_re fast_head");
      fast_title = chxalloc (L(80), THIN, "a_get_re fast_title");
      attach = make_attach();
      if (NOT load_header (resp->cnum, resp->inum, fast_head, fast_title,
          attach)) {
         chxfree (fast_head);
         chxfree (fast_title);
         free_attach (&attach);
         return  (A_NORESP);
      }
      header = fast_head;
      title  = fast_title;
      slot   = 0;
   }

   else {
      tbuf = a_cch_tbuf (V_GET, XCRD, XCRF, resp->cnum, resp->inum, 
                                resp->rnum, nullchix);
      if ( (header = GETLINE (resp, 0)) == nullchix)  return (A_NORESP);
      lnum = 1;
      if (resp->rnum == 0) {
         title = GETLINE (resp, 1);
         lnum  = 2;
      }

      /*** SLOT is the entry in TBUF corresponding to response # RNUM. */
      slot = resp->rnum - tbuf->r_first;
   }

   /*** At this point we're sure the response exists (although not
   /    all requested data, such as attachments, may exist). */
   resp->exists = 1;

   /*** Get File Attachment, if selected and available. */
   if ((mask & P_ATTACH) && (mask != P_EVERY)) {

      if (tbuf->attach[slot]       == (Attachment) NULL  ||
          tbuf->attach[slot]->size == 0)
         return (A_NOATTACH);

      /*** Copy over file attachment info. */
      if (resp->attach == (Attachment) NULL) resp->attach = make_attach();
      copy_attach (resp->attach, tbuf->attach[slot]);

      /* Does it even exist? */
      if (resp->attach->size == 0) return (A_NOATTACH);

      /*** Copy file attachment to user space. */
      systmpfile (att_copydir, resp->attach->name);
      unit_lock  (XIRE, WRITE, L(0), L(0), L(0), L(0), resp->attach->name);
      unit_lock  (XARF, READ,  L(resp->cnum), L(resp->inum), L(resp->rnum),
                              L(0), nullchix);
      unit_b_copy (XARF, XIRE); 
      unit_unlk (XARF); 
      unit_unlk (XIRE);
   }
  
   resp->frozen = 0;
   chxclear (resp->author);
   chxclear (resp->date);
   chxclear (resp->owner);

   /*** Pluck out the owner, frozen, date/time, author fields from the
   /    header line. */
   temp   = chxalloc (L(20), THIN, "a_get_re temp");
   frozen = chxalloc (L(20), THIN, "a_get_re frozen");
   chxtoken  (resp->owner, nullchix,  2, header);
   chxtoken  (frozen,      nullchix,  3, header);
   chxtoken  (resp->date,  nullchix,  4, header);
   if (chxlen (resp->date) < 10)  
       chxalter (resp->date, L(0), chxquick ("-9", 0), chxquick ("-199", 1));
   if (chxtoken (temp, nullchix,  5, header) >= 0) {
      chxcatval (resp->date, THIN, L(' '));
      if (chxlen(temp) < 5)  chxcatval (resp->date, THIN, L('0'));
      chxcat    (resp->date, temp);
   }
   chxtoken  (nullchix, resp->author, 6, header);

   /*** Get frozen property, if selected. */
   zero = 0;
   if (resp->rnum == 0  &&  (mask & P_FROZEN))
      resp->frozen = (char) chxint4 (frozen, &zero);

   /*** Get title, if selected. */
   if (mask & P_TITLE) {
      if (resp->rnum == 0) {
         chxclear (resp->title);
         if (title != nullchix)  chxcpy (resp->title, title);
      }

      /*** Copy over file attachment info. */
      if (resp->attach == (Attachment) NULL) resp->attach = make_attach();

      if (fast)  copy_attach (resp->attach, attach);
      else if (tbuf->attach[slot] != (Attachment) NULL)
                 copy_attach (resp->attach, tbuf->attach[slot]);
      else {
         resp->attach->size = 0;
         chxclear (resp->attach->db_name);
      }
   }

   /*** Get text, if selected. */
   if (mask & P_TEXT) {
      chxclear (resp->text->data);
      resp->text->lines = tbuf->lines   [slot];
      resp->text->chars = tbuf->bytes   [slot];
      resp->textprop    = tbuf->textprop[slot] & MA_TEXT;
      resp->bits        = tbuf->textprop[slot] & MA_BITS;
      chxcpy (resp->copied, tbuf->copied[slot]);
      for (;   (line = GETLINE (resp, lnum)) != nullchix;   ++lnum) {
         chxcat    (resp->text->data, line);
         chxcatval (resp->text->data, THIN, L('\n'));
      }
   }

   /*** Clean up. */
   chxfree   (temp);
   chxfree   (frozen);
   if (fast) {
      chxfree (fast_head);
      chxfree (fast_title);
      free_attach (&attach);
   }

   resp->ready = 1;
   return (A_OK);
}
