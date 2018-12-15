
/*** AND_RSINCE.  Reduce an Rlist to responses added "since" a particular time.
/
/    result = and_rsince (rl, since, cnum);
/
/    Parameters:
/       Rlist     result;      (returned "and" result)
/       Chix      since;       ("since" specification)
/       int       cnum;        (conference number)
/
/    Purpose:
/       Reduce an Rlist to only those responses that have been
/       entered "since" a particular time.
/
/    How it works:
/       Parse the "since" date.  For each item in RL, call matchdate()
/       to find the earliest response on that item, and add the
/       appropriate response range to a new Rlist.  Return that Rlist.
/
/    Returns: new Rlist (may include empty Rlist if no responses match)
/             NULL on error
/
/    Error Conditions: RL is null.  
/                      Bad format for date in SINCE.
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
/    Home:  gen/andrsinc.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  8/03/93 13:27 New function. */
/*: CK  8/07/93 18:15 Declare int4 systime(), time_of_date(). */
/*: CP  8/10/93 11:26 Add test for <CANCEL>. */
/*: CR 10/18/93 13:34 Remove unused 'success', 'done'. */
/*: CR 10/22/93 12:50 Handle r0 of -1 properly. */
/*: CK 11/02/93 14:27 Return error if date is not parseable. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"

#define  RLNULL     ((Rlist) NULL)

FUNCTION  Rlist and_rsince (rl, since, cnum)
   Rlist  rl;
   Chix   since;
   int    cnum;
{
   Chix   rest;
   Chix   blank, empty;
   int4   date, subtract, match;
   int    ok, item, resp, success;
   Rlist  result, tail, ptr, add_to_rlist();
   int4   systime(), time_of_date();

   ENTRY ("and_rsince", "");

   /*** Check argument. */
   if (rl==RLNULL)  RETURN (RLNULL);

   /*** Get the value after the word SINCE, and remove all blanks. */
   rest  = chxalloc (L(40), THIN, "and_rsince rest");
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
   result = a_mak_rlist ("andrsinc result");
   tail   = result;

   /*** For each item in RL... */
   for (ptr=rl->next;   ptr!=RLNULL;   ptr=ptr->next) {
      for (item=ptr->i0;   item<=ptr->i1;   ++item) {
         if (sysbrktest())  break;

         /*** ...find the earliest MATCHing response entered since DATE. */
         ok = matchdate (XCFD, XCFN, cnum, L(item), date, &match);
         if (NOT ok  ||  match == -1)              continue;
         if (ptr->r1 != -1  &&  match > ptr->r1)   continue;

         /*** Add the appropriate response range to the new Rlist. */
         if (ptr->r0 == -1)  resp = -1;
         else                resp = (match > ptr->r0 ? match : ptr->r0);
         tail = add_to_rlist (tail, item, resp, ptr->r1);
      }
   }

done:
   chxfree (rest);
   RETURN  (result);
}
