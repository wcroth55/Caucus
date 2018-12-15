
/*** PWFIND.  Find a person in the password file.
/
/    PWFIND is handed the NAME of the person we're trying to find
/    in the password file.  If PWFIND cannot find that name, it
/    returns 0.  If it does find NAME, it returns 1, and sets PASSWORD
/    to the encryption of that user's password as recorded in the
/    password file. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:42 Source code master starting point*/
/*: CR  3/29/91 17:29 Use AND/OR code in matchnames() call. */
/*: CR  8/02/91 14:00 Add punctuation argument to matchnames() call. */
/*: CR 11/07/91 14:18 Add confnum arg to matchnames() call. */
/*: DE  4/01/92 18:23 Chixified */

#include <stdio.h>
#include "caucus.h"

extern union null_t null;

FUNCTION  pwfind (name, password)
   int4   *password;
   Chix   name;
{
   struct namlist_t *who, *matches, *p, *nlnode();
   Chix   word, tempchx;
   int    success;

   ENTRY ("pwfind", "");

   word     = chxalloc (L(50), THIN, "pwfind word");
   tempchx  = chxalloc (L(40), THIN, "pwfind tempchx");
   success  = 0;

   who = nlnode(1);
   nladd  (who, name, 0);
   matchnames (XSPD, XSPN, 0, null.chx, who, AND, &matches, NOPUNCT);
   nlfree (who);

   for (p=matches->next;   p!=null.nl;   p = p->next) {
      chxtoken  (word, null.chx, 1, p->str);
      if (chxeq (word, name)) {
         chxtoken (word, null.chx, 2, p->str);
         chxtoken (tempchx, null.chx, 1, word);
         chxnum   (tempchx, password);
         nlfree   (matches);
         SUCCEED;
      }
   }

   nlfree (matches);
   FAIL;

 done:

   chxfree ( word );
   chxfree ( tempchx );

   RETURN ( success );
}


