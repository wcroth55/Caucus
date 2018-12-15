/*** JIXSCRSIZE.  Returns the two "widths", of a "jix" character.
/
/    truncate = jixscrsize (a, maxlen, maxchar, maxscr);
/
/    Parameters:
/       int   truncate;   // Will the caller need to truncate to fit text?
/       Chix  a;          // Chix we're measuring
/       int   maxlen,     // Max scrunits (screen units) caller wants to use
/            *maxchar,    // Returned value: Max number of chars that will
/                         // fit in MAXLEN scrunits
/            *maxscr      // Max scrunits actually used by MAXCHAR chars.
/
/    Purpose:
/       A jix string can be made up of characters that take up one or two
/       screen columns each.  When formatting a line of screen output,
/       we need to know how many screen columns the jix will take up.
/       This function gives the caller the necessary information to format
/       text properly.
/
/    How it works:  It uses jixwidth to measure each character.
/       
/    Returns: TRUNCATE: 1 if A will not fit in MAXLEN scrunits.
/             MAXCHAR:  Number of characters of A that will fit in MAXLEN
/                       scrunits.
/             MAXSCR:   Number of scrunits used by MAXCHAR chars.
/
/    Error Conditions:  A not a chix.
/  
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  chxformat().
/
/    Operating system dependencies: none
/
/    Known bugs:
/
/    Home:  chx/jixscrsi.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JX  6/30/92 14:54 New function. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CR 12/11/92 14:02 Increase speed; int4 chxvalue arg. */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  jixscrsize (a, maxlen, maxchar, maxscr)
   Chix   a;
   int    maxlen, *maxchar, *maxscr;
{
   int    trunc, charscr;
   int4   i, value;

   ENTRY ("jixscrsize", "");

   *maxscr = trunc = 0;

   /*** Measure and sum each chix element. */
   for (i=0;   *maxscr <= maxlen;   i++) {
      value   = chxvalue (a, i);
      if (value == 0)  break;
      charscr = jixwidth (value);
      *maxscr += charscr;
   }

   *maxchar = i;

   /*** Back up if we've gone too far. */
   if (*maxscr > maxlen) {
      *maxscr -= charscr;
      (*maxchar)--;
      trunc = 1;
   }

   RETURN (trunc);
}
