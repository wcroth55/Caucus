/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** STRTOKEN.  Find the POS'th token in a string.
/
/    STRTOKEN finds the POS'th token in the string LINE, and copies it
/    into WORD.  A token is a sequence of nonblank characters.  The first
/    token in a string is number 1.
/
/    STRTOKEN returns a pointer to the beginning of the found token in 
/    LINE.  If no such token was found, STRTOKEN returns NULL, and
/    sets WORD to the empty string.
/
/ */


/*: AA  7/29/88 17:53 Source code master starting point */
/*: CR  9/15/89 14:33 Add to header comments. */
/*: CS 11/30/90 13:10 Expand size of buf[]. */
/*: CR  4/25/91  0:48 Define TRUE/FALSE only if not defined. */
/*: JV  8/29/91 14:54 Ask questions. */
/*: jV 11/16/91 13:56 Remove Q's, change header. */

#include <stdio.h>
#include "caucus.h"

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

FUNCTION  char *strtoken (word, pos, line)
   char    word[], line[];
   int     pos;
{
   int   between, i, len;
   char  buf[300];
   char  *w, *start;

   ENTRY ("strtoken", "");

   /*** BETWEEN means we are between tokens (in the white space).  */
   between  = TRUE;
   len = strlen (line);

   for (i=0;   i<=len;   i++) {

      if (line[i]==' '  ||  line[i]=='\0') {
         if (NOT between) {
            *w = '\0';
            if ((--pos) <= 0) { strcpy (word, buf);    RETURN (start); }
         }
         between = TRUE;
      }

      else {
         if (between)  {
            w       = buf;
            start   = line + i;
            between = FALSE;
         }
         *w++ = line[i];
      }
   }

   word[0] = '\0';
   RETURN (NULL);
}
