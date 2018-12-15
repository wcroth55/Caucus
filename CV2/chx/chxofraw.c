
/*** CHXOFRAW.  Load "raw" data into a chix.
/
/    chxofraw (a, raw, width, len);
/
/    Parameters:
/       Chix  a;       (place to put resulting chix)
/       char *raw;     (source of raw data)
/       int   width;   (width of data)
/       int4  len;     (length of data, in bytes(?) )
/
/    Purpose:
/       Copy "raw" data into a chix.
/
/    How it works:
/       Copies LEN bytes from RAW into chix A's data area, expanding
/       it as needed.  Sets chix A's type to WIDTH.  A must be an
/       already allocated chix.
/
/       Chxofraw() automatically puts the proper number of 0 bytes
/       on the end of the data, even if they were already supplied
/       in RAW.
/
/    Returns: 1 if successful, 0 on error.
/
/    Error Conditions:
/       A not chix.
/       RAW points to null.
/       LEN <= 0.
/       WIDTH < 1, WIDTH > WIDEST
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  mdstr()
/
/    Operating system dependencies: none
/
/    Known bugs:
/
/    Home:  chx/chxofraw.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/08/92 16:15 New function. */
/*: CR 12/11/92 12:52 Chxmem 1st arg is long. */
/*: OQ  2/14/98 replace chxchex by in line commands */
/*: OQ  3/02/98 added code for single byte character chix type */

#include <stdio.h>
#include "chixdef.h"

FUNCTION  chxofraw (a, raw, width, len)
   Chix   a;
   char  *raw;
   int    width;
   int4   len;
{
   int4   i;
   int    j;

   ENTRY ("chxofraw", "");

   /*** Input validation. */
   /*if (NOT chxcheck (a, "chxofraw")     ||
         raw == NULL                      ||
         len <= 0                         ||
         width < 1  ||  width > WIDEST)   RETURN (0);*/
   if (a == nullchix)
      { chxerror (CXE_NULL, "chxofraw", "");       RETURN(0); }
   if (FREED(a))    
      { chxerror (CXE_FREED, "chxfraw", a->name);  RETURN(0); }
   if (NOT ISCHIX(a))
      { chxerror (CXE_NOTCHX, "chxfraw", "");      RETURN(0); }

   if ( raw   == NULL                      ||
        len   <= 0                         ||
        width <  1  ||  width > WIDEST)   RETURN (0);
   
   sysfree ((char *) a->p);
   a->p = (uchar *) chxmem (len + width, a->name);
   a->maxlen = len/width;
   a->actlen = -1;
   a->resize =  0;
   a->type   = width;

   for (i=0;   i<len;    ++i)  a->p[i]   = raw[i];

#if USE_THIN_CHIX
   a->p[i] = 0;
#else
   for (j=0;   j<width;  ++j)  a->p[i++] = 0;
#endif
   RETURN (1);
}
