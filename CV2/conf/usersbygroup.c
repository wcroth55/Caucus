
/*** users_by_group.   Add users in a group to userlist.
/
/    users_by_group (userlist, groupfile, grouphistory);
/
/    Parameters:
/       Chix     userlist;  (append results to this space-separated string)
/       Chix     groupfile; (name of groupfile)
/
/    Purpose:
/       Scan through all the userids in this 'groupfile' and add them
/       to the end of userlist.  Recursively scan any sub-groups 
/       as needed.
/
/    How it works:
/       When called recursively, add groupfile onto the end of
/       grouphistory.  Return immediately if groupfile is found
/       *in* grouphistory.
/
/    Returns:  1
/  
/    Known bugs:      none
/
/    Home:  conf/usersbygroup.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 08/01/04 New function, slightly derived from groupmatch. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "done.h"

FUNCTION  int  users_by_group (Chix userlist, Chix groupfile, 
                               Chix grouphistory) {
   Chix   recurse_files, word, temp, history;
   int    pos;

   ENTRY ("users_by_group", "");

   if (tablefind (grouphistory, groupfile) >= 0)  RETURN(0);

   recurse_files = chxalloc (L(200), THIN, "recurse_files");
   history       = chxalloc (L(200), THIN, "history");
   word          = chxalloc (L(100), THIN, "word");
   temp          = chxalloc (L(100), THIN, "temp");

   if (unit_lock (XSUG, READ, L(0), L(0), L(0), L(0), groupfile)) {
      if (unit_view (XSUG)) {
         while (unit_read (XSUG, temp, L(0))) {
            if (chxvalue (temp, L(0)) == L('#'))  continue;
            chxtoken (word, nullchix, 1, temp);
            if (chxvalue (word, L(0)) == 0)       continue;

            if (chxvalue (word, L(0)) == L('<')) {
               chxcatsub (recurse_files, word, 1, 1000);
               chxcatval (recurse_files, THIN, ' ');
            }
            else if (tablefind (userlist, word) < 0) {
               chxcat    (userlist, word);
               chxcatval (userlist, THIN, ' ');
            }
         }
         unit_close (XSUG);
      }
      unit_unlk (XSUG);
   }

   chxcpy    (history, grouphistory);
   chxcatval (history, THIN, ' ');
   chxcat    (history, groupfile);
   chxcatval (history, THIN, ' ');
   for (pos=0;   chxnextword (word, recurse_files, &pos) >= 0;  ) {
      users_by_group (userlist, word, history);
   }

done:
   chxfree (temp);
   chxfree (word);
   chxfree (history);
   chxfree (recurse_files);
   RETURN  (1);
}
