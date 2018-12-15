/*** CHXTOKEN.  Get N'th token out of a line.
/
/    i = chxtoken (word, rest, n, line);
/
/    Parameters:
/       int   i;     (starting position of found token, or -1 if fail)
/       Chix  word;  (Put N'th token here)
/       Chix  rest;  (Put N'th token through end of line here)
/       int   n;     (Desired token.  First token is #1. )
/       Chix  line;  (Line of text being scanned)
/
/    Purpose:
/       Extract single words or "tokens" from a chix.  Tokens are
/       sequences of characters separated by any number of blanks.
/       Blanks are characters that have been assigned the SC_BLANK
/       property by chxspecial().
/
/    How it works:
/       Scan LINE for the N'th token.  If WORD is not nullchix, put
/       that token into WORD.  If REST is not nullchix, copy all of
/       line, from the N'th token on, into REST.
/
/       The first token in a line is token number 1.  If there is no
/       N'th token, WORD and REST are cleared.
/
/    Returns: -1 on failure
/             starting position of N'th token
/
/    Error Conditions: LINE not chix.  REST not nullchix and not chix,
/       WORD not nullchix and not chix.  Complains via chxerror().
/  
/    Side effects:
/
/    Related functions: chxspecial()
/
/    Called by:  application
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Speed: 
/
/    Home:  chx/chxtoken.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX 12/10/90 23:36 New function. */
/*: CR  7/28/91 15:48 chxalloc() first argument is long. */
/*: CR 10/18/91 14:22 Return starting position of token. */
/*: JV 10/15/92 11:02 Actually free BLANKS. */
/*: JV 10/16/92 09:24 Make BLANKS static, don't free it. */
/*: CL 12/11/92 11:13 Long Chxcatsub args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CR 12/17/92 12:19 Return -1 on bad args. */
/*: CR  3/20/95 13:38 Speed up 'blank' comparison. */
/*: OQ  2/21/98 replace chxcheck with in line code */
/*: CR  6/23/98 Speed: 1-byte uses blank-lookup table, replaced chxvalue. */

#include <stdio.h>
#include "chixdef.h"
#include "done.h"

FUNCTION  chxtoken (word, rest, n, line)
   Chix   word, rest, line;
   int    n;
{
   static char  blanktab[256];
   static int4 *blanklist = NULL;
   Chix         blanks;
   int4   c, b;
   int    j, w, start, between, space, line_type;
   int4   i;
   char  *sysmem();

   ENTRY ("chxtoken", "");

   /*** Input validation. */
   /*if (word != nullchix  &&  NOT chxcheck (word, "chxtoken"))  RETURN (-1);*/
   /*if (rest != nullchix  &&  NOT chxcheck (rest, "chxtoken"))  RETURN (-1);*/
   /*if (                      NOT chxcheck (line, "chxtoken"))  RETURN (-1);*/

   if ( word != nullchix) { 
      if (FREED(word)) 
         { chxerror (CXE_FREED, "chxtoken", word->name);         RETURN(-1); }
      if (NOT ISCHIX(word) )
         { chxerror (CXE_NOTCHX, "chxtoken", "");                RETURN(-1); }
   }
   if ( rest != nullchix) { 
      if (FREED(rest)) 
         { chxerror (CXE_FREED, "chxtoken", rest->name);         RETURN(-1); }
      if (NOT ISCHIX(rest) )
         { chxerror (CXE_NOTCHX, "chxtoken", "");                RETURN(-1); }
   }
   
   if (line == nullchix)
         { chxerror (CXE_NULL, "chxtoken", "");                  RETURN(-1); }
   if (FREED(line))    
         { chxerror (CXE_FREED, "chxtoken", line->name);         RETURN(-1); }
   if (NOT ISCHIX(line))
         { chxerror (CXE_NOTCHX, "chxtoken", "");                RETURN(-1); }

   /*** Get the list of "blank" characters.  If it's empty, use the
   /    standard ascii blank, ' '. */
   if (blanklist == NULL) {
      blanks = chxalloc (16L, THIN, "chxtoken blanks");
      chxspecial (SC_GET, SC_BLANK, blanks);
      if (chxvalue (blanks, 0L) == 0L)  chxcatval (blanks, THIN, (int4) ' ');
      blanklist = (int4 *) sysmem (L(sizeof(int4) * (chxlen (blanks) + 2)), "token");
      for (j=0;   (blanklist[j] = chxvalue (blanks, L(j)));   ++j)  ;
      for (j=0;   j < 256;        ++j)  blanktab[j] = 0;
      for (j=0;   blanklist[j];   ++j)  {
         if (blanklist[j] < 256)  blanktab[blanklist[j]] = 1;
      }
      chxfree (blanks);
   }

   if (word != nullchix)  chxclear (word);
   if (rest != nullchix)  chxclear (rest); 

   /*** BETWEEN means we are between tokens (in the white space).   START
   /    is the starting position of the N'th token.  W is the current word
   /    number we are on. */
   between =  1;
   start   = -1;
   w       =  0;

   /*** Scan characters C in LINE one at a time... */
   line_type = chxtype (line);

#if USE_THIN_CHIX
   for (i=0;   (c = line->p[i]);   ++i) {
      space = blanktab[c];

#else
   for (i=0;   (c = chxvalue (line, i));   ++i) {

      /*** Is this character a SPACE? */
      for (space=j=0;   blanklist[j];   ++j) {
         if (c == blanklist[j])  { space = 1;   break; }
      }
#endif

      if (space) {
         if (w >= n)  break;
         between = 1;
      }

      /*** Not a space... */
      else {
         if (between  &&  ++w == n)          start = i;
         if (w == n   &&  word != nullchix)  chxcatval (word, line_type, c);
         between = 0;
      }
   }

   if (start < 0)  RETURN (-1);
   if (rest != nullchix) {
/*    chxclear  (rest); */
      chxcatsub (rest, line, L(start), ALLCHARS);
   }

   RETURN (start);
}
