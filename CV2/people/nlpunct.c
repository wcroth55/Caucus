/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** NLPUNCT.  Remove punctuation from names in a namelist.
/
/    NLPUNCT produces a new namelist, modified from the contents of
/    namelist LIST so that:
/
/    (a) If WITHPUNCT is FALSE, all punctuation chars in the first word
/        in each node in LIST are removed.  If the word becomes empty, 
/        the node is removed.
/
/    (b) If WITHPUNCT is TRUE, for each node whose first word contains 
/        any punctuation characters, a copy of the node *without* the 
/        punctuation characters in the first word is added to LIST.
/
/    NLPUNCT returns as its value the pointer to the new namelist.
/    The contents of this new list are properly sorted. 
/ */

/*: CR  8/28/90 17:24 New function. */
/*: DE  4/14/92 16:12 Chixified */
/*: JX  5/28/92 14:49 strpunct->chxpunct. */
/*: CX  6/29/92 15:30 Allocate S! */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

FUNCTION  struct namlist_t *nlpunct (list, withpunct)
   struct namlist_t *list;
   int    withpunct;
{
   struct namlist_t *new, *p, *nlnode();
   Chix   p_word, n_word, str;
   Chix   s;

   ENTRY ("nlpunct", "");

   p_word    = chxalloc (L(100), THIN, "nlpunct p_word");
   n_word    = chxalloc (L(100), THIN, "nlpunct n_word");
   str       = chxalloc (L(180), THIN, "nlpunct str");
   s         = chxalloc (L(100), THIN, "nlpunct s");

   new = nlnode (1);

   /*** For each node in LIST, examine the first word, and strip off any
   /    punctuation. */
   for (p=list->next;   p!=null.nl;   p=p->next) {
      chxtoken (p_word, null.chx, 1, p->str);
      chxpunct (n_word, p_word);

      /*** If with & without punctuation are the same, add this node 
      /    to the NEW list. */
      if (chxeq (p_word, n_word)) { nladd (new, p->str, p->num);   continue; }

      chxtoken (null.chx, s, 2, p->str);

      /*** Otherwise, add without punctuation (if anything's left)... */
      if (NOT EMPTYCHX (n_word)) {
         chxconc (str, n_word, CQ(" "));   chxcat (str, s);
         nladd   (new, str, p->num);
      }

      /*** ... and add with punctuation (if caller told us to). */
      if (withpunct) {
         chxconc (str, p_word, CQ(" "));   chxcat (str, s);
         nladd   (new, str, p->num);
      }
   }

   chxfree ( p_word );
   chxfree ( n_word );
   chxfree ( str );
   chxfree ( s );

   RETURN (new);
}
