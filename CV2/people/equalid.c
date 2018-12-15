
/*** EQUAL_ID.  Are two userid's identical?
/
/    ok = equal_id (a, b, wild);
/
/    Parameters:
/       int    ok;      (Yes or No?)
/       Chix   a;       (first userid)
/       Chix   b;       (second userid)
/       Chix   wild;    (wildcard character)
/
/    Purpose:
/       Now that userid's sometimes contain "@host", we need an easy way
/       to compare two userids to see if they are effectively identical.
/       "Effectively identical" means that a userid *without* an "@host"
/       is treated as though it had the current hostname appended.
/
/       If WILDCARD is not null or empty, equal_id() also handles wildcard 
/       matching in both the id and host parts of a userid.  But the
/       wildcard may only appear in A, and only once in either the
/       id or the host part(s).
/
/    How it works:
/
/    Returns: 1 if the userids are effectively equal, 0 otherwise.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Caucus functions
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  people/equalid.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/19/91 14:26 New function. */
/*: CR  7/23/91 15:00 Don't do wildcard matching if WILDCARD is empty. */
/*: DE  4/10/92 11:35 Chixified */
/*: CR 10/11/92 21:41 Allow nullchix for 'wild' arg. */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: JV  9/08/93 09:51 Replace wildmatch's RETURN with return. */
/*: CR 10/12/01 16:40 Ignore @'s!  Replace with "^" for now. */
/*: CR 01/08/05 Handle "*xyz" matches. */
/*: CR 03/24/05 Fix false positive bug in wildmatch, of 'abc*x' case */

#include <stdio.h>
#include "caucus.h"

FUNCTION  equal_id (a, b, wild)
   Chix  a, b, wild;
{
   Chix   ai, bi, ah, bh;
   int    success;

   ENTRY ("equal_id", "");

   if (chxlen(a)==0  || chxlen(b)==0)   RETURN(0);

   ai       = chxalloc (L(160), THIN, "equalid ai");
   bi       = chxalloc (L(160), THIN, "equalid bi");
   ah       = chxalloc (L(160), THIN, "equalid ah");
   bh       = chxalloc (L(160), THIN, "equalid bh");
   success  = 0;

   /*** Get the id and host parts of each userid. */
   chxbreak (a, ai, ah, CQ("^"));   if (EMPTYCHX (ah))  getmyhost (ah);
   chxbreak (b, bi, bh, CQ("^"));   if (EMPTYCHX (bh))  getmyhost (bh);

   /*** Both id and host must match, either directly or via wildcards. */
   success = ( (wildmatch (ai, bi, wild)  ||  wildmatch (bi, ai, wild))
         &&    (wildmatch (ah, bh, wild)  ||  wildmatch (bh, ah, wild)) );

   chxfree ( ai );
   chxfree ( bi );
   chxfree ( ah );
   chxfree ( bh );

   RETURN ( success );
}


/*** Match x = y, allowing for a single wildcard in x. */
FUNCTION  wildmatch (x, y, wild)
   Chix  x, y, wild;
{
   int4   star, pos;
   Chix   match, tempchx;
   int    success;

   if (chxeq (x, y))                            return (1);
   if (EMPTYCHX (wild))                         return (0);
   if (chxeq (x, wild))                         return (1);
   if ( (star = chxindex (x, L(0), wild)) < 0)  return (0);

   if (star > 0  &&  chxlen(x)-1 > star)        return (0); /* abc*x */

   if (star > 0) {
      tempchx =  chxalsub (x, L(0), star);
      success = (chxindex (y, L(0), tempchx) == 0);
   }
   else {
      tempchx = chxalsub (x, L(1), 10000);
      pos = chxrevdex (y, L(10000), tempchx);
      success = (pos == chxlen(y) - chxlen(tempchx));
   }
   chxfree (tempchx);
   return  ( success );
}
