
/*** CHXALLOC.   Allocate a chix string and initialize it.
/
/    x = chxalloc (maxlen, type, name);
/
/    Parameters:
/       chix  x;          (created chix)
/       int4  maxlen;     (initial maximum length)
/       int   type;       (width of each char in bytes)
/       char *name;       (name for debugging)
/
/    Purpose:
/       Create a new chix string, allocate all of the needed memory for it,
/       initialize it properly, and set it to the "empty" string.
/
/    How it works:
/       Chxalloc() allocates a new chix string header (a struct chix_t)
/       from the heap.  It initializes it to point to a block of at
/       least MAXLEN characters, also allocated from the heap.  If TYPE
/       is THIN, these are single-byte characters; if WIDE, they are
/       double-byte characters.  The TYPE is stored as part of the chix.
/
/       The NAME of the string is stored in the chix for debugging purposes.
/       The name is truncated to the first 7 chars of the first word, 
/       followed by a blank, followed by the first 7 chars of the next
/       word.
/
/       Chxalloc() enforces reasonable values for MAXLEN and TYPE.
/
/    Returns:
/       The new chix (a pointer to a struct chix_t).
/
/    Error Conditions:  see chxmem().
/  
/    Side effects:      see chxmem().
/
/    Related functions: chxfree(), chxmem().
/
/    Called by:  Practically everybody.
/
/    Operating system dependencies:
/
/    Known bugs:
/       This version does not check (using the name of the string)
/       to see if this chix has already been allocated.
/
/       Chix in general can only handle max length of 32K chars,
/       even though the MAXLEN argument is a long.
/
/    Speed:
/
/    Home:  chx/chxalloc.c.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR  7/24/91 22:31 Declare strcpy() to make Lint happy. */
/*: CR  7/28/91 15:35 Make 1st argument long.  */
/*: JV 10/19/92 08:40 Save allocation info in file if debugging. */
/*: CR 12/11/92 17:28 Long maxlen. */
/*: CP  8/01/93 16:09 Use new mem_debug() for memory event recording. */
/*: CK  9/22/93 21:09 Raise minimum size to 128. */
/*: CR  3/15/95 20:29 Add USE_WIDE_CHIX. */
/*: CR  9/18/97 13:39 Optimize USE_THIN_CHIX. */
/*: CR  6/23/98 13:39 Speed: else-if's; embed chxclear code. */

#include <stdio.h>
#include "chixdef.h"

extern int debug;

FUNCTION  Chix chxalloc (maxl, type, name)
   int4   maxl;
   int    type;
   char  *name;
{
   Chix   a;
   char   strname[16], *s, *p;
   int    i;
   int4   maxlen;
   char  *strcpy(), *chxmem();

   ENTRY ("chxalloc", "");
   maxlen = maxl;

   /*** Chix NAMEs are usually of the form "name function".  Take up to the
   /    first 7 letters of "name", and up to the first 7 letters of "function",
   /    and stick them together as the official string name. */
   if  (name==NULL)  name = "";
   for (s=name, p=strname, i=0;   *s && *s!=' ' && i<7;   ++s, ++i)  *p++ = *s;
   while (*s  &&  *s!=' ')  ++s;
   while (        *s==' ')  ++s;
   *p++ = ' ';
   for (                   i=0;   *s && *s!=' ' && i<7;   ++s, ++i)  *p++ = *s;
   *p = '\0';

   /*** (Someday: If this name has already been allocated, complain.) */

   /*** If debugging, record memory allocation event. */
   if (debug)  mem_debug (MEM_ALLOC, strname, "chx", maxl);

   /*** Some versions of Caucus have chix size forced. */
   if      (USE_THIN_CHIX)    type = THIN;
   else if (USE_WIDE_CHIX)    type = WIDE;
   else if (type < THINNEST)  type = THINNEST;
   else if (type > WIDEST)    type = WIDEST;

   /*** Allocate memory for the chix, and initialize it.  MAXLEN must 
   /    be at least 2. */
   if (maxlen <  63)  maxlen =  63;
   a    = (Chix)  chxmem (SIZE (struct chix_t), strname);
#if USE_THIN_CHIX
   a->p = (uchar *) chxmem ( (maxlen+1)       ,   strname);
#else
   a->p = (uchar *) chxmem ( (maxlen+1) * type,   strname);
#endif
   a->maxlen = maxlen;
   a->actlen =  0;
   a->ischix = CHIX_MAGIC;
   a->resize =  0;
   a->dic    = -1;
   a->type   = type;
   strcpy (a->name, strname);

/* chxclear (a); */

#if USE_THIN_CHIX
   a->p[0] = '\0';
#else
   for (i=0;   i<a->type;   ++i)  a->p[i] = '\0';
#endif

   RETURN   (a);
}
