
/*** STORNPART.   Store Name partfile. 
/
/    ok = stornpart (ud, uf, owner, cnum, item, namdir, p, tbuf);
/
/    Parameters:
/       int     ok;          (success?)
/       int     ud;          (partfile directory unit)
/       int     uf;          (partfile file unit)
/       Chix    owner;       (partfile owner userid, if any)
/       int     cnum;        (conference number)
/       int4    item;        (item number, if relevant)
/       Namlist namdir;      (namelist of partfile directory)
/       int     p;           (partfile number)
/       Textbuf tbuf;        (text buffer that contains name partfile)
/
/    Purpose:
/       TBUF contains a (modified) name partfile.  Stornpart() writes
/       it out to the partfile specified by units UD,UF, userid OWNER,
/       conference CNUM, and ITEM.
/
/    How it works:
/       NAMDIR contains a Namelist of the directory of partfiles for
/       UD/UF.  P is the partfile number.
/
/       Stornpart() tries to just write TBUF out to the appropriate file.
/       If the contents of TBUF are too large to fit in one file, stornpart()
/       splits TBUF in two and writes two files.  In the latter case, it
/       also modifies the directory in NAMDIR.
/
/    Returns: 1 on success, 0 on error
/
/    Error Conditions:
/       Fails if TBUF is full and contains all identical names.
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  modnames(), ??
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  people/stornpar.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:50 Source code master starting point */
/*: CR  8/28/89 13:00 Don't split namepart file between identical strings. */
/*: CR  9/12/89 16:47 Correct bugtell/buglist confusion. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: CR 11/21/91 15:06 Add CNUM as argument. */
/*: UL 12/11/91 18:43 Make unit_lock args 3-6 int4.*/
/*: CR 12/30/91 14:49 Splitting must use 1st token for comparison. */
/*: CX  5/27/92 14:56 Guru-ized. */
/*: CP  5/10/93  0:00 Add 'item' argument. */
/*: CR 11/17/94 11:45 Allow split if full of identical strings. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

FUNCTION   stornpart (ud, uf, owner, cnum, item, namdir, p, tbuf)
   int     ud, uf, p, cnum;
   int4    item;
   Chix    owner;
   struct  namlist_t *namdir;
   Textbuf tbuf;
{
   short  split, i, pnew, mid;
   Chix   boundary, high, low, middle;
   char  *bugtell();
   int    success;

   ENTRY ("stornpart", "");

   boundary = chxalloc (L(150), THIN, "stornpar boundry");
   high     = chxalloc (L(150), THIN, "stornpar high");
   low      = chxalloc (L(150), THIN, "stornpar low");
   middle   = chxalloc (L(150), THIN, "stornpar middle");
   success  = 0;

   /*** We should SPLIT this partfile before storing it only if
   /    it contains too many lines. */
   split = tbuf->l_used > tbuf->l_max-10;

   if (split) {

      /*** Start the split by inserting a boundary marker in the MIDdle.
      /    The same marker will be used for the end of the first file and
      /    the start of the second.  Try not to split between two identical
      /    strings--shift MID around to find strings that are different. */
      mid = tbuf->l_used / 2;
      chxtoken (middle, null.chx, 1, tbuf->line[mid]);
      for (i=1;   (1);   ++i) {
         chxtoken (high, null.chx, 1, tbuf->line[mid+i]);
         if (NOT chxeq (middle, high)) { mid = mid+i-1;   break; }

         chxtoken (low,  null.chx, 1, tbuf->line[mid-i]);
         if (NOT chxeq (middle, low))  { mid = mid-i;     break; }

         if (mid-i < 3  ||  mid+i+3 >= tbuf->l_used) {
            /*** Most strings are identical; hold our breath and split
            /    in the middle. */
            mid = tbuf->l_used / 2;
            break;
         }
      }
      chxtoken   (boundary, null.chx, 1, tbuf->line[mid]);
      chxcat     (boundary, CQ("!"));
      nameinsert (boundary, mid+1, tbuf);

      /*** Since splitting means turning one file into two, we must add
      /    a new entry to the name directory (and mark it changed). */
      nladd  (namdir, boundary, pnew = namdir->num++);
      chxcpy (namdir->str, CQ("1"));

      stornp2 (uf, owner, cnum, item, p,        0, mid+1,          tbuf);
      stornp2 (uf, owner, cnum, item, pnew, mid+1, tbuf->l_used-1, tbuf);
   }
   else {
      stornp2 (uf, owner, cnum, item, p,        0, tbuf->l_used-1, tbuf);
   }
   SUCCEED;

 done:

   chxfree ( boundary );
   chxfree ( high );
   chxfree ( low );
   chxfree ( middle );

   RETURN ( success );
}


FUNCTION   stornp2 (u, owner, cnum, item, p, a, b, tbuf)
   int     u, p, a, b, cnum;
   int4    item;
   Chix    owner;
   Textbuf tbuf;
{
   int   i;

   unit_lock (u, WRITE, L(cnum), L(p), item, L(0), owner);
   unit_make (u);
   for (i=a;   i<=b;   ++i) {
      unit_write (u, tbuf->line[i]);
      unit_write (u, CQ("\n"));
   }
   unit_close (u);
   unit_unlk  (u);
   return (1);
}
