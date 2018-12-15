/*** MAKE_CONF.  Create the database structures that "are" a conference.
/
/    ok = make_conf (confname, orgname, listed, library, orgs, error);
/
/    Parameters:
/       Chix  confname;          Name of the conference. Must be unique.
/       Chix  orgname;           Name of the primary organizer.
/       int   listed;            Conf is Open=1, Closed=0.
/       int   library;           New Conf=0, New File Library=1
/       Chix  orgs;              list of other organizer userids.
/       int  *error;             Place for storing error code.
/
/    Purpose:
/       Both caumnt_x and netmaint_x provide a method for the Caucus manager
/       to create a new conference or file library.  This function takes the
/       information they get/provide and create a new, empty conference or 
/       file library based on that information.
/
/    How it works:
/ 
/       Note: ORGNAME may be a null string, but cannot point to garbage.
/             MAKE_CONF assumes that oknewconf() has been called.
/
/    Returns: new conf # on success.
/             0 on failure.
/
/    Error Conditions:
/ 
/    Side effects:
/
/    Called by: 
/
/    Related functions:
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  xmaint/makeconf.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV 11/06/91 09:24 New function. */
/*: CR 11/18/91 17:20 Add CNUM argument to modnames() call. */
/*: WC 11/29/91 21:14 Add 3rd arg to cnumber(). */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: JV  1/03/92 15:10 Declare strcpy. */
/*: CR  7/26/92 15:45 Use cst_Tchgstat to control conf chg perms. */
/*: JX  3/09/92 17:45 Chixify. */
/*: DE  5/26/92 13:04 Fixed inconsistant args */
/*: JX  9/22/92 14:48 Fixify. */
/*: CI 10/07/92 17:44 Chix Integration. */
/*: JV  5/24/93 19:11 Create XACD (Attachment directory). */
/*: CP  5/06/92 12:00 Add initialization of XCED/XCEN. */
/*: CP  5/06/92 12:41 Add L(0) as 'item' arg to modnames() call. */
/*: CN  5/16/93 13:27 Add 'cnum' arg to initnmdir(). */
/*: CP  7/08/93 16:05 Integrate 2.5 changes. */
/*: JZ  9/28/93 14:52 Add support for file libraries. */
/*: JZ 10/18/93 10:48 Fix chxformat call. */
/*: CR  8/04/99 23:55 Match any "!delxxx" for reuse. */
/*: CR  9/09/05 Major rehack for C5. */
/*: CR 10/12/05 Remove mdwrite(). */
/*: CR 09/22/09 Remove sql_sequence(grouprules), it's auto-increment now. */
/*: CR 09/28/09 Call privValue() to get permission for initial organizers. */

#include <stdio.h>
#include "unitcode.h"
#include "handicap.h"
#include "derr.h"
#include "conf.h"
#include "chixuse.h"
#include "namelist.h"
#include "null.h"
#include "done.h"

extern union null_t     null;
extern short            confnum;

FUNCTION  make_conf (Chix confname, Chix orgname, int listed, int library, 
                     Chix orgs, int *error) {
   char   name[160], temp[300], nothing[10];
   char   numbuf[100];
   Chix   str, new, cnum;
   int    v, success, reuse, ok, rc, pos;

   ENTRY ("make_conf", "");

   str      = chxalloc (L(80), THIN, "str");
   new      = chxalloc (L(80), THIN, "new");
   cnum     = chxalloc (L(80), THIN, "cnum");

   /*** Increment the # of confs, and get a new one. */
   sql_sequence ("confs", cnum);
   confnum = atoi (ascquick(cnum));

   /*** First try and make the conference directory... */
   if (NOT sysmkconf (confnum)) {
      *error = DMKDIR;
      FAIL;
   }

   /*** Put in the entry for the new conference. */
   sprintf (temp, "INSERT INTO confs (cnum, name) VALUES ('%d', '%s')",
                  confnum, ascquick(confname));
   sql_quick_limited (0, temp, nothing, 9, 0);

   /*** Insert the primary organizer rule into the CONF group. */
   sprintf (temp, 
      "INSERT INTO grouprules (grkey, owner, name, userid, access) VALUES "
      "(NULL, 'CONF', '%d', '%s', 100)", confnum, ascquick(orgname));
   rc = sql_quick_limited (0, temp, nothing, 9, 0);

   /*** Insert the other organizer(s) rule into the CONF group. */
   for (pos=0;   chxnextword (str, orgs, &pos) >= 0;  ) {
      sprintf (temp, 
         "INSERT INTO grouprules (grkey, owner, name, userid, access) VALUES "
         "(NULL, 'CONF', '%d', '%s', %d)", confnum, ascquick(str),
         privValue("organizer"));
      rc = sql_quick_limited (0, temp, nothing, 9, 0);
   }

   /*** Now that all the grouprules are built, update the groups table. */
   chxcpy (new, CQ("CONF "));
   chxcat (new, cnum);
   quoter (str, "quote", new);
   group_update (str);


   /*** The rest of the code assumes that since we could lock & write
   /    one file, we can do the rest OK.  Probably not the greatest
   /    idea, but it has worked historically. */

   /*** Write a MASTER file that implies "no items". */
   unit_lock (XCMA, WRITE, L(confnum), L(0), L(0), L(0), nullchix);
   unit_make (XCMA);
   if (EMPTYCHX (orgname)) unit_write (XCMA, CQ("---@---"));
   else                    unit_write (XCMA, orgname);
   unit_write (XCMA, CQ("\n"));
   unit_write (XCMA, CQ("0 0 0 0\n"));

   unit_close (XCMA);
   unit_unlk  (XCMA);
 
   /*** Create the conference membership directory. */
   initnmdir (XCMD, XCMN, confnum, nullchix, L(0));
 
   /*** Create the conference title partfile directory. */
   initnmdir (XCTD, XCTN, confnum, nullchix, L(0));
 
   /*** Create the conference author-item partfile directory. */
   initnmdir (XCAD, XCAN, confnum, nullchix, L(0));
 
   /*** Create the conference Subject namelist files. */
   initnmdir (XCND, XCNN, confnum, nullchix, L(0));
 
   /*** Create the conference Alphabetized subject files. */
   initnmdir (XCOD, XCON, confnum, nullchix, L(0));
 
   /*** Create the "time item entered" namelist file. */
   initnmdir (XCED, XCEN, confnum, nullchix, L(0));

   /*** Create the conference Subject text file. */
   unit_lock (XCSF, WRITE, L(confnum), L(0), L(0), L(0), nullchix);
   unit_make (XCSF);
   unit_write(XCSF, CQ("#0 0\n"));
   unit_close(XCSF);
   unit_unlk (XCSF);
 
   /*** Create the TrueConf file. */
   unit_lock  (XCTC, WRITE, L(confnum), L(0), L(0), L(0), nullchix);
   unit_make  (XCTC);
   unit_write (XCTC, confname);   unit_write (XCTC, CQ("\n"));
   unit_close (XCTC);  unit_unlk (XCTC);

   success = confnum;

done:
   chxfree (str);
   chxfree (new);
   RETURN  (success);
}
