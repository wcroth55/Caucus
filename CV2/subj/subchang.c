/*** SUBCHANGE.  Change a subject, on local host and remotely.
/
/    ok = subchange (name, cnum, cname, owner, op, items);
/
/    Parameters:
/       int    ok;          (success?)
/       Chix   name;        (subject name)
/       int    cnum;        (conference number)
/       Chix   cname;       (conference name)
/       Chix   owner;       (psubject owner id)
/       int    op;          (1=>remove items, 0=>add items)
/       Chix   items;       (user selected items)
/
/    Purpose:
/       Take a list of items as entered by a user, and add or remove
/       them to/from a subject, both on the local host and remotely
/       (if CaucusLink is installed).
/
/    How it works:
/       Modify the OSUBJECT or PSUBJECT selected by NAME, conference 
/       number CNUM, and OWNER userid.  Remove items if OP==1, else
/       add them.  ITEMS is a list of items in highest-level form,
/       which may need considerable parsing.
/
/       OP and the ITEMS list is converted into a transaction-style
/       list of items, such as "+15@thor +27@saxon".  Chg_dsubject()
/       and chgxsubject() are called to make the changes locally and
/       on the network.
/
/    Returns: 1 on success, 0 on error.
/
/    Error Conditions:
/  
/    Side effects: May reload global "master" for conference CNUM.
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  subj/subchang.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/23/91 16:25 New function. */
/*: CR  7/28/91 15:48 chxalloc() first argument is int4. */
/*: CR  7/30/91 13:51 Replace license check with is_distributed(). */
/*: JV 10/31/91 15:30 Add dest_chn arg to addxosubject() call. */
/*: CR 11/07/91 13:56 Add cname argument. */
/*: DE  3/23/92 15:03 Chixified */
/*: DE  3/24/92 12:10 Chixified */
/*: JX  5/19/92 15:31 Fix Chix. */
/*: JX  8/09/92 16:41 Fix Chix. */
/*: CL 12/11/92 15:52 Long sysmem arg. */
/*: CN  5/15/93 15:20 Replace parse_range with new_parse(). */
/*: CT  5/28/93 13:47 Replace SELECT with Rlist. */
/*: CP  6/25/93 14:05 Add new args to new_parse() call. */
/*: CP  8/06/93 15:39 Skip item 0. */
/*: CP  8/09/93 11:41 Add 'attach' arg to new_parse() call. */
/*: CP  9/10/93 10:06 Add 'clearmod' arg to new_parse() call. */
/*: CK  9/15/93 13:36 Add 'mod1st' arg to new_parse() call. */
/*: CK 10/10/93 14:35 Add ERRCHK, ERRPOS args to new_parse() call. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"
#include "api.h"
#include "modifier.h"
#define   RLNULL   ( (Rlist) NULL )

extern struct master_template master;
extern struct this_template   thisitem;
extern union  null_t          null;

FUNCTION  subchange (name, cnum, cname, owner, op, items)
   Chix   name, owner, items, cname;
   int    cnum, op;
{
   int    oitem;
   int    i, error1, error2, ok, success, errpos;
   Chix   ctext;
   Chix   orighost, temp;
   char   modifier[MODIFIERS+5];
   Rlist  range, rptr;

   ENTRY ("subchange", "");

   orighost  = chxalloc (L(80),  THIN, "subchang orighost");
   temp      = chxalloc (L(100), THIN, "subchang temp");
   ctext     = chxalloc (L(200), THIN, "subchang ctext");
   range     = RLNULL;
   success   = 0;

   /*** Reload the "master" struct if wrong conf. */
   if (master.cnum != cnum  &&  NOT srvldmas (1, &master, cnum))  FAIL;

   /*** Parse the RANGE of items, and turn it into a map of the 
   /    selected items. */
   if (NOT new_parse (items, &range, modifier, nullchix, cnum, 
                             &thisitem, 1, 1, 1, &errpos)) SUCCEED;
   
   /*** Build the chix of the transaction-style item list. */
   for (rptr=range->next;   rptr!=RLNULL;   rptr=rptr->next) {
      for (i=rptr->i0;   i<=rptr->i1;   ++i) {
         if (i == 0)     continue;
         getoitem (&oitem, orighost, i, &master);
         chxclear (temp);
         chxformat (temp, CQ(" %s%d@%s\n"), L(oitem), L(0), 
                     (op ? chxquick("-",1) : chxquick("+",2)),
                      orighost);
         chxcat   (ctext, temp );
      }
   }

   ok = chg_dsubject (XT_CHANGE, cnum, cname, owner, name, ctext, &error1);

   /*** If distributed, write OSUBJECT change transaction. */
   if (ok  &&  EMPTYCHX(owner)  &&  is_distributed (cnum)) {
      ok = addxosubject (XT_CHANGE, cnum, name, ctext, nullchix, &error2);
   }
   DONE(ok);

 done:
   a_fre_rlist (range);
   chxfree (orighost);
   chxfree (temp);
   chxfree (ctext);

   RETURN  (success);
}
