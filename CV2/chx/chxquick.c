
/*** CHXQUICK.  "Quick" conversion of ascii to chix.
/
/    a = chxquick (str, slot);
/
/    Parameters:
/       Chix  a;
/       char *str;     (ascii string to be converted)
/       int   slot;    (internal slot number, 0-9, to use)
/
/    Purpose:
/       Convert a regular "C" string of ASCII characters (an array of
/       bytes, terminated by a 0 byte) in STR, to a chix.  Return the chix.
/   
/    How it works:
/       Chxquick() keeps an internal array of 10 chix.  It converts
/       STR to a chix; stores the result in internal chix number SLOT;
/       and returns that chix as the function value.  This returned chix
/       can be used, but must not be chxfree'd.
/
/       Chxquick() is intended as a quick way to provide a "temporary"
/       conversion of an ascii string to a chix.  The idea is that this
/       temporary form will be used once and then essentially tossed away.
/       For example:
/
/           Chix a, b;
/           ...
/           chxconc (a, b, chxquick ("xyzzy", 0));
/
/       On the other hand, the next example is a BAD use of chxquick():
/
/           Chix a;
/           a = chxquick ("plugh", 1);
/           ...
/           chxfree (a);
/
/       The SLOT number is automatically constrained to 0 through 9.  If
/       STR is null, chxquick() returns an empty chix.
/
/    Returns: a chix
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:
/
/    Home:  chx/chxquick.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  5/24/91 14:45 New function. */
/*: CR  7/28/91 15:48 chxalloc() first argument is long. */
/*: CR  1/03/92 13:36 If STR is too big, return empty chix. */
/*: CR 12/11/92 11:05 chxcapc last arg is long. */
/*: CR  6/23/98 17:10 Speed: else-ifs; embed chxclear; 1-byte strcpy, actlen. */

#include <stdio.h>
#include "chixdef.h"

#define  CTABSIZE  10

FUNCTION  Chix chxquick (str, slot)
   char  *str;
   int    slot;
{
   static Chix ctab[CTABSIZE] = {nullchix};
   int    i;
   int4   len;

   ENTRY ("chxquick", "");

   /*** Input validation. */
   if      (str == NULL)           str  = "";
   if      (slot < 0)              slot = 0;
   else if (slot > CTABSIZE-1)     slot = CTABSIZE-1;
   
   /*** On first call, allocate the slots in CTAB. */
   if (ctab[0] == nullchix)
      for (i=0;   i<CTABSIZE;   ++i)  ctab[i] = chxalloc (30L, THIN, "ctab[i]");

   /*** If by some stupid chance the caller managed to free this SLOT, and
   /    we can still detect it, re-allocate it. */
   if (ctab[slot]->ischix == CHIX_FREED)
       ctab[slot] = chxalloc (30L, THIN, "ctab realloc");

   /*** Make sure this SLOT is big enough to hold STR. */
   len = strlen (str);

/* chxclear (ctab[slot]); */
#if USE_THIN_CHIX
   ctab[slot]->p[0] = '\0';
#else
   for (i=0;   i<ctab[slot]->type;   ++i)  ctab[slot]->p[i] = '\0';
#endif
   ctab[slot]->actlen = 0L;

   if (ctab[slot]->maxlen < len + 5)
      if (NOT chxresize (ctab[slot], ctab[slot]->type, len + 5))
         RETURN (ctab[slot]);

#if USE_THIN_CHIX
   strcpy (ctab[slot]->p, str);
#else
   chxcapc  (ctab[slot]->p, ctab[slot]->type, str, THIN, -1L);
#endif
   ctab[slot]->actlen = len;

   RETURN (ctab[slot]);
}
   
