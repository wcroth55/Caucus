/*** A_CHG_RESP.   Change a response or item in a conference, or
/                  (add or change) an attachment to an item or response.
/
/    ok = a_chg_resp (resp, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Resp      resp;     (object selecting response to be deleted)
/       int4      mask;     (which property to change?)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Change a response (or, if the response number is 0, an item).
/       Add or change an attachment to an item or response.
/
/       You can only change *one* of TEXT | ATTACH.  To change both,
/       call this function twice.
/
/    How it works:
/       Caller fills in CNUM, INUM, and RNUM.
/       Caller fills in new TEXT (if P_TEXT), or title (P_TITLE,
/       RNUM==0), or attachment info (if P_ATTACH), or else
/       FROZEN status (P_FROZEN, RNUM==0).
/ 
/       A_chg_resp calls chg_ditem(), chgxitem(), chg_dresp(), chgxresp(),
/       chg_dtitle(), chgxtitle(), frz_ditem(), and frzxitem() as needed
/       to do the dirty work.
/
/       To ensure that we don't store file attachment info for a failed
/       attempt to store an attachment, we use the following algorithm
/       when storing it:
/          0) Get response
/          1) Lock all relevant files or bail-out
/          2) Copy attached file to ATTACH/Cnn/iiirrr
/          3) Change response to hold original text and new attach info.
/          4) Send attachment to 'Link neighbors
/
/    Returns: A_OK        success
/             A_BADARG    bad arguments
/             A_NOCONF    no such conference
/             A_NOWRITE   user not allowed to write in conf or to item
/             A_NORESP    no such item/resp
/             A_NOTHAW    item cannot be thawed
/             A_NOTOWNER  user not owner or not organizer
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
/    Home:  api/achgresp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/06/92 23:18 New function. */
/*: JV  3/03/93 14:48 Add attach arg to chg_dresp(), chg_ditem(). */
/*: JV  5/25/93 13:09 Add code for attachments, correct Home: field in header. */
/*: CR  2/22/93 23:31 Add new args to a_cache() calls. */
/*: CR  2/28/93 22:10 Replace global 'tbuf' with a_cch_tbuf call. */
/*: JV  6/01/93 17:19 Clear tbuf cache on exit. */
/*: CP  7/14/93 15:56 Integrate 2.5 changes. */
/*: CK  8/12/93 18:02 Copy attachment from attach->name, not ->db_name. */
/*: JV  9/21/93 15:57 Remove tbuf use, call a_get_resp instead, use oldresp. */
/*: CR  6/16/95 16:12 Add text properties. */
/*: CR  6/30/95 15:49 Fix bug: use oldresp->owner. */
/*: CR  3/16/00 16:11 Implement P_OWNER and P_AUTHOR changes. */
/*: CR  5/14/04 Use co_priv_str() to check access levels. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"
#include "xaction.h"

extern struct   master_template  master;

FUNCTION  a_chg_resp (Resp resp, int4 mask, int wait)
{
   char    name[100];
   int     access, delitem, chgresp, item, oitem, rnum, success,
           can_change, ok, error, oresp, chgsubj, inum,
           rc;
   int     distributed = 0;
   Chix    userid, itemchn, respchn;
   Resp    oldresp;
   int     newprop;

   if (disk_failure (0, &error, NULL))                    return (A_DBERR);

   if (resp == (Resp) NULL  ||  resp->tag != T_RESP)      return (A_BADARG);
   if (resp->cnum <= 0)                                   return (A_NOCONF);

   /*** Make sure this user can access this conference. */
   if (NOT a_cache (V_GET, resp->cnum, &access, &delitem, &chgresp,
                           &chgsubj, nullchix))           return (A_NOCONF);
/* if (access < 2)                                        return (A_NOWRITE); */
   if (access < co_priv_str("include"))                   return (A_NOWRITE);

   /*** Get old response info to limit that which caller must supply. */
   oldresp = a_mak_resp ("a_chg_resp old");
   oldresp->cnum = resp->cnum;
   oldresp->inum = resp->inum;
   oldresp->rnum = resp->rnum;
   
   ok = a_get_resp (oldresp, P_EVERY, A_WAIT);
   if (ok != A_OK) return (ok);

   /*** Can the user change this response?  (Is the user the owner or
   /    an organizer?) */
   if (access < co_priv_str("instructor")) {
      userid = chxalloc (L(20), THIN, "a_chg_re userid");
      sysuserid (userid);
      can_change = equal_id (oldresp->owner, userid, nullchix);
      chxfree (userid);
      if (NOT can_change)  return (A_NOTOWNER);
   }


   newprop = resp->textprop | (mask & P_BITS ? resp->bits : oldresp->bits);

   rc = A_BADARG;   /* In case no legal property is changed... */

   /*** Change an item text. */
   if (resp->rnum == 0  &&  (mask & P_TEXT)) {
      item = resp->inum;

      if (distributed) {
         ok = chgxitem (XT_CHANGE, resp->cnum, oitem, itemchn, nullchix,
            nullchix, nullchix, nullchix, nullchix, nullchix, nullchix, 
            newprop, &error);
         if (NOT ok)  DONE (A_DBERR);
      }

      chg_ditem (XT_CHANGE, resp->cnum, -1, &item,
                 nullchix, nullchix, nullchix, resp->text->data,
                 nullchix, nullchix, nullchix, oldresp->attach, 
                 newprop, oldresp->copied, &error);
      if (error != 0)  DONE (A_DBERR);
      rc = A_OK;
   }

   /*** Change owner or author (or both).   Must be organizer. */
   if ((mask & P_OWNER)  ||  (mask & P_NAME)) {
      if (access < co_priv_str("instructor"))  DONE (A_NOTOWNER);
      if (mask & P_OWNER)  chxcpy (oldresp->owner,  resp->owner);
      if (mask & P_NAME)   chxcpy (oldresp->author, resp->author);

      rnum = resp->rnum;
      chg_dresp (XT_CHANGE, resp->cnum, resp->inum, -1, &rnum, nullchix,
           oldresp->author, oldresp->owner, oldresp->text->data, nullchix, 
           nullchix, oldresp->attach, newprop, oldresp->copied, &error);
      if (error != 0)  DONE (A_DBERR);
      rc = A_OK;
   }

   /*** Change an item title. */
   if (resp->rnum == 0  &&  (mask & P_TITLE)) {
      if (distributed) {
         ok = chgxtitle (resp->cnum, oitem, itemchn, resp->title, &error);
         if (NOT ok)   DONE (A_DBERR);
      }

      chg_dtitle (resp->cnum, resp->inum, resp->title, &error);
      a_cch_tbuf (V_CLR, XCRD, XCRF, resp->cnum, resp->inum, 0, nullchix);
      if (error != 0)  DONE (A_DBERR);
      rc = A_OK;
   }

   /*** Change an item's frozen status. */
   if (resp->rnum == 0  &&  (mask & P_FROZEN)) {
      if (distributed) {
         ok = frzxitem (resp->cnum, oitem, itemchn, resp->frozen, nullchix,
                          nullchix, nullchix, &error);
         if (NOT ok)   DONE (A_DBERR);
      }

      ok = frz_ditem (resp->cnum, resp->inum, resp->frozen);
      if (NOT ok)      DONE (A_NOTHAW);
      a_cch_tbuf (V_CLR, XCRD, XCRF, resp->cnum, resp->inum, 0, nullchix);
      rc = A_OK;
   }

   /*** Change a single response. */
   if (resp->rnum != 0  &&  (mask & P_TEXT)) {
      if (distributed) {
         respchn = chxalloc (L(30), THIN, "a_chg_re respchn");
         getoresp (&oresp, respchn, resp->cnum, resp->inum, resp->rnum);
         ok = chgxresp (XT_CHANGE, resp->cnum, oitem, itemchn, oresp, respchn,
            nullchix, nullchix, resp->text->data, nullchix, nullchix, nullchix,
            newprop, &error);
         chxfree  (respchn);
         if (NOT ok)   DONE (A_DBERR);
      }
      rnum = resp->rnum;
      chg_dresp (XT_CHANGE, resp->cnum, resp->inum, -1, &rnum, nullchix,
           nullchix, nullchix, resp->text->data, nullchix, nullchix,
           oldresp->attach, newprop, oldresp->copied, &error);
      if (error != 0)  DONE (A_DBERR);
      rc = A_OK;
   }

   /*** Add or change an attachment to an item or response. */
   if (mask & P_ATTACH) {
      if (distributed) {
         respchn = chxalloc (L(30), THIN, "a_chg_re respchn");
         getoresp (&oresp, respchn, resp->cnum, resp->inum, resp->rnum);
         chxfree  (respchn);
      }
      rnum = resp->rnum;

      /*** First check for existence of ATTACH/Cnnn. */
      if (NOT unit_lock  (XACD, READ, L(resp->cnum), L(0), L(0), L(0), nullchix))
         DONE (A_DBERR);
  
      if (NOT unit_check (XACD)) {
         unit_name (name, XACD, L(resp->cnum), L(0), L(0), L(0), nullchix);
         sysmkdir  (name, 0700, NULL);
      }
      unit_unlk (XACD);
  
      /*** Lock relevant files. */
      if (NOT unit_lock (XIRE, READ,  L(0), L(0), L(0), L(0), resp->attach->name))
         DONE (A_DBERR);

      if (NOT unit_lock (XARF, WRITE, L(resp->cnum), L(resp->inum),
                         L(resp->rnum), L(0), nullchix)) {
         unit_unlk (XIRE);
         DONE (A_DBERR);
      }

      /*** Copy the attachment to local DB. */
      if (NOT unit_b_copy (XIRE, XARF)) {
         unit_unlk (XIRE);
         unit_kill (XARF);   /* Maybe some was copied over. */
         unit_unlk (XARF);
         DONE (A_DBERR);
      }
      unit_unlk   (XIRE);    /* Done with this. */
  
      /*** Add attachment info to header lines. */
      if (rnum > 0)
         chg_dresp (XT_CHANGE, resp->cnum, resp->inum, 0, &rnum, nullchix,
              nullchix, nullchix, oldresp->text->data, nullchix, nullchix,
              resp->attach, oldresp->textprop | oldresp->bits, 
              oldresp->copied, &error);
      else {
         inum = resp->inum;
         chg_ditem (XT_CHANGE, resp->cnum, 0, &inum, nullchix, nullchix,
                    nullchix, oldresp->text->data, nullchix, nullchix, nullchix,
                    resp->attach, oldresp->textprop | oldresp->bits, 
                    oldresp->copied, &error);
      }
      if (error != 0) {
         unit_kill (XARF);
         unit_unlk (XARF);
         DONE (A_DBERR);
      }
      unit_unlk   (XARF);    /* Done with this. */
      rc = A_OK;
   }

   resp->ready = 1;
   DONE (rc);

done:
   if (distributed)  chxfree (itemchn);
   a_fre_resp (oldresp);
   return  (success);
}
