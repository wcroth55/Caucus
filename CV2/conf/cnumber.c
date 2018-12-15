/*** CNUMBER.  Find the number of a given conference NAME.
/
/   int cnumber (name, which, openval)
/
/   Parameters:
/      Chix   name;     (conference name, supplied *and* returned)
/      int    which;    (TRUECONF or LOCALCONF desired?)
/      int   *openval;  (returned value: conference open or closed?)
/
/    Purpose:
/       Find the number of a conference NAME.  Fill out the full conference
/       name.  Get the open/closed status of the conference.
/
/    How it works:
/       CNUMBER takes NAME, an initial substring of a conference name,
/       and looks for a match in the system XSCD/XSCN conference name tables.
/       If it finds a unique match, it copies the full name into NAME.
/   
/       CNUMBER allows you to specify whether you're giving the "local" name
/       for the conf or the "true conf name", or you'll take either if unique.
/       To specify a Local confname, WHICH must be LOCALCONF.  To specify a
/       True confname, WHICH must be TRUECONF, and NAME must be in the form:
/       name@origchname.
/
/       Format of files (XSCN) is:
/       !
/       !confs o#002#L
/       demonstration o#001#L
/       zoo@orighostCHN o#002#T
/       ~~~~~~~~~~~~~~~~~~~~~~~~
/
/       where L specifies that it's a Local confname, and T a True confname.
/
/       Sets OPENVAL to 1 if an open (visible) conference, else 0.
/
/    Returns:
/       -1   if no matches at all.
/        0   some matches, none exact.
/        n   conference number, if only match.
/
/    Error Conditions:
/  
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  conf/cnumber.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: JV  8/21/89 18:54 Return -1 if no match, 0 if ambiguous matches. */
/*: CR  8/28/90 14:32 Replace nlnames() call with nlnode()/nladd().  */
/*: CR  3/29/91 17:29 Use AND/OR code in matchnames() call. */
/*: JV  7/16/91 14:02 Add more header. */
/*: CR  8/02/91 14:00 Add punctuation argument to matchnames() call. */
/*: JV 10/28/91 14:23 Add arg WHICH, true vs. local confnames. */
/*: JV 10/31/91 11:08 Remove EITHERCONF, move L,T.. */
/*: JV 11/11/91 13:58 No code chg, but TrueConf-->TConfName@OrigCHN. */
/*: JV 11/13/91 22:05 Add confnum to matchnames call. */
/*: WC 11/29/91 21:07 Add openval argument, spiff up header. */
/*: WC 12/01/91 13:22 Move openval setting to bottom. */
/*: JV 12/03/91 12:33 Fix header. */
/*: JX  3/04/92 12:08 Chixify. */
/*: DE  5/26/92 13:50 Fix inconsistant args */
/*: CL 12/11/92 13:47 Long Chxvalue N. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"

extern union null_t null;

FUNCTION  cnumber (name, which, openval)
   Chix   name;
   int    which;
   int   *openval;
{
   struct namlist_t *list, *owners, *this, *maybe, *nlnode();
   Chix   fullname, open, number, cname;
   static char   types[2] = {'T', 'L'};
   Chix   type, temp1, temp2;
   short  ok, size;
   int    success;
   int4   cnum;

   ENTRY ("cnumber", "");

   type     = chxalloc (L(20), THIN, "type");
   open     = chxalloc (L(20), THIN, "open");
   temp1    = chxalloc (L(80), THIN, "temp1");
   temp2    = chxalloc (L(80), THIN, "temp2");
   cname    = chxalloc (L(80), THIN, "cname");
   number   = chxalloc (L(20), THIN, "number");
   fullname = chxalloc (L(80), THIN, "fullname");

   /*** Get namelist of conferences matching NAME. */
   list = nlnode (1);
   nladd (list, name, 0);
   ok = matchnames (XSCD, XSCN, 0, null.chx, list, AND, &owners, NOPUNCT);
   nlfree (list);
   if (ok != 1)  {success = -1; goto done; }

   /*** If no matches, return and complain. */
   if ( (size = nlsize (owners)) == 0) {
      nlfree (owners);
      success = -1;
      goto done;
   }

   /*** Scan the list of matches and find the one exact match.
   /    Allow both "old" (pre-CaucusLink) and new confsNNN formats. */
   cnum = 0;
   maybe = null.nl;
   for (this=owners->next;   this!=null.nl;   this = this->next) {
      chxtoken (fullname, null.chx, 1, this->str);
      chxtoken (temp1,    null.chx, 2, this->str);
      chxbreak (temp1, open,   temp2, CQ("#"));
      chxbreak (temp2, number, type,  CQ("#"));
      if (NOT EMPTYCHX(type)  &&  chxvalue (type, L(0)) != L(types[which])) {
         size -= 1;
         continue;          /* Wrong type of name. */
      }
      if (size > 1) {       /* Right type, but only substring? */
         maybe = this;
         if (NOT chxeq (name, fullname))   continue;
      }
      chxcpy   (cname, fullname);
      chxnum   (number, &cnum);
   }

   if (size == 1  &&  maybe != null.nl) {     /* Now check to see if only */
      chxtoken (fullname, null.chx, 1, maybe->str);  /* one substr match. */
      chxtoken (temp1, null.chx, 2, maybe->str);
      chxbreak (temp1, open, temp2, CQ("#"));
      chxbreak (temp2, number, type, CQ("#"));
      chxcpy (cname, fullname);
      chxnum (number, &cnum);
   }

  *openval = (chxvalue (open, L(0)) == L('o'));
   nlfree (owners);
   if (cnum) chxcpy (name, cname);
   success = cnum;

done:
   chxfree (type);
   chxfree (temp1);
   chxfree (temp2);
   chxfree (open);
   chxfree (cname);
   chxfree (number);
   chxfree (fullname);
   RETURN (success);
}
