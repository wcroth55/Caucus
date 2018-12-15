
/*** SEARCH_FILTER.   Filter text by searchable "hits".
/
/    search_filter (result, arg, new);
/   
/    Parameters:
/       Chix        result;  (append resulting value here)
/       Chix        arg;     (function argument)
/       int         new;     (use new $find_filter() form?)
/
/    Purpose:
/       Output filter: reduces text to searchable "hits" and context.
/
/    How it works:
/       The CML functions are: 
/         $search_filter (size word1 [word2...]        text)
/         $find_filter   (size word1 [word2...] inword text)
/
/       SIZE is the number of words in word1 [word2...].  TEXT is the
/       text to be searched.
/
/       INWORD controls whether matches are allowed inside a word, (or else)
/       only at the beginning of a word.
/
/       The filter finds those lines of TEXT that contain one or
/       more of the WORDS, and outputs them (with appropriate additional
/       lines for context), encoding the "hits" so that they will
/       appear in boldface.
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  searchfilter.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/17/96 16:22 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

FUNCTION  search_filter (Chix result, Chix arg, int new)
{
   int4   pos, size, ptr, spot, where, inword, len, wp;
   int    which, base, hit, miss;
   Chix   word, table, orig, line, replace, previous;
   ENTRY ("search_filter", "");

   word     = chxalloc (L(40), THIN, "search_filter word");
   table    = chxalloc (L(60), THIN, "search_filter table");
   orig     = chxalloc (L(90), THIN, "search_filter orig");
   line     = chxalloc (L(90), THIN, "search_filter line");
   replace  = chxalloc (L(40), THIN, "search_filter replace");
   previous = chxalloc (L(40), THIN, "search_filter previous");

   pos  = L(0);
   size = chxint4 (arg, &pos);
   
   /*** Pick up the SIZE words and put them in a table. */
   for (which=0;   which < size;   ++which) {
      chxnextword (word, arg, &pos);
      chxcat      (table, word);
      chxcatval   (table, THIN, L(' '));
   }
   jixreduce (table);

   /*** Pick up INWORD. */
   inword = 0;
   if (new) inword = chxint4 (arg, &pos);

   /*** For each line in ARG, display it (and its surrounding lines) if
   /    there is a "hit". */
   hit = miss = 0;
   while (chxnextline (orig, arg, &pos)) {
      chxcpy    (line, orig);
      jixreduce (line);

      if (inword) {
/*       fprintf (stderr, "\n--------\n");  fflush (stderr); */
         for (base=ptr=0;   chxvalue (line, ptr);   ++ptr) {
/*          fprintf (stderr, "%c", chxvalue(line,ptr));  fflush (stderr); */
            for (wp=0;   chxnextword (word, table, &wp) >= 0;   ) {
               if (chxsubeq (line, ptr, word)) {
                  len = chxlen(word);
                  spot = base + ptr;
                  chxclear  (word);
                  chxcatsub (word, orig, spot, len);
                  chxclear  (replace);
                  chxcatval (replace, THIN, L(1));
                  chxcat    (replace, word);
                  chxcatval (replace, THIN, L(2));
                  chxalter  (orig, base+ptr, word, replace);
                  base += 2;
                  ptr  += len - 1;
                  break;
               }
            }
         }
      }

      else {
         /*** Scan words in LINE for hits. */
         for (base=ptr=0;   (where = chxnextword (word, line, &ptr)) >= 0;   ) {
   
            if (tablepart (table, word) >= 0)  {
               /*** Tag each "hit" word in ORIG with surrounding ^A ^B pair. */
               spot = base + where;
               chxnextword (word, orig, &spot);
               chxclear  (replace);
               chxcatval (replace, THIN, L(1));
               chxcat    (replace, word);
               chxcatval (replace, THIN, L(2));
               chxalter  (orig, base+where, word, replace);
               base += 2;
            }
         }
      }

      /*** A hit? */
      if (base) {
         /*** 2nd or more hit after a gap puts in "..." */
         if (hit  &&  miss > 2) {
            chxcatval (result, THIN, L(1));
            chxcat    (result, CQ(" ... "));
            chxcatval (result, THIN, L(2));
            chxcatval (result, THIN, L('\n'));
         }
         hit  = 1;
         miss = 0;
         /*** Display this line and previous line. */
         chxcat    (result, previous);
         chxcat    (result, orig);
         chxcatval (result, THIN, L('\n'));
         chxclear  (previous);
      }
      else {
         /*** Line after a hit gets displayed. */
         ++miss;
         if (hit  &&  miss==1) {
            chxcat    (result, orig);
            chxcatval (result, THIN, L('\n'));
            chxclear  (previous);
         }
         /*** All other lines get saved in case *next* line is a hit. */
         else {
            chxcpy    (previous, orig);
            chxcatval (previous, THIN, L('\n'));
         }
      }

   }

   chxfree (word);
   chxfree (table);
   chxfree (orig);
   chxfree (line);
   chxfree (replace);
   chxfree (previous);
   RETURN  (1);
}
