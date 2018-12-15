/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** CHGXRESP.  Create an ADD RESPONSE xaction.
/
/   int n = chgxresp (type, confnum, itemnum, itemchn, respnum, respchn,
/            author_name, author_id, text, date, time, dest_chn, prop, error)
/
/   Parameters:
/      int   type;          (XT_NEW, XT_CHANGE, or XT_DELETE)
/      int   confnum;       (conference number)
/      int   itemnum;       (item number on original host of item)
/      Chix  itemchn;       (original host of item)
/      int   respnum;       (response number on original host of response)
/      Chix  respchn;       (original host of response)
/      Chix  author_name;   (name of author of response: XT_NEW only)
/      Chix  author_id;     (userid of author: XT_NEW only)
/      Chix  text;          (text of response)
/      Chix  date;          (date of response: XT_NEW only)
/      Chix  time;          (time of response: XT_NEW only)
/      Chix  dest_chn;      (null => go everywhere; else go to one host)
/      int   prop;          (text property)
/      int  *error;         (returned error code)
/
/   Purpose:
/   1) For a new response, Caucus_x should call this to generate a xaction
/      and append it to the user's xaction file, so that this response
/      gets added on all the other appropriate hosts.  In this case
/      ITEMCHN should be the CHN of the host where the item was originally
/      added.  RESPNUM is the response number assigned on this host.
/      RESPCHN must be supplied by the caller.
/
/   2) For a response change/deletion, Caucus_x doesn't need to fill in
/      AUTHOR_NAME, and AUTHOR_ID as these aren't used.  However, ITEMCHN
/      must be the CHN of the host where the item was first added.  Further,
/      RESPNUM is the number of the response on the host where it was added
/      and RESPCHN must be the CHN of the host where the response was added.
/
/      If this is the original host for this conference,
/      CHGXRESP will change the local Caucus DB and then generate a
/      CHANGE RESP xaction headed for all neighbors that have
/      subscribed to this conf.
/
/      If this host is not the original host for this conf, CHGXRESP
/      will create an "IN TRANSIT" xaction headed for the original host.
/      Caucus_x will also change the local DB.
/
/      There the change will be applied and distributed.
/
/  3)  For deleting a response, caucus_x should call this with TYPE=DELETE
/      and fill in ITEMNUM, ITEMCHN, RESPNUM, RESPCHN, and CONF.
/      This is handled like (2) above in all other respects.
/
/      It does not affect the local Caucus DB in any way.
/
/   4) To initialize a subscriber, NetMaint sends one xaction per conference
/      object to the subscriber.  This "initialization" is indicated by
/      calling chgxresp with a valid DEST_CHN char pointer.  Otherwise,
/      the caller must put a null string ("") in DEST_CHN.
/
/   How it works:
/      First it checks to see if this is a change xaction.  If it is,
/      and it's the original host for this conf, it spreads a CHANGE RESP
/      xaction to all the neighbors that subscribe to this conf via
          sow_resp().
/
/      The rest is real simple.  Most of the data it needs is supplied,
/      tho it's got to look up this host's CHN.  The only weird
/      parts are:
/      1) the header for the CHANGE xactions need only a subset of the
/         header that the ADD xactions need;
/      2) change xactions go in XHML instead of XUXF.
/
/   Returns:
/      1 on success
/      0 on failure.  In this case it logs the error in the bugfile.
/
/   Error Conditions:
/
/   Related Functions: chgxitem, chgxtitle
/
/   Called by:
/
/   Home: xaction/chgxresp.c
/ */

/*: JV  6/05/91 11:46 Create this. */
/*: JV  6/10/91 16:09 Change the arg list, add code. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/01/91 14:38 Remove TITLE from args, chg get_chn call, clean. */
/*: JV  7/04/91 18:05 Remove lint, move get_chn, change text output. */
/*: JV  7/09/91 13:40 Completely rewrite to use putnextxact(). */
/*: JV  7/15/91 20:46 Add multiple text fields. */
/*: JV  7/23/91 16:11 Check 4 DATE==NULL, cuz somebody did that. */
/*: CR  7/26/91 18:33 Declare systime(), strtoken(); unit_lock(XUXF)->unlk.*/
/*: CR  7/27/91 14:17 Include "goodlint.h". */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: JV  7/29/91 12:24 Add arg to get_trueconf, fix putnextxact */
/*: JV 10/30/91 11:16 Add optional destination CHN for xactions. */
/*: JV 11/06/91 10:29 Change common.h to null.h. */
/*: JV 11/11/91 14:32 Allow CHN==null.str, add confochn. */
/*: JV 11/18/91 22:10 Fix UID vs DEST_CHN bug. */
/*: CR 11/25/91 22:35 Always insist on having RESPCHN. */
/*: JV 11/26/91 16:39 Send chg's generated at OCHN to XCXT, not XHML. */
/*: JV 12/07/91 19:26 Declare chxalloc(). */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: JV  1/03/92 14:50 Remove unused code and variable. */
/*: CR  2/03/92 21:36 Bugtell() if itemnum <= 0. */
/*: DE  6/02/92 16:41 Chixified */
/*: JX  9/01/92 12:51 Change empty_xact to make_xaction(). */
/*: CR 12/04/92 15:21 Add more detail to header comments. */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: JV  5/13/93 14:40 Change itemnum bugtell to say "chgxresp", not chgxitem. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "handicap.h"
#include "derr.h"
#include "chixuse.h"
#include "null.h"
#include "unitcode.h"
#include "xaction.h"
#include "goodlint.h"
#include "done.h"

#define MAXSTR 160

extern union null_t null;

FUNCTION chgxresp (type, confnum, itemnum, itemchn, respnum, respchn,
                   author_name, author_id, ctext, date, time, dest_chn,
          prop, error)
   int   type, confnum, itemnum, respnum, prop, *error;
   Chix  dest_chn;
   Chix  itemchn, respchn, author_name, author_id, date, time;
   Chix  ctext;
{
   Chix tname, locname, orighost, mychn, mydate, mytime;
   char temp[120];
   int  lhnum, unit, success;
   Chix carray[2];
   Xactionptr xact, make_xaction();
   int4    systime();
   char   *strtoken(), *bugtell();

   ENTRY ("chgxresp", "");

   tname     = chxalloc (L(80), THIN, "chgxresp tname");
   locname   = chxalloc (L(80), THIN, "chgxresp locname");
   orighost  = chxalloc (L(80), THIN, "chgxresp orighost");
   mychn     = chxalloc (L(80), THIN, "chgxresp mychn");
   mydate    = chxalloc (L(40), THIN, "chgxresp mydate");
   mytime    = chxalloc (L(20), THIN, "chgxresp mytime");
   success   = 0;
   
   xact = make_xaction();

   if (NOT get_chn (mychn, XSNF, 0)) {
      bug ("CHGXRESP: can't get XSNF.", 0); FAIL; }

   get_trueconf (confnum, locname, tname, orighost);

   xact->typenum = TT_RESPONSE;
   xact->actnum  = type;
   if ((type==XT_CHANGE || type==XT_DELETE) && NOT chxeq (orighost, mychn))
      xact->transit = 1;
   chxcpy (xact->conftname, tname);
   chxcpy (xact->confochn,  orighost);
   xact->conflocnum = confnum;
   xact->textprop   = prop;

   if (type == XT_NEW) {
      chxcpy  (xact->uname, author_name);
      if (chxindex (author_id, L(0), CQ("@")) >= 0)  chxcpy (xact->uid, author_id);
      else chxformat (xact->uid, CQ("%s@%s"), L(0), L(0), author_id, respchn);
   }
   xact->itemid = itemnum;
   xact->respid = respnum;
   chxcpy (xact->itemchn, itemchn);
   chxcpy (xact->respchn, respchn);
   if (itemnum <= 0)
      buglist (bugtell ("chgxresp: bad item #", itemnum, L(confnum), 
               ascquick(itemchn)));

   if (EMPTYCHX(date)) {
      date=mydate; time=mytime;
      sysdaytime  (date, time, 0, systime());
   }
   if (chxlen(date) > 0)
      chxformat (xact->date, CQ("%s %s"), L(0), L(0), date, time);
   if (type == XT_NEW || type == XT_CHANGE)
      xact->datasize[0] = chxlen (ctext);

   switch (type) {
   case XT_NEW:
      if (EMPTYCHX(dest_chn)) {                    /* Normal NEW xactions.  */
         unit = XUXF;
         if (NOT unit_lock (XUXF, WRITE, L(0), L(0), L(0), L(0), author_id)) {
            bug ("CHGXRESP: can't lock XUXF.", 0); *error=DLOCK;
            FAIL; }
      } else {                                   /* NEW CONF request */
         unit = XHML;
         if ((lhnum = hnumber (dest_chn)) < 1) {
            sprintf (temp, "CHGXRESP: sending to bad host name: %s.",
                     ascquick (dest_chn));
            bug (temp, 0); }
         if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
            bug ("CHGXRESP: can't lock XUXF.", 0); *error=DLOCK;
            FAIL; }
      }

      if (NOT unit_append (unit)) {
         bug ("CHGXRESP: can't open unit: ", unit); *error=DFILE;
         unit_unlk (unit); FAIL; }
  
      carray[0] = ctext;
      carray[1] = chxalloc (L(0), THIN, "chgxresp carray[1]");
      putnextxact (unit, xact, carray);
      chxfree (carray[1]);
      unit_close  (unit); unit_unlk (unit); break;

   case XT_CHANGE:
   case XT_DELETE:
      if (chxeq (mychn, orighost)) {
         if (NOT unit_lock (XCXT, WRITE, L(confnum), L(0), L(0), L(0), null.chx)) {
            bug ("CHGXRESP: can't lock XCXT.", 0); *error=DLOCK; FAIL; }
         if (NOT unit_append (XCXT)) {
               bug ("CHGXRESP: can't open XCXT.", 0);
               unit_unlk (XCXT);
               *error=DFILE;  FAIL; }
         if (type == XT_CHANGE) {
            carray[0] = ctext;
            carray[1] = chxalloc (L(0), THIN, "chgxresp carray[1]");
         } else
            carray[0] = carray[1] = chxalloc (L(0), THIN, "chgxresp carray");
         putnextxact (XCXT, xact, carray);
         unit_close (XCXT); unit_unlk (XCXT);
         chxfree (carray[1]);                 /* Frees both if XT_DELETE */
         break;

      } else {
         if (NOT get_course (orighost, &lhnum)) {
            bug ("CHGXRESP: host not found.", 0);  *error=DNOCHN; FAIL; }
         if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
            bug ("CHGXRESP: can't lock XHML.", 0); *error=DLOCK;  FAIL; }
         if (NOT unit_append (XHML)) {
            bug ("CHGXRESP: can't open XHML.", 0); *error=DFILE;
            unit_unlk (XHML); FAIL; }
         if (type == XT_CHANGE) {
            carray[0] = ctext;
            carray[1] = chxalloc (L(0), THIN, "chgxresp carray[1]");
         } else
            carray[0] = carray[1] = chxalloc (L(0), THIN, "chgxresp carray");
         putnextxact (XHML, xact, carray);
         chxfree (carray[1]);
         unit_close (XHML); unit_unlk (XHML);
         break;
      }
   default: bug ("CHGXRESP: bad xaction." ,0); FAIL;
   }

   SUCCEED;

done:
   chxfree (tname);
   chxfree (locname);
   chxfree (orighost);
   chxfree (mychn);
   chxfree (mydate);
   chxfree (mytime);
   free_xaction (xact);

   RETURN (success);
}   
