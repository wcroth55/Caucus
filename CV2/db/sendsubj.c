/*** SEND_SUBJECTS.   Send all of the OSUBJECTS in a conference to a host.
/
/    ok = send_subjects (cnum, to_chn, master);
/
/    Parameters:
/       int    ok;                       (Success?)
/       int    cnum;                     (conference containing osubjects)
/       Chix   to_chn;                   (CHN of requesting host)
/       struct master_template *master;  (master record of items & responses)
/
/    Purpose:
/       Send the record of all of the OSUBJECTS in a conference CNUM to
/       a newly-subscribing host TO_CHN.
/
/    How it works:
/       Read the XCSF file for this conference, one subject at a time,
/       turn all the item references into "item@host", and send transactions
/       to create the subject and add all the items to it.
/
/    Returns: 1 on success.
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
/    Home:  xaction/sendsubj.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/21/91 17:29 New function. */
/*: DE  3/25/92 17:29 Chixified*/
/*: JX  5/13/92 16:45 Fix use of Flag, getline2(), nullchix. */
/*: JX  7/31/92 14:32 CHN's are chixen. */
/*: CL 12/11/92 15:52 Long sysmem arg. */
/*: CN  5/15/93 17:11 Use new item selection codes in 'found'. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"

extern union  null_t         null;

FUNCTION  send_subjects (cnum, to_chn, master)
   int                     cnum;
   Chix                    to_chn;
   struct master_template *master;
{
   int    sub;
   Chix   temp;
   short *found;
   Chix   orighost, itemhost, title, ctext;
   int    oitem, i, num, ok, error, success;
   char  *sysmem();
   Chix   format, getline2();

   ENTRY ("send_subj", "");

   orighost   = chxalloc (L(100), THIN, "send_subj orighost");
   itemhost   = chxalloc (L(100), THIN, "send_subj itemhost");
   title      = chxalloc (L(160), THIN, "send_subj title");
   ctext      = chxalloc (L(200), THIN, "send_subj ctext");
   format     = chxalloc (L(40),  THIN, "send_subj format");
   success    = 0;

   /*** Allocate an array to store list of item #s FOUND in each subject. */
   found = (short *)  sysmem (L(sizeof(short)) * L(master->items+2), "sendsubj");
   if (found == NULL) FAIL;

   for (sub=1;   getline2 (XCSD, XCSF, cnum, null.chx, 0, sub, 0, 
                         (Textbuf) NULL) != NULL;
            ++sub) {
      temp = getline2 (XCSD, XCSF, cnum, null.chx, 0, sub, 1, (Textbuf) NULL);
      if (temp == nullchix)  continue;
      chxcpy (title, temp);

      /*** Clear the list of items found in this subject. */
      for (i=1;   i<=master->items;   ++i)  found[i] = -1;

      /*** Scan all the lines of item numbers for this subject, and flag
      /    all the items that were FOUND. */
      for (num=2;
          (temp = getline2 (XCSD, XCSF, cnum, null.chx, 0, sub, num, 
                          (Textbuf) NULL)) !=nullchix;
           ++num)
         parsenum (temp, found, master->items, 0);


      /*** Build the chix of the transaction-style item list. */
      chxclear (ctext);
      for (i=1;   i<=master->items;   ++i)  if (found[i] >= 0) {
         getoitem (&oitem, orighost, i, master);
         chxclear  (itemhost);
         chxformat (itemhost, CQ(" +%d@%s\n"), L(oitem), L(0), orighost, 
                    null.chx );
         chxcat   (ctext, itemhost);
      }
   
      /*** Send a transaction creating the new subject, and another one
      /    to add the text of the subject. */
      if (ok = addxosubject (XT_NEW, cnum, title, nullchix, to_chn, 
                             &error)) {
          ok = addxosubject (XT_CHANGE, cnum, title, ctext, to_chn, 
                             &error);
      }
      if (NOT ok) {
         chxclear  (format);
         chxformat (format, 
                 CQ("send_subj failed: sub=%d, host %s, title %s, err=%d\n"),
                  L(sub), L(error), to_chn, title);
         buglist (ascquick(format));
      }
   }

   sysfree ( (char *) found);
   SUCCEED;

 done:
   chxfree (orighost);
   chxfree (itemhost);
   chxfree (title);
   chxfree (ctext);
   chxfree (format);

   RETURN (success );
}
