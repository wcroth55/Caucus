
/*** MATCHDATE.   Search namelist file for a datemap entry.
/
/    n = matchdate (ud, uf, cnum, item, date, match);
/
/    Parameters:
/       int       n;         (success?)
/       int       ud, uf;    (namelist file unit numbers)
/       int       cnum;      (conference number)
/       int4      item;      (item number, if relevant)
/       int4      date;      (date to be matched against)
/       int4     *match;     (value part of matched date)
/
/    Purpose:
/       Find the lowest "value" in the namelist datemap files (specified
/       by unit numbers UD and UF, conference CNUM, item ITEM) that has 
/       a date greater than or equal to DATE.  Return the value in MATCH.
/
/    How it works:
/       Each datemap namelist entry has a date and a "value", typically
/       a response or item number.  Matchdate() binary searches the
/       namelist file(s) until it finds the earliest entry >= DATE.
/
/       It then scans from that entry to the end of the namelist file(s),
/       and returns the lowest "value" found, in MATCH.
/
/       If no match at all was found, MATCH is set to -1.
/
/    Returns:
/       0 on error
/       1 if successful
/
/    Error Conditions:
/       Error if some namelist partfiles cannot be found.
/       Internal errors in nlnode(), nladd(), etc. functions that allocate
/          memory may also cause error return.
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  "show since" code
/
/    Operating system dependencies: None.
/
/    Known bugs:
/       May wish to use its own private textbuf, rather than the
/       global tbuf.
/
/    Home:  people/matchdat.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  4/29/92  0:19 New function. */
/*: CN  5/15/93 23:22 Set MATCH to -1 if no matches. */

#include <stdio.h>
#include "caucus.h"

#define  NC         nullchix
#define  HIGHBOUND  L(20000)

extern Textbuf          tbuf;
extern union  null_t    null;

FUNCTION  matchdate (ud, uf, cnum, item, date, match)
   int    ud, uf, cnum;
   int4   item;
   int4   date;
   int4  *match;
{
   Namelist namdir, partptr, nlnode(), whatnm();
   int      success;
   int4     len;
   int      part;
   short    lo, md, hi, result;
   char     datestr[40];
   Chix     str;
   int4     matchdl();

   ENTRY ("matchdate", "");

   /*** Get the relevant namepart directory. */
   nlclear();
   if (NOT unit_lock (ud, READ, L(cnum), item, L(0), L(0), nullchix))  RETURN(0);
   namdir   = nlnode(4);
   str      = chxalloc (L(20), THIN, "matchdate str");
   if (NOT loadnmdir (ud, namdir))  FAIL;

   /*** Prepare the string form of DATE. */
   sprintf (datestr, "%010d", date);
   chxcpy  (str, CQ(datestr));
   len = 10;

   /*** Start scanning by figuring out which partfile must contain
   /    the desired date. */
   if ( (partptr = whatnm (str, namdir)) == null.nl)   FAIL;
   if ( (part    = partptr->num)         == 0)         FAIL;

   /*** Load that partfile. */
   if (NOT unit_lock (uf, READ, L(cnum), L(part), item, L(0), nullchix)) FAIL;
   if (NOT loadnpart (uf, part, (Textbuf) NULL))  { unit_unlk (uf);    FAIL; }
   unit_unlk (uf);

   /*** Binary search that partfile for DATE.  MD will point to an
   /    entry >= DATE. */
   for (lo=md=0, hi=tbuf->l_used-1, result=1;   hi > lo+1; ) {
      md = (hi + lo) / 2;
      result = chxcompare (tbuf->line[md], str, len);
      if      (result > 0)  hi = md;
      else if (result < 0)  lo = md;
      else                  break;
   }
   if (hi == lo+1)  md = hi;

   /*** Backup if need be, to make MD point to the *first* entry >= DATE. */
   while (md > 1  &&  
          chxcompare (tbuf->line[md-1], str, len) == 0) --md;

   /*** Scan through the data to find the lowest value. */
   *match = matchdl (md, tbuf, HIGHBOUND);


   /*** Now scan through to the end of this set of namelist files to find 
   /    the entry with the lowest value. */
   for (partptr=partptr->next;   partptr!=null.nl;    partptr=partptr->next) {
      if ( (part = partptr->num) == 0)  break;

      if (NOT unit_lock (uf, READ, L(cnum), L(part), item, L(0), NC))      break;
      if (NOT loadnpart (uf, part, (Textbuf) NULL))   { unit_unlk (uf);  break; }
      unit_unlk (uf);
   
      /*** Scan through the data to find the lowest value. */
      *match = matchdl (0, tbuf, *match);
   }
   SUCCEED;

 done:
   unit_unlk (ud);
   nlfree    (namdir);
   chxfree   (str);
   
   if (*match == HIGHBOUND) *match = -1;
   RETURN ( success );
}

FUNCTION   int4 matchdl (md, tbuf, lowest)
   int     md;
   Textbuf tbuf;
   int4    lowest;
{
   int4    pos, thisval;
   Chix    thisline;

   for (;   md < tbuf->l_used;   ++md) {
      thisline = tbuf->line[md];
      pos      = L(10);
      thisval  = chxint4 (thisline, &pos);
      if (pos > 10  &&  thisval < lowest)  lowest = thisval;
   }

   return (lowest);
}
