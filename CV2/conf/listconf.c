/*** LISTCONFS.   Display list of conferences on OUT. 
/
/  listconfs (out, manager)
/
/  Parameters:
/    int    out         (Unit to which to display)
/           manager     (controls whether to display confs. see below)
/
/  Purpose:
/    Go through the list of conferences on this system, and attempt to
/    display their names, up to 4 per line.  Display a particular
/    conference name if it is a Local ConfName and:
/      (a) MANAGER is true;
/ or   (b) the conference is visible;
/ or   (c) the user is a member of the conference;
/ or   (d) the user could become a member of the conference.
/
/  How it works:
/
/  Returns:
/
/  Error Conditions:
/
/  Side Effects:
/
/  Related to:
/
/  Called by: 
/
/  Home: 
*/

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: JV  8/12/89 21:54 Fixed ENTRY name! */
/*: CW  5/18/89 14:25 Add XW unitcodes. */
/*: CR  9/06/89 14:14 Skip display of Closed conferences. */
/*: CR 10/05/89 16:36 Test for <CANCEL> in display of conf names. */
/*: CR 10/09/89 15:02 Add "manager" argument. */
/*: CR  7/17/90 13:57 Add code for "(d)...could become a member..." */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: JV  5/15/91 17:46 Add new loadnpart arg. */
/*: CR  7/02/91  0:00 Include "unitwipe.h". */
/*: CR 10/29/91 13:07 Use confnum arg in allowed_on() call. */
/*: JV 11/01/91 14:23 Deal correctly with Local vs. TrueConf. */
/*: CR 11/15/91 14:26 Remove a_user, use thisuser.id in allowed_on() call. */
/*: JV 12/06/91 13:57 Fix Local vs. True. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: JX  3/09/92 09:39 Chixify, add header. */
/*: DE  5/26/92 13:10 Fixed structure reference, inconsistant args */
/*: JX  5/26/92 13:53 Fix free_textbuf. */
/*: JX  8/10/92 11:37 Flush allowed_on's ulist cache. */
/*: JX 10/01/92 14:50 Allocate and clear codetmp. */
/*: CR 11/15/92 21:47 Free codetmp. */
/*: CR 12/11/92 11:30 Long chxcatsub() args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CP  8/01/93 14:32 Add 'name' arg to make_textbuf() call. */

#include <stdio.h>
#include "caucus.h"
#include "unitwipe.h"
#include "chixuse.h"
#include "done.h"

extern union  null_t        null;
extern Userreg       thisuser;
extern short         confnum;
extern Chix          confname;

FUNCTION  listconfs (out, manager)
   int    out, manager;
{
   struct namlist_t *namdir, *this, *groups, *nlnode();
   int    member, savecnum, success;
   short  i, ok, cpl;
   Chix   name, code, codetmp, message;
   int4   cnum;
   Textbuf tbuf, make_textbuf();

   ENTRY ("listconfs", "");

   name     = chxalloc (L(80),  THIN, "name");
   code     = chxalloc (L(20),  THIN, "code");
   codetmp  = chxalloc (L(16),  THIN, "code");
   message  = chxalloc (L(200), THIN, "code");

   tbuf = make_textbuf (TB_RESPS, TB_LINES, "listconf tbuf");

   /*** Load the Conference names directory. */
   if (NOT unit_lock (XSCD, READ, L(0), L(0), L(0), L(0), nullchix))  FAIL;

   namdir = nlnode(4);
   ok = loadnmdir (XSCD, namdir);
   unit_unlk (XSCD);
   if (NOT ok)  { nlfree (namdir);               FAIL; }

   /*** Load each Conference namepart file in turn, and if the user is
   /    appropriately permitted, display the name of each conference. */
   cpl = 0;
   unit_write (out, CQ("\n"));
   unit_wipe  (LCNF);
   groups = nlnode (1);
   sysbrkclear();
   savecnum = confnum;
   for (this=namdir->next;   this != null.nl;   this = this->next) {
      if (sysbrktest())  break;
      if (this->num <= 0)  continue;
      if (NOT unit_lock (XSCN, READ, L(0), L(this->num), L(0), L(0), nullchix))
          break;
      if (NOT loadnpart (XSCN, this->num, tbuf))
          { unit_unlk (XSCN); break; }
      unit_unlk (XSCN);
   
      /*** For each line in the conference name part file 
      /    (i.e, for each conference)... */
      for (i=0;   i<tbuf->l_used;   ++i) {
         if (sysbrktest())  break;
         chxtoken (name, nullchix, 1, tbuf->line[i]);
         chxclear (code);
         chxtoken (code, nullchix, 2, tbuf->line[i]);
         if (EMPTYCHX (code)  ||  chxvalue (name, L(0)) =='!')  continue;
 
         /*** Only check Local Confnames, but allow old syntax (6=='\0') */
         if (chxvalue (code, L(6)) == 'T') continue;
      
         /*** If this conference is closed... */
         if (NOT manager  &&  chxvalue (code, L(0)) == 'c') {
            chxclear  (codetmp);
            chxcatsub (codetmp, code, L(2), chxlen (code));
            if (NOT chxnum (codetmp, &cnum))  continue;
            confnum = cnum;
            member  = 0;
            
            /*** Display the name if the user is a member. */
            if (unit_lock (XUPA, READ, L(cnum), L(0), L(0), L(0), thisuser->id)) {
               member = unit_check (XUPA);
               unit_unlk (XUPA);
            }

            /*** Or, if not a member, could the user become a member? */
            if (NOT member) {
               allowed_on (nullchix, null.nl, -1);  /* Flush cache. */
               member = allowed_on (thisuser->id, groups, confnum);
               unit_wipe (LCNF);
            }
       
            confnum = -1;
            if (NOT member) continue;
         }

         chxclear  (message);
         chxformat (message, CQ("%-19s"), L(0), L(0), name, nullchix);
         unit_write (out, message);
         if (++cpl >= 4)  { cpl = 0;   unit_write (out, CQ("\n")); }
      }
   }
   sysbrkclear();
   nlfree (namdir);
   nlfree (groups);
   unit_write (out, CQ("\n"));
   confnum = savecnum;
   SUCCEED;

done:
   free_textbuf (tbuf);
   chxfree (name);
   chxfree (code);
   chxfree (codetmp);
   chxfree (message);

   RETURN (success);
}
