
/*** CHXCOMPARE.  Lexical comparison between two chix strings.
/
/    n = chxcompare (x, y, len);
/
/    Parameters:
/       int   n;    ( x > y --> 1;   x < y --> -1;  x = y --> 0)
/       Chix  x;
/       Chix  y;
/       int4  len;  (compare to this many characters)
/
/    Purpose:
/       Perform a lexical comparison between two chix.  The result
/       can be thought of as the sign of "x - y".  That is, if X
/       is lexically greater than (later in the collating order than)
/       Y, chxcompare (x, y) returns 1.  The opposite case returns -1.
/       If the two chix are identical, chxcompare() returns 0.
/
/       As usual, when chix of different sizes are compared, the shorter
/       characters are promoted by adding leading 0 bytes.
/
/       Chxcompare() compares the first LEN characters of X and Y.
/       If they are identical to that point, it returns 0.
/
/    How it works:
/       Chxcompare() calls chxscan() to find where the chix differ, 
/       and then applies the collating order defined by chxcollate()
/       to decide which string is lexically greatest.
/
/    Returns: 1  for "X > Y"
/            -1  for "X < Y"
/             0  for "X = Y"  (within LEN characters)
/
/    Error Conditions: If X or Y is not a chix, it is treated as
/       a zero-length chix for the purposes of comparison.
/  
/    Side effects:
/
/    Related functions: chxcollate().
/
/    Called by:  application
/
/    Operating system dependencies:
/
/    Known bugs:
/       This code should eventually include the ebcdic -> ascii mapping
/       for the default collating order for 1 byte chix.  See the
/       old sysstrco() function for details.
/
/    Home:  chx/chxcompa.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CX  5/21/92 13:11 Add LEN argument. */
/*: JX  6/16/92 16:35 LEN == DIFF_POS is a match. */
/*: CR 12/11/92 13:05 LEN arg is long. */
/*: CR 11/17/94 14:33 Handle len==ALLCHARS case properly. */
/*: OQ 02/10/98 replace chxcheck with in line command */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxcompare (x, y, len)
   Chix   x, y;
   int4   len;
{
   int4   x_end, y_end, xc, yc, diff_pos;
   int    x_type, y_type;
   uchar *xp, *yp;

   ENTRY ("chxcompa", "");

   /*if (chxcheck (x, "chxcompa")) { x_type = x->type;   xp = x->p; }*/
   if ( (x != nullchix) &&  (NOT FREED(x)) && ISCHIX(x)) 
          { x_type = x->type;   xp = x->p; }
   else   { x_type = THIN;      xp = "";   }

   /*if (chxcheck (y, "chxcompa")) { y_type = y->type;   yp = y->p; }*/
   if ( (y != nullchix) && (NOT FREED(y)) && ISCHIX(y)) 
          { y_type = y->type;   yp = y->p; }
   else   { y_type = THIN;      yp = "";   }
   
   diff_pos = chxscan (xp, x_type, yp, y_type, &x_end, &y_end);

   /*** Take care of the easy cases involving hitting end of a string. */
   if (len   != ALLCHARS  &&  len   <= diff_pos)  RETURN ( 0);
   if (x_end == 0L        &&  y_end == 0L)        RETURN ( 0);
   if (len   == ALLCHARS  &&  x_end == 0L)        RETURN (-1);
   if (len   == ALLCHARS  &&  y_end == 0L)        RETURN ( 1);

   /*** All other cases we have to do the collation sequence transform,
   /    and compare the differing characters. */
   xc = chxcollate (APPLY_CT, x_end, 0L, 0L, 0L);
   yc = chxcollate (APPLY_CT, y_end, 0L, 0L, 0L);

   if (xc > yc)  RETURN ( 1);
   if (xc < yc)  RETURN (-1);
   RETURN (0);
}
