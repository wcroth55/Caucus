
/*** PLUCK_PERSON.   Pluck a "PERSON name..." string off a command line.
/
/    PLUCK_PERSON looks for a person name specification of the form
/    "PERSON name1 name2..." in the command line COMMAND.  If it finds
/    one, it removes it from COMMAND, and puts the names into NAME,
/    and returns 1.  Otherwise, it returns 0.
/
/    If no names follow the PERSON keyword, PLUCK_PERSON still returns
/    1, and sets NAME to the empty string.
/
/    PLUCK_PERSON was created to help parse commands such as
/    "SHOW ITEM range PERSON names".  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/10/89 15:02 New function. */
/*: CR  4/18/90 23:05 Expand tag to reduce conflict with 8bit. */
/*: CR  6/26/90 10:15 Fix bug: remove entire TAG, not just 1st char! */
/*: DE  4/01/92 17:47 Chixified */
/*: JX  5/27/92 16:18 Fixify. */
/*: JX  7/01/92 10:54 Fixify. */
/*: CR 10/09/92 17:10 Removed unused 's' (and allocation!) */
/*: CL 12/10/92 14:52 Long chxalter args. */
/*: CL 12/10/92 15:22 Long chxindex args. */
/*: CL 12/11/92 11:17 Long Chxcatsub args. */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;
extern Chix         objectable, ss_qt1, ss_qt2;

FUNCTION  pluck_person (command, name)
   Chix   command, name;
{
   static Chix tag;
   Chix   keep[2], word;
   int    found, quote, i;
   int4   n;
   Chix   tempchx;
   int    success;

   ENTRY ("pluck_person", "");

   if (NOT chxtype(tag)) {
      tag   = chxalloc (L(5),   THIN, "pluck_person tag");
      chxofascii (tag, "\004\004\004");
    }

   word     = chxalloc (L(80),  THIN, "pluck_person word");
   keep[0]  = chxalloc (L(200), THIN, "pluck_person keep0");
   keep[1]  = chxalloc (L(200), THIN, "pluck_person keep1");
   success  = 0;

   /*** Pull out any quoted string, so as to not confuse the searching
   /    for PERSONS. */
   quote   = chxquote (command, 0, keep);

   /*** Scan the COMMAND line for the PERSONS keyword.  If found, replace
   /    it with a special TAG so we can (uniquely) find it later, after
   /    we've put back the (possible) quoted string. */
   found   =   0;
   chxclear ( name );
   for (i=1;   (chxtoken (word, nullchix, i, command)) != -1;   ++i) {
      if (tablematch (objectable, word) == 5) {
         found = 1;
         chxalter (command, L(0), word, tag);
         break;
      }
   }

   /*** Replace a quoted string if we found one earlier. */
   if (quote)  chxquote (command, 1, keep);

   /*** If PERSON was found, remove the names from the command line, and
   /    put them in NAME.  Remove the PERSON tag altogether. */
   if (found  &&  (n = chxindex (command, L(0), tag)) >= 0) {
      chxclear  (name);
      chxcatsub (name, command, (n + chxlen(tag)), ALLCHARS);
      tempchx = chxalsub (command, L(0), n);
      chxcpy    (command, tempchx);
      chxfree   (tempchx);

      /*** Remove any quotes from NAME. */
      while (chxalter (name, L(0), ss_qt1, CQ(""))) ;
      while (chxalter (name, L(0), ss_qt2, CQ(""))) ;
      chxsimplify     (name);
   }

   DONE(found);

 done:

   chxfree (keep[0]);
   chxfree (keep[1]);
   chxfree (word);

   RETURN ( success );
}
