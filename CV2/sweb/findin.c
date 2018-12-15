
/*** FIND_IN.   Search (part of) an item for keywords, and return hit list.
/
/    find_in (result, arg, conf);
/   
/    Parameters:
/       Chix     result;  (append resulting value here)
/       Chix     arg;     (function argument)
/       SWconf   conf;    (configuration parameters)
/
/    Purpose:
/       Implement 4.1 "$find_in_item (cnum inum r0 r1 any inword text)"
/       function.
/
/    How it works:
/
/    Returns: (in RESULT)
/       "pickup triplet" followed by sets of triplets of responses that
/       had hits (if any).  The pickup triplet tells where to continue
/       searching from if find_in() timed out before it was done searching.
/       Otherwise, if find_in() *did* finish searching, the pickup 
/       triplet is "0 0 0".
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

/*: CR  8/07/99 22:16 New function, based on old search_it(). */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

extern Cstat g_cstat;
extern Pstat g_pstat;

extern FILE *sublog;
extern int   cml_debug;

FUNCTION  find_in (Chix result, Chix arg, SWconf conf)
{
   static Resp  resp  = (Resp)  NULL;
   int4         pos, cnum, inum, r0, r1, rnum, any, inword, time0;
   int4         systime();
   Chix         search, word, triplets;
   char         temp[100], pickup[100];
   int          which, hits[200], i, match, count, textmark;
   ENTRY ("find_in", "");

   time0 = systime();
   strcpy (pickup, "0 0 0 ");

   if (resp == (Resp) NULL) resp = a_mak_resp ("find_in resp");
   search   = chxalloc (L(60), THIN, "find_in search");
   triplets = chxalloc (L(60), THIN, "find_in triplets");
   word     = chxalloc (L(60), THIN, "find_in word");

   /*** Decode conf num, item num, resp range. */
   pos    = L(0);
   cnum   = chxint4 (arg, &pos);
   inum   = chxint4 (arg, &pos);
   r0     = chxint4 (arg, &pos);
   r1     = chxint4 (arg, &pos);
   any    = chxint4 (arg, &pos);
   inword = chxint4 (arg, &pos);
   textmark = 7;

   /*** Pluck out the string to be searched for, and COUNT the number
   /    of words it contains. */
   if (chxtoken (nullchix, search, textmark, arg) < 0)  goto done;
   jixreduce (search);
   for (count=0;   chxtoken (word, nullchix, count+1, search) >= 0;   ++count) ;

   /*** Verify the conference and item. */
   if (count == 0)                                      goto done;
   if (NOT touch_it (cnum, 0, 0))                       goto done;
   if (inum < 0  ||  inum > g_cstat->items)             goto done;
   if (g_cstat->resps[inum] < 0)                        goto done;

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
         chxcat  (triplets, CQ(temp));
      }

      /*** If we've spent more than pagetime/2 searching this item,
      /    bail out with info telling the caller where to resume searching. */
      if (rnum < r1  &&  systime() - time0 > conf->pagetime / 2) {
         sprintf (pickup, "%d %d %d ", cnum, inum, rnum+1);
         break;
      }
   }

done:
   chxcat  (result, CQ(pickup));
   chxcat  (result, triplets);
   
   chxfree (search);
   chxfree (word);
   chxfree (triplets);
   RETURN(1);
}
