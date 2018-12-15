/*** RMCONF.  Remove a Caucus conference.
/
/   int ok = rmconf (confnum)
/
/   Parameters:
/      int  confnum;   (Local Conference Number of conference)
/
/   Purpose:
/      This is the analogue of MAKECONF().  RMCONF() removes all traces
/      of a conference, including all of the conference DB, the Users'
/      Participation Files, and the conference entry(ies) in XSCN.
/
/   How it Works:
/
/   Returns:
/      1 on success
/      0 on failure
/
/   Error Conditions:
/      Couldn't lock or load member directory.
/
/   Related Functions: make_conf
/
/   Called by: cremove, reqreport
/
/   Home: conf/rmconf.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  1/01/92 11:08 Source code master starting point */
/*: CR  9/25/92 17:12 If get_trueconf fails, use confname. */
/*: DE  5/26/92 12:45 Fixed inconsistant args */
/*: CI 10/07/92 11:10 Integration Clean-up. */
/*: CI 10/07/92 17:51 Chix Integration. */
/*: CP  5/06/92 12:41 Add L(0) as 'item' arg to modnames() call. */
/*: CP  8/01/93 14:32 Add 'name' arg to make_textbuf() call. */
/*: JV  9/26/93 13:13 Add ENTRY. */
/*: JZ 10/01/93 11:27 Remove file attachment directory. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "done.h"
#include "textbuf.h"

extern Chix                    confname;
extern union  null_t           null;

FUNCTION  rmconf (int confnum)
{
   char   attdir[120];
   struct namlist_t *namdir, *this, *nlnode();
   short   i, ok, success;
   Chix    id, tname, lname, orig_host;
   Textbuf tbuf, make_textbuf();

   ENTRY ("rmconf", "");

   id          = chxalloc (L(40),  THIN, "id");
   tname       = chxalloc (L(100), THIN, "tname");
   lname       = chxalloc (L(100), THIN, "lname");
   orig_host   = chxalloc (L(100), THIN, "orig_host");

   /*** Get the TrueConf name, if it exists. */
   if (NOT get_trueconf (confnum, lname, tname, orig_host)) {
      chxcpy (lname, confname); 
   }
   if (NOT EMPTYCHX(tname)) {
      chxcat (tname, CQ("@"));
      chxcat (tname, orig_host);
   }

   /*** Go through the list of members of this conference.  For each
   /    one, delete the participation file. */

   /*** Load each namepart file in turn, and delete XUPA file for
   /    each user. */
/*
         if (unit_lock (XUPA, WRITE, L(confnum), L(0), L(0), L(0), id)) {
             unit_kill (XUPA);
             unit_unlk (XUPA);
         }
*/

   /*** Finally, delete the conference and attachment directories. */
   sysrmconf  (confnum);
   unit_name  (attdir, XACD, L(confnum), L(0), L(0), L(0), nullchix);
   sysrmdir   (attdir);

   /*** Clear the trueconf cache. */
   get_trueconf (-1, nullchix, nullchix, nullchix);

   SUCCEED;

done:
   chxfree (id);
   chxfree (tname);
   chxfree (lname);
   chxfree (orig_host);

   RETURN (success);
}
