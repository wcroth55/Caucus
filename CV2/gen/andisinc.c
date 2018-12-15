
/*** AND_ISINCE.  Reduce an Rlist to items entered "since" a particular time.
/
/    result = and_isince (rl, since, cnum);
/
/    Parameters:
/       Rlist     result;      (returned "and" result)
/       Chix      since;       ("since" specification)
/       int       cnum;        (conference number)
/
/    Purpose:
/       Reduce an Rlist of items to only those items that have been
/       entered "since" a particular time.
/
/    How it works:
/       Parse the "since" date.  Call matchdate() to find the earliest
/       item MATCH "since" that date.   Return a new Rlist that contains
/       only items from RL that are >= MATCH.
/
/    Returns: new Rlist (may include empty Rlist if no items match)
/             NULL on error
/
/    Error Conditions: RL is null.
/                      Bad date format in SINCE.
/
/    Side effects: 
/
/    Related functions:
/
/    Called by:  newparse(), nr_to_rlist()
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  gen/andisinc.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  8/03/93 13:27 New function. */
/*: CK  8/07/93 18:15 Declare int4 systime(), time_of_date(). */
/*: CR 10/18/93 13:32 Add comments re Lint. */
/*: CK 11/02/93 14:27 Return error if date is not parseable. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"

#define  RLNULL     ((Rlist) NULL)

FUNCTION  Rlist and_isince (rl, since, cnum)
   Rlist  rl;
   Chix   since;
   int    cnum;
{
   Chix   rest;
   Chix   blank, empty;
   int4   date, subtract, match;
   int    ok, lobound;
   int    success;   /* used only for debugging; Lint will complain. */
   Rlist  result, tail, ptr, add_to_rlist();
   int4   systime(), time_of_date();

   ENTRY ("and_isince", "");

   /*** Check argument. */
   if (rl==RLNULL)  RETURN (RLNULL);

   /*** Get the value after the word SINCE, and remove all blanks. */
   rest  = chxalloc (L(40), THIN, "and_isince rest");
   chxtoken (nullchix, rest, 2, since);
   blank = chxquick (" ", 3);
   empty = chxquick ("",  4);
   while (chxalter (rest, L(0), blank, empty)) ;

   /*** Look for "-N" days syntax; otherwise parse full mm/dd/yy or
   /    dd-mmm-yyy date. */
   if (chxvalue (rest, L(0)) == '-') {
      chxnum (rest, &subtract);
      date = systime() + subtract * L(24) * L(3600);
   }
   else date = time_of_date (rest, nullchix);

   /*** Date of '0' means error; return a NULL result. */
   if (date == 0) { result = RLNULL;    FAIL; }

   /*** RESULT is the new Rlist we are building.  TAIL points to the
   /    last node in the Rlist.  New nodes are always added at the
   /    tail. */
   result = a_mak_rlist ("andisinc result");
   tail   = result;

   /*** Figure out the first item (MATCH) that was entered since DATE. */
   ok = matchdate (XCED, XCEN, cnum, L(0), date, &match);
   if  (NOT ok  ||  match == -1)  FAIL;

   /*** Add to RESULT only those items in RL that are MATCH or after. */
   for (ptr=rl->next;   ptr!=RLNULL;   ptr=ptr->next) {
      if (ptr->i1 < match)  continue;
      lobound = (match > ptr->i0 ? match : ptr->i0);
      tail = add_to_rlist (tail, lobound, ptr->r0, ptr->r1);
      tail->i1 = ptr->i1;
   }

done:
   chxfree (rest);
   RETURN  (result);
}
