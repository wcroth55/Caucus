/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** CHGXITEM.  Create an ITEM xaction.
/
/   int n = chgxitem (type,confnum, itemnum, itemchn, author_name, author_id,
/                 title, text, date, time, dest_chn, prop, error)
/
/   Parameters:
/      int   type;         (type of xaction: XT_CHANGE, XT_DELETE, XT_NEW)
/      int   confnum;      (conference number)
/      int   itemnum;      (original item number (==local for new))
/      Chix  itemchn;      (originating host of item)
/      Chix  author_name;  (name of author of item)
/      Chix  author_id;    (userid of author of item)
/      Chix  title;        (title of item)
/      Chix  text;         (text of item)
/      Chix  date;         (date item created, for XT_NEW)
/      Chix  time;         (time item created, for XT_NEW)
/      Chix  dest_chn;     (xaction destined for dest_chn)
/      int   prop;         (text property)
/      int  *error;        (returned error code)
/
/   Purpose:
/   1) For a new item, Caucus_x should call this to generate a xaction
/      and append it to the user's xaction file, so that this item
/      gets added on all the other appropriate hosts.  In this case
/      ITEMCHN is required.
/
/   2) For an item change/deletion, Caucus_x doesn't need to fill in
/      AUTHOR_NAME, AUTHOR_ID, and TITLE as these aren't used.
/      However, ITEMCHN must be the CHN of the host where the item
/      was first added.
/
/      If this host is the original host for this conference,
/      CHGXITEM will generate a CHANGE ITEM xaction headed for
/      all neighbors that have subscribed to this conf.
/
/      If this host is not the original host for this conf, CHGXITEM
/      will create an "IN TRANSIT" xaction headed for the original host.
/      Caucus_x will also change the local DB.
/
/      There the change will be applied and distributed.
/
/   3) To delete an item, Caucus_x should call this with TYPE=DELETE
/      and fill in CONF and ITEMNUM and ITEMCHN.  This is handled
/      like (2) above in all other respects.
/
/   4) To initialize a subscriber, NetMaint sends one xaction per conference
/      object to the subscriber.  This "initialization" is indicated by
/      calling chgxitem with a valid DEST_CHN char pointer.  Otherwise,
/      the caller must put a null string ("") in DEST_CHN.
/
/   How it works:
/      It's real simple.  Most of the data it needs is supplied,
/      tho it's got to look up this host's CHN.  The only weird
/      parts are:
/      1) the header for the CHANGE xactions need only a subset of the
/         header that the ADD xactions need;
/      2) change xactions go in XHML instead of XUXF.
/      3) INITIAL is handled much the same way as changes going to the
/         original host, except that the destination CHN is handed to us.
/
/   Future Modifications:
/      Add an "ORIGINATION" field to the xaction header which indicates
/      the CHN where the change was made.  For changes and deletions this
/      can be used to prevent a change from being made twice on the
/      host where the change was originally made.
/
/   Returns:
/      1 on success
/      0 on failure.  In this case it logs the error in the bugfile.
/
/   Error Conditions:
/
/   Related Functions: chgxresp, chgxtitle
/
/   Called by:
/
/   Home: xaction/chgxitem.c
/ */

/*: JV  6/05/91 11:46 Create this. */
/*: JV  6/10/91 18:08 Change arg list, add code. */
/*: JV  6/28/91 13:28 Pre-Integration CARVMark*/
/*: JV  7/01/91 19:48 Change get_chn call. */
/*: JV  7/05/91 10:59 Move get_chn. */
/*: JV  7/08/91 18:09 Completely rewrite to use putnextxact(). */
/*: JV  7/15/91 20:42 Add multiple text fields. */
/*: JV  7/24/91 07:19 Check 4 DATE==NULL. */
/*: CR  7/26/91 18:32 Declare systime(), strtoken(). */
/*: CR  7/27/91 14:17 Include "goodlint.h". */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: JV  7/29/91 12:20 Add arg to get_trueconf, fix putnextxaction call. */
/*: JV 10/22/91 10:28 Add DEST_CHN stuff. */
/*: CR 10/21/91 22:25 Always use ITEMCHN, add header comments(!) */
/*: JV 10/30/91 10:43 Add optional destination CHN for xactions. */
/*: JV 11/06/91 10:29 Change common.h to null.h. */
/*: JV 11/11/91 14:23 Catch CHN==null.str, add confochn. */
/*: JV 11/13/91 14:08 Help header. */
/*: JV 11/18/91 21:22 Fix author vs. DEST_CHN. */
/*: CR 11/25/91 22:13 XT_NEW always sets name & uid. */
/*: JV 11/26/91 13:32 Send chg's generated at OCHN to XCXT, not XHML. */
/*: JV 12/04/91 14:42 Add SUBNUM field. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: JV  1/17/92 16:22 Remove old deadcode. */
/*: DE  6/02/92 15:52 Chixified */
/*: JX  7/31/92 14:01 Chixify DEST_CHN. */
/*: JX  9/01/92 12:45 Change empty_xact to make_xaction(). */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "chixuse.h"
#include "done.h"
#include "derr.h"
#include "handicap.h"
#include "null.h"
#include "unitcode.h"
#include "xaction.h"
#include "goodlint.h"

#define MAXSTR 160
#define LEAVE goto ERRORPROC

extern union null_t null;

FUNCTION chgxitem (type, confnum, itemnum, itemchn, author_name, author_id,
                   title, ctext, date, time, dest_chn, prop, error)
   int   type, confnum, itemnum, prop, *error;
   Chix  dest_chn;
   Chix  itemchn, author_name, author_id, title, date, time;
   Chix  ctext;
{
   Chix tname, locname, orighost, mychn, subscriber, mydate, mytime;
   char temp[120];
   int  lhnum, unit, success;
   Chix carray[2];
   Xactionptr xact, make_xaction();
   int4    systime();
   char   *strtoken();

   ENTRY ("chgxitem", "");

   tname      = chxalloc (L(80),  THIN, "chgxitem tname");
   locname    = chxalloc (L(80),  THIN, "chgxitem locname");
   orighost   = chxalloc (L(80),  THIN, "chgxitem orighost");
   mychn      = chxalloc (L(80),  THIN, "chgxitem mychn");
   subscriber = chxalloc (L(80),  THIN, "chgxitem subscriber");
   mydate     = chxalloc (L(20),  THIN, "chgxitem mydate");
   mytime     = chxalloc (L(20),  THIN, "chgxitem mytime");

   xact = make_xaction();

   if (NOT get_chn (mychn, XSNF, 0)) {
      bug ("CHGXITEM: can't get XSNF.", 0); FAIL; }

   get_trueconf (confnum, locname, tname, orighost);

   xact->typenum  = TT_ITEM;
   xact->subnum   = SUBTEXT;
   xact->actnum   = type;
   xact->textprop = prop;
   if ((type==XT_CHANGE || type==XT_DELETE) && NOT chxeq (orighost, mychn))
      xact->transit = 1;
   chxcpy (xact->conftname, tname);
   chxcpy (xact->confochn,  orighost);
   xact->conflocnum = confnum;

   if (type == XT_NEW) {
      chxcpy (xact->uname, author_name);
      if (chxindex (author_id, L(0), CQ("@")) >= 0) chxcpy (xact->uid, author_id);
      else  chxformat (xact->uid, CQ("%s@%s"), L(0), L(0), author_id, itemchn);
   }
   xact->itemid = itemnum;
   chxcpy (xact->itemchn, itemchn);
   if (type == XT_NEW) chxcpy (xact->title, title);

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
      if (EMPTYCHX (dest_chn)) { /* Normal NEW xactions.  */
         unit = XUXF;
         if (NOT unit_lock (XUXF, WRITE, L(0), L(0), L(0), L(0), author_id)) {
            bug ("CHGXITEM: can't lock XUXF.", 0); *error=DLOCK;
            FAIL; }
      } else {                                    /* NEW CONF request */
         unit = XHML;
         if ((lhnum = hnumber (dest_chn)) < 1) {
            sprintf (temp, "CHGXITEM: sending to bad host name: %s.",
                     ascquick (dest_chn));
            bug (temp, 0); }
         if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
            bug ("CHGXITEM: can't lock XUXF.", 0); *error=DLOCK;
            FAIL; }
      }

      if (NOT unit_append (unit)) {
         bug ("CHGXITEM: can't open unit: ", unit); *error=DFILE;
         unit_unlk (unit); FAIL; }
         
      carray[0] = ctext;
      carray[1] = chxalloc (L(0), THIN, "chgxitem carray[1]");
      putnextxact (unit, xact, carray);
      chxfree (carray[1]);
      unit_close  (unit); unit_unlk (unit);
      break;
   case XT_CHANGE:
   case XT_DELETE:
      if (type==XT_CHANGE) {
         carray[0] = ctext;
         carray[1] = chxalloc (L(0), THIN, "chgxitem carray[1]");
      } else
         carray[0] = carray[1] = chxalloc (L(0), THIN, "chgxitem carray");
      if (chxeq (mychn, orighost)) {
         if (NOT unit_lock (XCXT, WRITE, L(confnum), L(0), L(0), L(0), null.chx)) {
            bug ("CHGXITEM: can't lock XCXT.", 0); *error=DLOCK; LEAVE; }
         if (NOT unit_append (XCXT)) {
            bug ("CHGXITEM: can't open XCXT.", 0);
            unit_unlk (XCXT);
            *error=DFILE;  LEAVE; }
         putnextxact (XCXT, xact, carray);
         unit_close (XCXT); unit_unlk (XCXT);
         chxfree (carray[1]);                /* Frees both if XT_DELETE */

      } else {
         if (NOT get_course (orighost, &lhnum)) {
            bug ("CHGXITEM: host not found.", 0);  *error=DNOCHN; LEAVE; }
         if (NOT unit_lock (XHML, WRITE, L(lhnum), L(0), L(0), L(0), null.chx)) {
            bug ("CHGXITEM: can't lock XHML.", 0); *error=DLOCK;  LEAVE; }
         if (NOT unit_append (XHML)) {
            bug ("CHGXITEM: can't open XHML.", 0); *error=DFILE;
            unit_unlk (XHML);  LEAVE; }
         putnextxact (XHML, xact, carray);
         chxfree (carray[1]);
         unit_close (XHML);  unit_unlk (XHML);
      }
      break;
   default: bug ("CHGXITEM: bad xaction.", 0); FAIL;
   }

   SUCCEED;

ERRORPROC:
   chxfree (carray[1]);
   success = 0;
 done:
   chxfree (tname);
   chxfree (locname);
   chxfree (orighost);
   chxfree (mychn);
   chxfree (subscriber);
   chxfree (mydate);
   chxfree (mytime);
   free_xaction (xact);

   RETURN (success);
}   
