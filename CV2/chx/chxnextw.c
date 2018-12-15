/*** CHXNEXTWORD.  Get "next" word out of text.
/
/    where = chxnextword (word, text, pos);
/
/    Parameters:
/       int4  where;  (starting position of found word, -1 if none)
/       Chix  word;   (put next word here)
/       Chix  text;   (text being scanned)
/       int4 *pos;    (search for next word from here; updated)
/
/    Purpose:
/       Extracts the "next" word (blank, tab, or newline-separated token)
/       from chix TEXT. 
/
/    How it works:
/       Scans from *pos, and updates *pos as the word is passed over, so
/       that chxnextword() can be used in a loop.
/
/    Returns: -1 on failure
/             starting position of this word
/
/    Related functions: chxspecial()
/
/    Called by:  application
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Speed: 1-byte, replace chxvalue(), use [256] array instead of
/           list of blanks.
/
/    Home:  chx/chxnextw.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  1/18/96 23:36 New function. */
/*: OQ  2/10/98 replace chxcheck call with in line commands */

#include <stdio.h>
#include "chixdef.h"
#include "done.h"

FUNCTION  int4 chxnextword (word, text, pos)
   Chix   word, text;
   int4  *pos;
{
   static int4 *blanklist = NULL;
   static char  blanktab[256];
   int          j, letter, text_type, space;
   int4         ptr, start_pos, c;
   char        *sysmem();

   ENTRY ("chxnextw", "");

   /*** Input validation. */
   /*if (NOT chxcheck (word, "chxnextw"))  RETURN (-1);*/
   /*if (NOT chxcheck (text, "chxnextw"))  RETURN (-1);*/
   if (word == nullchix || text == nullchix)
      { chxerror (CXE_NULL, "chxnextw", "");           RETURN(-1); }
   if (FREED(word))    
      { chxerror (CXE_FREED, "chxnextw", word->name);  RETURN(-1); }
   if (FREED(text))    
      { chxerror (CXE_FREED, "chxnextw", text->name);  RETURN(-1); }
   if (NOT ISCHIX(word) || NOT ISCHIX(text))
      { chxerror (CXE_NOTCHX, "chxnextw", "");         RETURN(-1); }
  

   /*** On first call, get the list of "blank" characters.  If it's 
   /    empty, use the standard ascii blank, ' '. */
   if (blanklist == NULL) {
      chxspecial (SC_GET, SC_BLANK, word);
      if (chxvalue (word, 0L) == 0L)  chxcatval (word, THIN, L(' '));
      chxcatval (word, THIN, L('\n'));
      chxcatval (word, THIN, L('\t'));
      blanklist = (int4 *) sysmem (L(sizeof(int4) * (chxlen (word) + 2)), "nextw");
      for (j=0;   j < 256;   ++j)  blanktab[j] = 0;
      for (j=0;   (blanklist[j] = chxvalue (word, L(j)));   ++j) {
         if (blanklist[j] < 256)  blanktab[blanklist[j]] = 1;
      }
   }


   /*** Starting at POS, skip any "spaces" in TEXT... */
#if USE_THIN_CHIX
   for (ptr=*pos;   (c = text->p[ptr]);   ++ptr) {
      if (NOT blanktab[c])  break;
#else
   for (ptr=*pos;   (c = chxvalue (text, ptr));   ++ptr) {
      if (c < 256  && NOT blanktab[c])  break;

      for (space=j=0;   blanklist[j];   ++j) {
         if (c == blanklist[j])  { space = 1;   break; }
      }
      if (NOT space)  break;
#endif
   }

  *pos = start_pos = ptr;
   if (c == 0L)  RETURN (-1);

   /*** The word starts with this character... */
   text_type = chxtype (text);
   chxclear  (word);
   chxcatval (word, text_type, c);

   /*** And keep accumulating until we get to a space (or end of string). */
#if USE_THIN_CHIX
   for (++ptr;   (c = text->p[ptr]);   ++ptr) {
      if (blanktab[c])  break;
#else
   for (++ptr;   (c = chxvalue (text, ptr));   ++ptr) {
      if (c < 256  &&  blanktab[c])  break;
      for (space=j=0;   blanklist[j];   ++j) {
         if (c == blanklist[j])  { space = 1;   break; }
      }
      if (space)  break;
#endif
      chxcatval (word, text_type, c);
   }

  *pos = ptr;
   RETURN (start_pos);
}
