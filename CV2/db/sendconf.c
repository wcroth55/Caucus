
/*** SEND_CONF.   Send an entire conference to a subscribing host.
/
/    ok = send_conf (cnum, to_chn);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    cnum;        (local number of conference to be sent)
/       Chix   to_chn;      (CHN of requesting host)
/
/    Purpose:
/       Send an entire conference CNUM on the local host, to a host
/       TO_CHN that is newly subscribing to that conference.
/
/       The caller MUST call ok_to_send_conf() first to determine
/       if it is legal to send conference CNUM to host TO_CHN.
/
/    How it works:
/       Send_conf() reads the entire conference, and sends it out as
/       individual transactions.  Specifically, it sends:
/           all items, with their responses, in order
/           OSUBJECTS
/           CUSTOMIZE ADD status
/           CUSTOMIZE CHANGE status
/           primary organizer id (and host)
/           text of conference greeting
/           text of conference introduction
/           CUSTOMIZE VISIBILITY status
/           CUSTOMIZE USERS userlist text
/
/       Note that the caller must "hook in" host TO_CHN first, then
/       immediately call send_conf().  This ensures that any new
/       material added to conference CNUM during the execution
/       of send_conf() will still get sent.
/
/
/    Returns: 1 on success
/             0 on error
/
/    Error Conditions:
/       Conference does not exist.
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  XFU
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  xaction/sendconf.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/21/91 17:29 New function. */
/*: CR 11/05/91 19:50 Add userlist, greeting, intro, etc. */
/*: CR 11/06/91 11:15 Decl strtoken(). */
/*: CR 11/07/91 14:18 Add confnum arg to matchnames() call. */
/*: CR 11/14/91 11:28 Make_master() allocates MASTER, split off free_master().*/
/*: CR 11/18/91 17:48 Correct make_master() call. */
/*: CR 11/21/91 18:47 Load & Store userlist, then load again. */
/*: CR 11/22/91 16:27 Use exact match with LOCALNAME for visibility flag. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: CR 12/26/91 11:42 Remove unused variables. */
/*: JX  5/21/92 14:05 Chixify. */
/*: CR 12/07/92 15:13 Add unit # arg to store_ulist(). */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CR 10/09/97 11:49 Simplify Ulist use. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"

extern union  null_t         null;
extern int                   debug;

FUNCTION  send_conf (cnum, to_chn)
   int    cnum;
   Chix   to_chn;
{
   struct    master_template  master;
   struct    namlist_t *names, *add, *p, *nlnode();
   int       item, success, error, visible;
   Chix      ctext;
   Chix      localname, truename, orighost, value;
   Chix      org_id, org_chn;
   Chix      ul0;

   ENTRY ("send_conf", "");

   localname  = chxalloc (L(100), THIN, "sendconf localname");
   truename   = chxalloc (L(100), THIN, "sendconf truename");
   orighost   = chxalloc (L(100), THIN, "sendconf orighost");
   value      = chxalloc (L(40),  THIN, "sendconf value");
   org_id     = chxalloc (L(40),  THIN, "sendconf org_id");
   org_chn    = chxalloc (L(40),  THIN, "sendconf org_chn");
   ctext      = chxalloc (L(500), THIN, "sendconf ctext");
   success    = 0;

   /*** Allocate space for MASTER record of items and responses. */
   make_master (&master, MAXITEMS);

   /*** Load the master item/response record for conference CNUM. */
   success = 1;
   if (NOT srvldmas (1, &master, cnum)) { free_master (&master);   FAIL; }

   /*** Send each (non-deleted!) item in the conference. */
   for (item=1;   item<=master.items;   ++item) {
      if (master.responses[item] < 0)  continue;
      send_item (cnum, item, to_chn, &master);
   }

   /*** Send all the OSUBJECTs to TO_CHN. */
   send_subjects (cnum, to_chn, &master);

   /*** Send CUSTOMIZE ADD status for this conference. */
   cusxattr (cnum, CUS_ADD, (master.noenter == 0), to_chn, &error);

   /*** Send CUSTOMIZE CHANGE status for this conference. */
   cusxattr (cnum, CUS_CHANGE, (master.nochange == 0), to_chn, 
             &error);

   /*** Send the current primary organizer id and host. */
   chxbreak (master.organizerid, org_id, org_chn, CQ("@"));
   if (EMPTYCHX(org_chn))  getmyhost (org_chn);
   cusxporg (cnum, org_id, org_chn, to_chn, &error);

   /*** Free the MASTER memory */
   free_master (&master);

   /*** Send the introduction and greeting text. */
   if (unit_lock (XCGR, READ, L(cnum), L(0), L(0), L(0), null.chx)) {
      chixfile   (XCGR, ctext, "");
      unit_unlk  (XCGR);
      cusxtext   (cnum, CUS_GREET, ctext, to_chn, &error);
   }
   if (unit_lock (XCIN, READ, L(cnum), L(0), L(0), L(0), null.chx)) {
      chixfile   (XCIN, ctext, "");
      unit_unlk  (XCIN);
      cusxtext   (cnum, CUS_INTRO, ctext, to_chn, &error);
   }

   /*** Send the CUSTOMIZE VISIBILITY status for this conference. 
   /    Map the CNUM to its LOCALNAME, then pull the LOCALNAME
   /    entry from the confs001 file.  Decide from that if the
   /    status is visible or invisible, and send the status. */
   if (get_trueconf (cnum, localname, truename, orighost)) {

      /*** Get the current entry for conference LOCALNAME.  Get the list of
      /    matches for LOCALNAME, then look for an exact match.  Pull the
      /    visibility indicator off of that match. */
      names = nlnode (1);
      nladd  (names, localname, 0);
      matchnames (XSCD, XSCN, 0, null.chx, names, AND, &add, NOPUNCT);
      nlfree (names);
      for (p=add->next;   p!=null.nl;   p = p->next) {
         chxtoken  (value, null.chx, 1, p->str);
         if (chxeq (value, localname)) {
            chxtoken  (value, null.chx, 2, p->str);
            visible = (chxvalue(value,0L) == 'o' ? 1 : 0);
            cusxattr  (cnum, CUS_VISIBILITY, visible, to_chn, 
                       &error);
            break;
         }
      }
      nlfree (add);
   }

   /*** Send the userlist to TO_CHN. */
   ul0 = chxalloc (L(200), THIN, "sendconf ul0");
   load_ulist  (XCUS, cnum, ul0, value, &error);
   cusxusers  (cnum, ul0, to_chn, &error);
   chxfree (ul0);

 done:

   chxfree ( localname );
   chxfree ( truename );
   chxfree ( orighost );
   chxfree ( value );
   chxfree ( org_id );
   chxfree ( org_chn );
   chxfree ( ctext );

   RETURN  (success);
}
