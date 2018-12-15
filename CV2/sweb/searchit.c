
/*** SEARCH_IT.   Search (part of) an item for keywords, and return hit list.
/
/    search_it (result, arg, new);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/       int                 new;     (use new '$find_it()' syntax?)
/
/    Purpose:
/
/    How it works:
/       $search_it (cnum inum r0 r1 any text)
/       $find_it   (cnum inum r0 r1 any inword text)
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  searchit.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/16/96 15:29 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

extern Cstat g_cstat;
extern Pstat g_pstat;

extern FILE *sublog;
extern int   cml_debug;

FUNCTION  search_it (Chix result, Chix arg, int new)
{
   static Resp  resp  = (Resp)  NULL;
   int4         pos, cnum, inum, r0, r1, rnum, any, inword;
   Chix         search, word;
   char         temp[100];
   int          which, hits[200], i, match, count, textmark;
   ENTRY ("search_it", "");

   if (resp  == (Resp) NULL) resp  = a_mak_resp ("search_it resp");
   search = chxalloc (L(60), THIN, "search_it search");
   word   = chxalloc (L(60), THIN, "word_it word");

   /*** Decode conf num, item num, resp range. */
   pos  = L(0);
   cnum = chxint4 (arg, &pos);
   inum = chxint4 (arg, &pos);
   r0   = chxint4 (arg, &pos);
   r1   = chxint4 (arg, &pos);
   any  = chxint4 (arg, &pos);

   /***  Handle differences between $search_it() and $find_it().  Ugh.  */
   inword   = 0;
   textmark = 6;
   if (new) {
      inword   = chxint4 (arg, &pos);
      textmark = 7;
   }

   /*** Pluck out the string to be searched for, and COUNT the number
   /    of words it contains. */
   if (chxtoken (nullchix, search, textmark, arg) < 0)  RETURN(1);
   jixreduce (search);
   for (count=0;   chxtoken (word, nullchix, count+1, search) >= 0;   ++count) ;

   /*** Verify the conference and item. */
   if (count == 0)                               RETURN(1);
   if (NOT touch_it (cnum, 0, 0))                RETURN(1);
   if (inum < 0  ||  inum > g_cstat->items)      RETURN(1);
   if (g_cstat->resps[inum] < 0)                 RETURN(1);

   /*** An R1 of -1 means "through last response". */
   if (r1 < 0)  r1 = g_cstat->resps[inum];

   /*** Search the selected response range in this item. */
   for (rnum = r0;   rnum <= r1;   ++rnum) {
      resp->cnum = cnum;
      resp->inum = inum;
      resp->rnum = rnum;
      if (a_get_resp (resp, P_EVERY, A_WAIT) != A_OK)   continue;
      jixreduce (resp->text->data);

      /*** Clear the # of hits for each word in SEARCH. */
      for (i=0;   i<count;   ++i)  hits[i] = 0;

      /*** If matches are allowed inside a word, just scan the entire
      /    response for occurences of the SEARCH words. */
      if (inword) {
         for (which=0;   chxtoken (word, nullchix, which+1, search) >= 0;   
              which++) {
            if (chxindex (resp->text->data, L(0), word) >= 0) {
               hits[which]++;
               if (any)  break;
            }
         }
      }

      /*** Otherwise (matches must occur at the start of a word),
      /    scan the entire response a word at a time. */
      else {
         for (pos=0L;   chxnextword (word, resp->text->data, &pos) >= 0;  ) {
            if ( (which = tablepart (search, word)) >= 0) {
               hits[which]++;
               if (any)  break;
            }
         }
      }

      /*** Figure the # of matches we got (out of the # of words in SEARCH). */
      for (match=i=0;   i<count;   ++i)   if (hits[i])   ++match;
         
      /*** Was this a "success" ? */
      if ( (any && match)  ||  (NOT any  &&  match==count)) {
         sprintf (temp, "%d %d %d ", cnum, inum, rnum);
         chxcat  (result, CQ(temp));
      }
   }
   
   chxfree (search);
   chxfree (word);
   RETURN(1);
}
