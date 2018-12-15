/*** A_ADD_RESP.   Add a response or item to a conference.
/
/    ok = a_add_resp (resp, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Resp      resp;     (object containing new response)
/       int4      mask;     (P_ATTACH indicates attachment)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Add a new response (or, if the response number is 0, a new item).
/
/    How it works:
/       Adding a new item: caller fills in CNUM, INUM = -1, TEXT, TITLE,
/       AUTHOR, OWNER members of RESP, optionally COPIED.
/
/       Adding a new response: caller fills in CNUM, INUM, TEXT, and
/       AUTHOR, OWNER members of RESP, optionally COPIED.
/ 
/       A_add_resp() fills in INUM, RNUM, DATE, and TIME as it works.
/
/       A_add_resp calls chg_ditem(), chgxitem(), chg_dresp(), 
/       and chgxresp() as needed to do the dirty work.
/
/       To ensure that file attachment info doesn't exist for a non-existent
/       attachment, we use the following algorithm:
/          1) Lock all relevant files or bail-out
/          2) chg_dresp() to add an empty response
/          3) copy attached file to ATTACH/Cnnn/iiirrr
/          4) if copy fails, delete response
/          5) Change response to hold real text
/          6) Send response to 'Link neighbors
/          7) Send attachment to 'Link neighbors
/
/    Returns: A_OK       success
/             A_NOCONF   no such conference
/             A_NOWRITE  user not allowed to write in conf or to item
/             A_NOITEM   no such item
/             A_DBERR    unknown error
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
/    Home:  api/aaddresp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/03/92 14:44 New function. */
/*: CR 12/04/92 15:27 Make it compile! */
/*: CR 12/07/92  0:01 Set 'ready' flag. */
/*: CR 12/07/92 10:41 Use L(' ') in chxcatval(). */
/*: CL 12/11/92 15:08 Long chxint4 arg. */
/*: CR  2/22/93 23:31 Add new args to a_cache() calls. */
/*: CR  2/28/93 21:44 Replace global 'tbuf' with a_cch_tbuf call. */
/*: JV  3/03/93 14:47 Add File Attachments. */
/*: JA  7/27/93 16:15 Change file locking order. */
/*: JA  7/28/93 09:15 Fix call to chg_dresp(). */
/*: CK  8/12/93 18:02 Copy attachment from attach->name, not ->db_name. */
/*: CR  6/16/95 16:12 Add text properties. */
/*: CR  5/14/04 Use co_priv_str() to check access levels. */
/*: CR  7/02/04 Add logger() debugging when add response fails. */
/*: CR  7/16/04 Fix bug when checking for permission to add new item. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"
#include "xaction.h"

extern struct   master_template  master;
extern Textbuf  tbuf;

FUNCTION  a_add_resp (Resp resp, int4 mask, int wait)
{
   int     access, additem, chgresp, item, error, success, ok, response,
           oitem, chgsubj;
   char    temp[1000];
   Chix    cday, ctime, chn, frozen, itemchn;
   Chix   hdr, emptychx, getline2();
   int4    zero, systime();
   Textbuf tbuf, a_cch_tbuf();

   if (disk_failure (0, &error, NULL))                    return (A_DBERR);

   if (resp == (Resp) NULL  ||  resp->tag != T_RESP)      return (A_BADARG);
   if (resp->cnum <= 0)                                   return (A_NOCONF);

   /*** Make sure this user can write in this conference. */
   if (NOT a_cache (V_GET, resp->cnum, &access, &additem, &chgresp,
                           &chgsubj, nullchix))            return (A_NOCONF);
/* if (access < 2)                                        return (A_NOWRITE); */
   if (access < co_priv_str("include")) {
      sprintf (temp, "JD:access=%d, include=%d", access, 
                     co_priv_str("include"));
      logger  (1, LOG_FILE, temp);
      return (A_NOWRITE);
   }

/* if (access < 3  &&  resp->rnum == 0  &&  NOT additem)  return (A_NOWRITE); */
/* if (access < co_priv_str("instructor")   &&  
                       resp->rnum == 0  &&  NOT additem)  { */
   if (access < co_priv_str("instructor")   &&  
                       resp->inum  < 0  &&  NOT additem)  {
      sprintf (temp, "JD:access=%d, instructor=%d, rnum=%d, additem=%d", 
                     access, co_priv_str("instructor"), resp->rnum, additem);
      logger  (1, LOG_FILE, temp);
      return (A_NOWRITE);
   }

   chn    = chxalloc ( L(4), THIN, "a_add_re chn");
   cday   = chxalloc (L(20), THIN, "a_add_re cday");
   ctime  = chxalloc (L(20), THIN, "a_add_re ctime");
   frozen = chxalloc ( L(4), THIN, "a_add_re frozen");
   emptychx = chxalloc (L(4), THIN, "a_add_re empty");

   sysdaytime (cday, ctime, 0, systime());
   chxcpy    (resp->date, cday);
   chxcatval (resp->date, THIN, L(' '));
   chxcat    (resp->date, ctime);

   /*** New Item. */
   if (resp->inum < 0) {
      if (NOT (mask & P_ATTACH)) {
         chg_ditem (XT_NEW, resp->cnum, 0, &item, chn,
                   resp->author, resp->owner, resp->text->data, resp->title,
                   cday, ctime, resp->attach, resp->textprop | resp->bits, 
                   resp->copied, &error);
         if (error != 0)  DONE (A_DBERR);
         resp->inum = item;
         resp->rnum = 0;
      }
      else {     /* Add attachment and item.  See algorithm in header. */
         error = add_resp_attach (resp, cday, ctime);
         if (error) DONE (error);
      }

   }

   /*** New response. */
   else {

      /*** Decide if this item is frozen. */
      tbuf = a_cch_tbuf (V_GET, XCRD, XCRF, resp->cnum, resp->inum, 0, 
                                nullchix);
      hdr  = getline2 (XCRD, XCRF, resp->cnum, nullchix, resp->inum, 0, 0, tbuf);
      if (hdr == nullchix)  {
         sprintf (temp, "JD:getline(%d,%d)=null", resp->cnum, resp->inum);
         logger  (1, LOG_FILE, temp);
         DONE (A_NOITEM);
      }
      chxtoken (frozen, nullchix, 3, hdr);
      zero = 0;
      if (chxint4 (frozen, &zero) > 0) {
         sprintf (temp, "JD:frozen(%d,%d)=%s", resp->cnum, resp->inum,
                         ascquick(frozen));
         logger  (1, LOG_FILE, temp);
         DONE (A_NOWRITE);
      }

      if (NOT (mask & P_ATTACH)) {
         /*** Add the response locally. */
         chg_dresp (XT_NEW, resp->cnum, resp->inum, 0, &response, chn,
                    resp->author, resp->owner, resp->text->data, cday, ctime, 
                    mask & P_ATTACH ? resp->attach : (Attachment) NULL, 
                    resp->textprop | resp->bits, resp->copied, &error);
         if (error != 0)  DONE (A_DBERR);
         resp->rnum = response;
      }
      else {      /* Add attachment & response. See algorithm in header. */
         error = add_resp_attach (resp, cday, ctime);
         if (error) DONE (error);
      }
 
   }

   resp->ready = 1;
   DONE (A_OK);

done:
   a_cch_tbuf (V_CLR, XCRD, XCRF, resp->cnum, resp->inum, 0, nullchix);
   chxfree (chn);
   chxfree (cday);
   chxfree (ctime);
   chxfree (frozen);
   chxfree (emptychx);

   return  (success);
}

/*** ADD_RESP_ATTACH: Only used when adding an item or resp *and*
/    an attachment. */

add_resp_attach (resp, cday, ctime)
Chix cday, ctime;
Resp resp;
{
   char name[100];
   int  success=0, response, error;
   Chix chn, owner, ownerh;

   chn    = chxalloc ( L(4), THIN, "a_add_re chn");
   owner  = chxalloc (L(16), THIN, "a_add_re owner");
   ownerh = chxalloc (L(16), THIN, "a_add_re ownerh");

   /*** First check for existence of ATTACH/Cnnn. */
   if (NOT unit_lock  (XACD, READ, L(resp->cnum), L(0), L(0), L(0), nullchix))
      DONE (A_DBERR);

   if (NOT unit_check (XACD)) {
      unit_name (name, XACD, L(resp->cnum), L(0), L(0), L(0), nullchix);
      sysmkdir  (name, 0700, NULL);
   }
   unit_unlk (XACD);

   /*** Lock source of attachment. */
   chxbreak (resp->owner, CQ("@"), owner, ownerh);
   if (NOT unit_lock (XIRE, READ,  L(0), L(0), L(0), L(0), resp->attach->name))
      DONE (A_DBERR);

   /*** Add empty response locally. */
   chg_dresp (XT_NEW, resp->cnum, resp->inum, 0, &response, chn,
              resp->author, resp->owner, nullchix, cday, ctime, 
          (Attachment) NULL, resp->textprop | resp->bits, resp->copied, &error);
   if (error != 0) {
      unit_unlk (XARF);
      unit_unlk (XIRE);
      DONE (A_DBERR);
   }
   resp->rnum = response;

   /*** Lock file where attachment will actually go. */
   if (NOT unit_lock (XARF, WRITE, L(resp->cnum), L(resp->inum),
                      L(response), L(0), nullchix)) {
      unit_unlk (XIRE);
      DONE (A_DBERR);
   }
         
   /*** Copy the attachment to local DB. */
   if (NOT unit_b_copy (XIRE, XARF)) {
      /* If copy failed, back out the empty response. */
      chg_dresp (XT_DELETE, resp->cnum, resp->inum, 0, &response, nullchix,
         nullchix, nullchix, nullchix, nullchix, nullchix, 
         (Attachment) NULL, resp->textprop | resp->bits, resp->copied, &error);
      unit_unlk (XIRE);
      unit_kill (XARF);   /* Maybe some was copied over. */
      unit_unlk (XARF);
      DONE (A_DBERR);
   }
   unit_unlk   (XIRE);    /* Done with this. */

   /*** Add the real text locally. */
   chg_dresp (XT_CHANGE, resp->cnum, resp->inum, 0, &response, nullchix,
              nullchix, nullchix, resp->text->data, nullchix, nullchix,
              resp->attach, resp->textprop | resp->bits, resp->copied, &error);

   if (error != 0) {
      unit_kill (XARF);
      unit_unlk (XARF);
      DONE (A_DBERR);
   }

   unit_unlk (XARF);

done:
   chxfree (chn);
   chxfree (owner);
   chxfree (ownerh);

   return (success);
}
