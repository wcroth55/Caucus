/*** CHG_DSUBJECT.   Create, change, or delete a SUBJECT in the local database.
/
/    ok = chg_dsubject (action, cnum, cname, id, title, ctext, error);
/
/    Parameters:
/       int    ok;        // Success?
/       int    action;    // XT_CHANGE/XT_NEW/XT_DELETE
/       int    cnum;      // conference number
/       Chix   cname;     // Name of Conf
/       Chix   id;        // PSUBJECT: id of owner.  OSUBJECT: empty
/       Chix   title;     // name of subject
/       Chix   ctext;     // text of changes to subject
/       int   *error;     // return error code
/
/    Purpose:
/       Chg_dsubject() is the one true way of creating, changing, or
/       deleting a SUBJECT category in the local database.  All Caucus
/       and CaucusLink code that works with subjects must call this function.
/
/    How it works:
/       TITLE is the complete name of the subject.
/       CNUM  is the conference name for the subject.
/       ID    is the owner userid of a PSUBJECT.  An empty ID => OSUBJECT.
/       ACTION controls what is done to the subject:
/          XT_NEW       creates a new subject
/          XT_CHANGE    applies the change in CTEXT
/          XT_DELETE    deletes the subject
/
/    Returns: 1 on success, sets ERROR to 0.
/             0 on any error condition, see below.
/
/    Error Conditions: sets ERROR accordingly:
/       DBADACT      Bad ACTION code.
/       DOEXIST      Creating a subject that already exists.
/       DSUBSET      Creating a subject, TITLE is proper subset of another.
/       DNOSUBJ      Deleting a subject that doesn't exist
/       D2SUBJ       TITLE matches multiple subjects
/       DSUBPUNC     title is all punctuation!
/       DBADDB       Database errors
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions, XFU
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/chgdsubj.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/17/91 15:06 New function. */
/*: JV  8/12/91 15:15 Add globals confnum & confname TEMPORARILY! */
/*: CR 11/07/91 13:39 Remove confnum & confname globals. */
/*: JX  7/14/92 10:12 Chixify. */
/*: CL 12/11/92 15:52 Long sysmem arg. */
/*: CN  5/15/93 17:11 Use new item selection codes. */
/*: CR  7/05/94 16:33 Add handling for DSUBPUNC. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"
#include "derr.h"

extern short confnum;

FUNCTION  chg_dsubject (action, cnum, confname, id, title, ctext, error)
   int    action, cnum;
   Chix   confname, id, title;
   Chix   ctext;
   int   *error;
{
   int    uad, uan, usd, usn, usq, ust, subnum, success;
   short *range;
   Chix   tname, o_host;
   char  *sysmem();

   ENTRY ("chg_dsubject", "");

   tname  = chxalloc (L(20), THIN, "chgdsub tname");
   o_host = chxalloc (L(20), THIN, "chgdsub ohost");

   confnum = cnum;
   get_trueconf (confnum, confname, tname, o_host);

   /*** Figure unit code numbers for OSUBJECT or PSUBJECT. */
   if (NOT EMPTYCHX (id)) {
      uad = XUOD;   uan = XUON;       usd = XUND;   usn = XUNN;
      usq = XUSD;   ust = XUSF;
   }
   else {
      uad = XCOD;   uan = XCON;       usd = XCND;   usn = XCNN;
      usq = XCSD;   ust = XCSF;
   }

   /*** Check action code, and get subject number if appropriate. */
   *error = NOERR;
   if      (action == XT_NEW) ;
   else if (action == XT_CHANGE  ||  action == XT_DELETE) {
      /*** Find the number of the subject that uniquely matches TITLE. */
      subnum = submatch (usd,usn, usq,ust, id, cnum,confname, title);
      if (subnum  < 0)   *error = D2SUBJ;
      if (subnum == 0)   *error = DNOSUBJ;
   }
   else *error = DBADACT;

   if (*error != NOERR)  FAIL;

   /*** Actually create, delete, or modify subject category. */

   if (action == XT_NEW) {
      chxpunct     (tname, title);
      chxsimplify  (tname);
      if (EMPTYCHX (tname)) *error = DSUBPUNC;
      else                  *error = subcreate (cnum,confname, uad,uan, usq,ust,
                                                usd,usn, title, id);
   }

   if (action == XT_DELETE) {
      if (NOT  subdelete (cnum,confname, uad,uan, usq,ust, usd,usn, id, subnum))
                         *error = DBADDB;
   }

   if (action == XT_CHANGE) {

      /*** Get & clear an item index RANGE. */
      if ( (range = (short *) sysmem (sizeof(short) * L(MAXITEMS+1)), "chgdsubj") == NULL)
         { *error = DNOMEM;   FAIL; }

      /*** Scan and process additions to subject. */
      if (scan_subject (ctext, '+', range, cnum)) {
         if (NOT submod (cnum, usq, ust, id, subnum, range, 0))
            *error = DBADDB;
      }

      /*** Scan and process removals from subject. */
      if (scan_subject (ctext, '-', range, cnum)) {
         if (NOT submod (cnum, usq, ust, id, subnum, range, 1))
            *error = DBADDB;
      }
      sysfree ( (char *) range);
   }
   success = *error==NOERR;

done:
   chxfree (tname);
   chxfree (o_host);

   RETURN (success);
}
