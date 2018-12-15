
/*** CHXBREAK.  "Break" chix A around PAT, yielding FORE and AFT.
/
/    ok = chxbreak (a, fore, aft, pat);
/
/    Parameters:
/       int   ok;    (1 ==> success, 0 ==> failure)
/       Chix  a;     (search in this chix...)
/       Chix  pat;   (...for this chix...)
/       Chix  fore;  (...and put everything before PAT here...)
/       Chix  aft;   (...and everything after PAT here.)
/
/    Purpose:
/       Break A around PAT.  Find PAT in A, and put everything to the
/       left of PAT in FORE.  Put everything after PAT in AFT.
/       If PAT is not found, copy all of A into FORE, and clear AFT.
/
/    How it works:
/       Note that, as usual, PAT may be found in A, even if they have
/       different types, so int4 as their corresponding characters match
/       after promotion.
/
/    Returns: 1 on success (PAT found), 0 otherwise.
/       Fails if A OR PAT or B are empty strings.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  application
/
/    Operating system dependencies:
/       Uses sysfree().
/
/    Known bugs:      none
/
/    Home:  chx/chxbreak.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/19/91 15:08 New function. */
/*: CR 10/18/91 15:00 Use POS argument in chxindex() call. */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: OQ 02/09/98 replace calls to chxcheck by inline commands */
#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxbreak (a, fore, aft, pat)
   Chix   a, fore, aft, pat;
{
   int4   i, patlen;

   ENTRY ("chxbreak", "");

   /*** Input validation. */
   /*if (NOT chxcheck (a,   "chxbreak")  ||  NOT chxcheck (fore, "chxbreak")  ||
       NOT chxcheck (aft, "chxbreak")  ||  NOT chxcheck (pat,  "chxbreak"))
      RETURN (0);*/

   if (a   == nullchix || fore == nullchix  || aft == nullchix  ||  
       pat == nullchix)
      { chxerror (CXE_NULL, "chxbreak" , "");            RETURN(0); }
   if (FREED(a))    
      { chxerror (CXE_FREED, "chxbreak" , a->name);      RETURN(0); }
   if (FREED(fore))    
      { chxerror (CXE_FREED, "chxbreak" , fore->name);   RETURN(0); }
   if (FREED(aft))    
      { chxerror (CXE_FREED, "chxbreak" , aft->name);    RETURN(0); }
   if (FREED(pat))    
      { chxerror (CXE_FREED, "chxbreak" , pat->name);    RETURN(0); }
   if ( (NOT ISCHIX(a))   || (NOT ISCHIX(fore))  || 
        (NOT ISCHIX(aft)) || (NOT ISCHIX(pat)))
      { chxerror (CXE_NOTCHX, "chxbreak" , "");          RETURN(0); } 

   chxclear (aft);
   chxclear (fore);

   /*** Can we find PAT in A? */
   if ( (i = chxindex (a, 0L, pat)) < 0) { chxcpy (fore, a);   RETURN (0); }

   patlen = chxlen (pat);
   chxcatsub (fore, a, 0L, i);
   chxcatsub (aft,  a, i + patlen, ALLCHARS);

   RETURN (1);
}
