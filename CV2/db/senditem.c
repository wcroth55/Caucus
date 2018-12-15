/*** SEND_ITEM.   Send an item to a newly subscribing host.
/
/    ok = send_item (cnum, item, to_chn, master);
/
/    Parameters:
/       int    ok;                       (Success?)
/       int    cnum;                     (conference containing item)
/       int    item;                     (local item # to be sent)
/       int    cnum;                     (local number of conference to be sent)
/       Chix   to_chn;                   (CHN of requesting host)
/       struct master_template *master;  (master record of items & responses)
/
/    Purpose:
/       Send an entire item and its responses to a newly subscribing host.
/
/    How it works:
/       Send_item() reads the entire item, and sends it out as
/       individual transactions via chgxitem() and chgxresp().
/
/    Returns:
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Related functions:
/
/    Called by:  send_conf()
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  xaction/senditem.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/21/91 17:29 New function. */
/*: CR 10/29/91 11:50 Add cnum argument. */
/*: JV 11/13/91 09:37 Use item arg in chgxitem call. */
/*: CR 11/13/91 14:10 Correct chgxitem(), chgxresp() calls to use OITEM. */
/*: CR 11/14/91 18:08 Pick up response text starting at line 1.*/
/*: CR 12/20/91 14:59 Include frozen status of item with frzxitem(). */
/*: CR  4/17/92 14:05 Copy header/title, don't just use pointers! */
/*: DE  3/25/92 16:56 Chixified */
/*: DE  4/07/92 13:59 Patch to use (char) bugtell */
/*: JX  5/16/92 15:00 Fix return from chxtoken's. */
/*: JX  7/31/92 14:31 CHN's are chixen. */
/*: CI 10/08/92 11:08 Chix Integration. */
/*: CR 10/08/92 22:58 Make it compile! */
/*: CR 10/09/92 17:09 Alloc header, title; fix hd_ptr test. */
/*: JV 11/17/92 17:55 Change orighost from Chix to short. */
/*: CL 12/10/92 17:06 Long ascofchx args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CK  3/18/93 16:00 Send "freeze" xaction *after* responses are sent! */
/*: CR  6/16/95 16:12 Add text properties. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"

extern union null_t null;
extern Chix        *hostnames;

FUNCTION  send_item (cnum, item, to_chn, master)
   int                     cnum, item;
   Chix                    to_chn;
   struct master_template *master;
{
   int       i, error, ok, resp, oresp, oitem, success;
/* char     *author_name, *hd_ptr, *ti_ptr, *line; */
   Chix      itemchn, temp, author_id;
   char      titlestr[160];
   Chix      date, time, respchn, frozen;
   Chix      author_name, header, hd_ptr,  title, ti_ptr,  line;
   Chix      ctext, getline2();
   char     *bugtell();

   ENTRY ("send_item", "");

   date      = chxalloc (L(100), THIN, "senditem date");
   time      = chxalloc (L(100), THIN, "senditem time");
   temp      = chxalloc (L(100), THIN, "senditem temp");
   ctext     = chxalloc (L(500), THIN, "senditem ctext");
   title     = chxalloc (L(100), THIN, "senditem title");
   frozen    = chxalloc (L(100), THIN, "senditem frozen");
   header    = chxalloc (L(100), THIN, "senditem header");
   itemchn   = chxalloc (L(102), THIN, "senditem itemchn");
   respchn   = chxalloc (L(100), THIN, "senditem respchn");
   author_id = chxalloc (L(100), THIN, "senditem author_id");
   author_name = chxalloc (L(40), THIN, "senditem author_name");
   success   = 0;

   /*** Get originating host chn for this item. */
   if (master->origitem[item] > 0) {
      oitem = master->origitem[item];
      chxcpy (itemchn, hostnames[master->orighost[item]]);
   }
   else {
      oitem = item;
      getmyhost (itemchn);
   }

   /*** Get the header and title from the item partfile. */
   hd_ptr = getline2 (XCRD, XCRF, cnum, null.chx, item, 0, 0, (Textbuf) NULL);
   ti_ptr = getline2 (XCRD, XCRF, cnum, null.chx, item, 0, 1, (Textbuf) NULL);
   if (hd_ptr == nullchix  ||  ti_ptr == nullchix)   FAIL;
   chxcpy (header, hd_ptr);
   chxcpy (title,  ti_ptr);

   /*** Pluck out the author name & id, date, & time. */
   if (chxtoken (temp, author_name, 6, header) == -1)
      chxclear (author_name);
   chxtoken (author_id, nullchix, 2, header);
   chxtoken (frozen,    nullchix, 3, header);
   chxtoken (date,      nullchix, 4, header);
   chxtoken (time,      nullchix, 5, header);

   /*** Assemble text of item into a chix. */
   for (i=2;   (line = getline2 (XCRD, XCRF, cnum, null.chx, item, 0, i,
                               (Textbuf) NULL)) != nullchix;
        ++i) {
      chxcat (ctext, line);
      chxcat (ctext, chxquick ("\n", 0));
   }

   /*** Send the item text (proper) aint4 to TO_CHN. */
   ok = chgxitem (XT_NEW, cnum, oitem, itemchn, author_name, author_id, title,
                  ctext, date, time, to_chn, 0, &error);
   if (NOT ok) {
      ascofchx (titlestr, title, L(0), L(160));
      buglist (bugtell ("sendconf: chgxitem failed", item,
                          (int4) error, titlestr));
      FAIL;
   }

   /*** Now examine each response to this item... */
   for (resp=1;   resp<=master->responses[item];   ++resp) {
      getoresp (&oresp, respchn, cnum, item, resp);

      /*** Get the header (skipping deleted responses), and pluck out the
      /    author name & id, and date & time created. */
      hd_ptr = getline2 (XCRD, XCRF, cnum, nullchix, item, resp, 0,
                       (Textbuf) NULL);
      if (hd_ptr == nullchix)   continue;
      chxcpy (header, hd_ptr);
      if ( chxtoken (temp, author_name, 6, header) == -1)  continue;
      chxtoken (author_id, nullchix, 2, header);
      chxtoken (date,      nullchix, 4, header);
      chxtoken (time,      nullchix, 5, header);

      /*** Assemble text of response into a chix. */
      chxclear (ctext);
      for (i=1;
         (line = getline2 (XCRD, XCRF, cnum, null.chx, item, resp, i,
                         (Textbuf) NULL)) != nullchix;
           ++i) {
         if (chxvalue(line,0L) == '+')  continue;
         chxcat (ctext, line);
         chxcat (ctext, chxquick ("\n", 0));
      }

      /*** Send this response on to host TO_CHN.   (What should we do
      /    on error return?) */
      chgxresp (XT_NEW, cnum, oitem, itemchn, oresp,
                respchn, author_name, author_id, ctext, date, time,
                to_chn, 0, &error);
   }

   /*** Finally, if the item is frozen, send aint4 a transaction to freeze
   /    it (now that the responses have been sent!). */
   if (chxvalue(frozen,0L) != '0')
      frzxitem (cnum, oitem, itemchn, 1, nullchix, nullchix, to_chn, &error);

   SUCCEED;

 done:
   chxfree (temp);
   chxfree (date);
   chxfree (time);
   chxfree (ctext);
   chxfree (title);
   chxfree (header);
   chxfree (itemchn);
   chxfree (respchn);
   chxfree (frozen);
   chxfree (author_id);
   chxfree (author_name);

   RETURN  (success);
}
