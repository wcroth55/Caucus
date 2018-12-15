
/*** A_CCH_TBUF.  Handle API caching of textbuf's.
/
/    tbuf = a_cch_tbuf (verb, ud, uf, cnum, inum, rnum, owner);
/
/    Parameters:
/       Textbuf tbuf;      (resulting textbuf, from internal cache)
/       int     verb;      (action to take: V_GET, V_CLR, V_SIZE, V_FREE)
/       int     ud;        (unitcode number of directory file)
/       int     uf;        (unitcode number of text file)
/       short   cnum;      (conference number)
/       int     inum;      (item number, or ?)
/       int     rnum;      (response number, or subject number?)
/       Chix    owner;     (if PSUBJECT, userid of owner; else nullchix)
/
/    Purpose:
/       Handle caching of textbuf's used by top-level API functions.
/
/    How it works:
/       V_SIZE is used once, during API initialization: it sets the size
/       of the cache (number of textbuf's in the cache) to the value of UD.
/
/       V_FREE is used once, during API closure; it frees all of the
/       entries in the cache.
/
/       V_GET returns the textbuf that contains the desired partfile
/       (specified by UD, UF, CNUM, INUM, RNUM, OWNER).  If no textbuf
/       in the cache contains that partfile, the oldest textbuf gets
/       loaded with the proper partfile.
/
/       V_CLR clears a textbuf.  This is used when the caller "knows"
/       that the cache'd textbuf may be out of date and should not be used.
/
/    Returns: the appropriate textbuf (V_GET)
/             NULL (all other verbs)
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by: API top-level functions (a_get_resp, etc.)
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/acchtbuf.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/28/93 11:15 New function. */
/*: CK  3/01/93 14:41 Force at least one tbuf. */
/*: CK  3/01/93 16:48 sysmem requires int4 arg. */
/*: CP  8/01/93 14:32 Add 'name' arg to make_textbuf() call. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "textbuf.h"
#define  NULLTBUF   ( (Textbuf) NULL)

FUNCTION   Textbuf a_cch_tbuf (verb, ud, uf, cnum, inum, rnum, owner)
   int     verb, ud, uf, cnum, inum, rnum;
   Chix    owner;
{
   static  Textbuf *cache = NULL;
   static  int      size;
   static  int4     used  = 0;
   int     i, hit;
   char    temp[80];
   Textbuf tbuf, make_textbuf();
   Chix    getline2();

   if (verb==V_SIZE  &&  cache!=NULL)   return (NULLTBUF);

   if (verb==V_SIZE) {
      size  = ud;
      if (size < 1)  size = 1;
      cache = (Textbuf *) sysmem (SIZE(Textbuf) * size, "*Textbuf");
      for (i=0;   i<size;   ++i) {
         sprintf (temp, "acchtbuf %d", i);
         cache[i] = make_textbuf (TB_RESPS, TB_LINES, temp);
      }
   }

   if (cache==NULL)  return (NULLTBUF);

   if (verb==V_FREE) {
      for (i=0;   i<size;   ++i)  free_textbuf (cache[i]);
      sysfree ( (char *) cache);
      cache = NULL;
   }

   if (verb==V_GET) {

      /*** Find the textbuf that contains this cnum/inum/rnum; else,
      /    find the oldest textbuf. */
      for (hit=i=0;   i<size;   ++i) {
         tbuf = cache[i];
         if (tbuf->lastused < cache[hit]->lastused)  hit = i;
         if (uf   != tbuf->type)   continue;
         if (cnum != tbuf->cnum)   continue;
         if (inum != tbuf->item)   continue;
         if (rnum <  tbuf->r_first  ||  rnum > tbuf->r_last)  continue;

         /*** We should also check OWNER, but there appears to be a bug
         /    in getline2()... it doesn't check to make sure that OWNER
         /    is the same!   Generally this doesn't matter, as Caucus
         /    only allows a user to fetch their own psubjects... but
         /    who knows if this will always be true? */

         /*** This appears to be the desired textbuf. */
         hit = i;
/*       printf ("a_cch_tbuf: hit=%d, uf=%d, cnum=%d, inum=%d, rnum=%d\n",
                  hit, uf, cnum, inum, rnum);  */
         break;
      }

      /*** Use the matching (or oldest) textbuf.  Force it to contain
      /    the proper partfile either way, by calling getline2(). */
      tbuf = cache[hit];
      tbuf->lastused = used++;
      getline2 (ud, uf, cnum, owner, inum, rnum, 0, tbuf);
      return  (tbuf);
   }

   if (verb==V_CLR) {
      for (i=0;   i<size;   ++i) {
         tbuf = cache[i];
         if (uf   != tbuf->type)   continue;
         if (cnum != tbuf->cnum)   continue;
         if (inum != tbuf->item)   continue;
         if (rnum <  tbuf->r_first  ||  rnum > tbuf->r_last)  continue;

         tbuf->type     = 0;
         tbuf->lastused = 0;
         break;
      }
   }

   return (NULLTBUF);
}
