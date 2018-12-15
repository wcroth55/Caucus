
/*** GETORESP.   Get original host and response # for a given response.
/
/    ok = getoresp (origresp, orighost, cnum, item, response);
/
/    Parameters:
/       int   ok;            (success?)
/       int  *origresp;      (place to put original response number)
/       Chix  orighost;      (name of original host)
/       int   cnum;          (local conference number)
/       int   item;          (local item number)
/       int   response;      (local response number)
/
/    Purpose:
/       Given a local conference, item and response number, get the name of
/       the original host on which this response was entered, and the number
/       of the response on that host.
/       
/    How it works:
/       Calls getline2() to load the information for this item/response.
/       Then plucks the information out of the block_template global.
/
/    Returns: 1 on success, 0 if item/response does not exist.
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
/    Home:  db/getoresp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/19/91 14:26 New function. */
/*: CR  7/10/91 17:58 If orighost is this host, just use response #. */
/*: CR  7/11/91 12:05 Check if orighost is null OR empty! */
/*: CR 10/29/91 11:45 Add confnum arg to getline2() call. */
/*: CR 10/29/91 11:50 Add cnum argument. */
/*: JX  5/13/92 16:50 Map BLOCK to TBUF, fix getline2(), chixify orighost. */

#include <stdio.h>
#include "caucus.h"

extern Textbuf               tbuf;

FUNCTION  getoresp (origresp, orighost, cnum, item, response)
   int   *origresp;
   Chix   orighost;
   int    cnum, item, response;
{
   int   b;
   Chix  getline2();

   ENTRY ("getoresp", "");

   if (getline2 (XCRD, XCRF, cnum, nullchix, item, response, 0, (Textbuf) NULL) ==
       nullchix) 
      RETURN (0);

   b =  response -  tbuf->r_first;
   if (b < 0  ||  response > tbuf->r_last)  RETURN (0);

#if C_LINK
   if (tbuf->orighost[b] == nullchix  ||  EMPTYCHX (tbuf->orighost[b])) {
      getmyhost (orighost);
      *origresp = response;
   }
   else {
      chxcpy  (orighost, tbuf->orighost[b]);
      *origresp = tbuf->origresp[b];
   }
#endif

   RETURN (1);
}
