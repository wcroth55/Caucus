/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** PARSEWORDS.   Parse and remove/replace modifier words from a command STR.
/
/    PARSEWORDS is handed a command STR, a table of modifier words
/    in WORDTAB, and a matching table of replacement words in REPTAB.
/    PARSEWORDS considers each word in STR; if it is a
/    legal abbreviation of an entry in WORDTAB, the corresponding
/    entry in FLAGTAB is set to 1.  (All other entries are set to 0.)
/
/    PARSEWORDS complains to the user about any illegal modifier words
/    found in STR.  In either case, the words are removed from STR.
/    (And replaced with the corresponding entry in REPTAB, if legal.)
/    Only numbers or numeric ranges are left behind.  */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: CW  5/18/89 14:25 Add XW unitcodes. */
/*: DE  6/02/92 11:01 Fix chixification */
/*: JX  6/09/92 17:39 chxtoken returns -1 on fail. */
/*: JX  6/16/92 17:39 allocate S, chxalter needs "". */
/*: JX  6/17/92 17:33 remove S, add POS. */
/*: CL 12/10/92 14:52 Long chxalter args. */
/*: CR 12/10/92 15:06 chxalter arg int4. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */

#include "caucus.h"
#include <stdio.h>

extern Chix  ss_dotnl;
extern union null_t  null;

FUNCTION  parsewords (str, wordtab, reptab, flagtab)
   Chix   str, wordtab, reptab[];
   Flag   flagtab[];
{
   short  i, code;
   Chix   word;
   int    success;
   int4   pos;

   ENTRY ("parsewords", "");

   word     = chxalloc (L(80), THIN, "parsewords word");
   success  = 0;

   /*** Clear all entries in FLAGTAB. */
   for (i=0;   chxtoken (word, null.chx, i+1, wordtab) != -1;   ++i)
      flagtab[i] = 0;

   /*** Replace all commas with blanks, and put a pair of blanks
   /    around each dash.  (This will help in parsing ranges like 1-LAST.) */
   while (chxalter (str, L(0), chxquick("-",0),  chxquick("_"  ,1)))  ;
   while (chxalter (str, L(0), chxquick("_",0),  chxquick(" - ",1)))  ;
   while (chxalter (str, L(0), chxquick(",",0),  chxquick(" "  ,1)))  ;

   /*** Consider each word in STR. */
   for (i=1;  (pos = chxtoken (word, nullchix, i, str)) != -1;   ++i) {

      /*** Skip numbers or numeric ranges. */
      if (cf_digit ((int) chxvalue(word,0L)) || (int) (chxvalue(word,0L) == '-'))
         continue;

      /*** Look up WORD in the table of legal modifiers. */
      code = tablematch (wordtab, word);

      /*** Legal or not, remove (replace) this WORD in STR. */
      if (code<0  ||  reptab[code]==NULL) {
         if (chxalter (str, pos, word, CQ("")))  --i;
      }
      else  chxalter (str, pos, word, reptab[code]);

      if      (code == -2)  ambiguous (word);

      else if (code == -1) {
         mdwrite    (XWER, "gen_Tbadmod", null.md);
         unit_write (XWER, word);
         unit_write (XWER, ss_dotnl);
      }

      else   flagtab[code] = 1;
   }
   SUCCEED;

 done:

   chxfree ( word );

   RETURN ( success );
}
