
/*** SUBMATCH.   Find subject that uniquely matches NAME.
/
/    num = submatch (usd,usf, utd,utf, owner, cnum, cname, name);
/
/    Parameters:
/       int    num;         (returned subject number)
/       int    usd;         (subject directory unit code)
/       int    usf          (subject names files unit code)
/       int    utd;
/       int    utf;
/       Chix   owner;       (owner id, if PSUBJECT, else ignored)
/       int    cnum;        (conference number)
/       Chix   cname;       (conference name)
/       Chix   name;        (supplied & returned subject name)
/
/    Purpose:
/       Find an OSUBJECT or PSUBJECT category that matches NAME.
/       Return the subject number, and full name.
/
/    How it works:
/       USD, USF, UTD, UTF, OWNER, and CNUM specify the OSUBJECT
/       data files for a particular conference, or the PSUBJECT
/       data files for a particular user.
/
/       Submatch() searches those files for a subject NAME in
/       conference CNUM (CNAME).  If it finds a subject that
/       uniquely matches NAME, it returns the index number of 
/       that subject, and sets NAME to the full name of the
/       subject.  If no subject is found, it  returns 0.  If 
/       multiple (ambiguous) matches are found, it returns -1.
/
/    Returns: subject number on success
/              0 if no match
/             -1 on too many matches
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  subj/submatch.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:56 Source code master starting point */
/*: CR  3/29/91 17:31 Use AND/OR code for matchnames() call. */
/*: CR  8/02/91 14:00 Add punctuation argument to matchnames() call. */
/*: CR 10/29/91 11:45 Add confnum arg to getline2() call. */
/*: CR 11/07/91 13:33 Supply confnum, confname as arguments. */
/*: CR 11/07/91 14:18 Add confnum arg to matchnames() call. */
/*: CR 11/18/91 11:17 New header, return full NAME. */
/*: CR 11/18/91 16:04 Fix the fix for full NAME.. */
/*: DE  3/24/92 12:11 Chixified */
/*: DE  4/20/92 12:49 Added args to getline2 */
/*: CR 10/11/92 21:58 Remove CQ from nlnames(). */
/*: CR  2/12/93 12:50 Fix header comments. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

FUNCTION  submatch (usd,usf, utd,utf, owner, cnum, cname, name)
   int    usd,usf, utd,utf, cnum;
   Chix   owner, name, cname;
{
   struct namlist_t *names, *matches, *this, *nlnames();
   int4   subnum;
   int    success;
   short  subject, ok;
   Chix   str, cnf, fullname;
   Chix   s, getline2();

   ENTRY ("submatch", "");

   str      = chxalloc (L(80),  THIN, "submatch str");
   cnf      = chxalloc (L(40),  THIN, "submatch cnf");
   fullname = chxalloc (L(160), THIN, "submatch fullname");
   success  = 0;

   /*** Get the list of subject names that match NAME. */
   names = nlnames (name, null.chx);
   ok = matchnames (usd, usf, cnum, owner, names, AND, &matches, NOPUNCT);
   nlfree (names);
   if (NOT ok)  FAIL;

   /*** For each subject name, make sure it's in this conference.  If
   /    we find more than one subject, complain. */
   subject = -1;
   for (this=matches->next;   this!=null.nl;   this=this->next) {
      chxtoken (str, null.chx, 2, this->str);
      chxnum   (str, &subnum);

      /*** See if that subject is in this conference. */
      if ( (s = getline2 (utd, utf, cnum, owner, 0, (int) subnum, 0, 
               (Textbuf) NULL))
              == null.chx)
         continue;
      chxtoken (cnf, null.chx, 2, s);
      if (NOT chxeq (cnf, cname))  continue;
      if ( (s = getline2 (utd, utf, cnum, owner, 0, (int) subnum, 1, 
                (Textbuf) NULL))
              == null.chx)
         continue;
      chxcpy (fullname, s);

      if (subject >= 0) {
         nlfree (matches);
         DONE(-1);
      }
      subject = (int) subnum;
   }
   nlfree (matches);

   if (subject > 0) {
      chxcpy (name, fullname);
      DONE(subject);
   }

done:
   chxfree (str);
   chxfree (cnf);
   chxfree (fullname);

   RETURN (success);
}
