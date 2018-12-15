/*** A_GET_PEOPLE.   Get a list of people whose name matches a substring.
/
/    ok = a_get_people (people, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       People    people;   (object to fill with linked list of userid's)
/       int4      mask;     (mask of properties to get)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Place a linked list of personal names into PEOPLE.
/
/    How it works:
/
/    Returns: A_OK on success
/             A_DBERR on database error
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/       This function should be modified to allow UI to request ALL
/       users, regardless of conf.  That will probably require loading
/       the entire namesNNN fileset, and then using nlsort and nlunique
/       to create an accurate list.  This method will be *very* slow,
/       since namelists are linked lists.  Maybe there's a better method.
/       There certainly are *faster* methods.
/
/    Home:  api/agetpeop.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV 12/14/92 15:53 Create function. */
/*: CR  1/13/93 14:24 Use success value for return! */
/*: CP  8/01/93 14:32 Add 'name' arg to make_textbuf() call. */
/*: CR  4/12/04 Comment out non-choice==0 code. */

#include <stdio.h>
#include "handicap.h"
#include "chixuse.h"
#include "api.h"
#include "null.h"
#include "textbuf.h"
#include "unitcode.h"
#include "done.h"

extern short           confnum;
extern union  null_t   null;

FUNCTION  a_get_people (People people, int4 mask, int wait)
{
   int     success, inconf=0, savecnum, choice=0, priv;
   short   i, ok;
   Chix    id;
   Namelist nluserid(), namdir,  nlnode(), this;
   Textbuf  tbuf, make_textbuf();

   id = chxalloc (L(16), THIN, "agetpeop id");
   savecnum = confnum;

   if (people == (People) NULL  ||  people->tag != T_PEOPLE) DONE (A_BADARG);

   tbuf = make_textbuf (TB_RESPS, TB_LINES, "agetpeop tbuf");

   /*** Empty PEOPLE, in case we were passed an old one by the caller. */
   if (people->ids != null.nl)   nlfree (people->ids);
   people->ids  = nlnode (1);
   people->ready = 1;

   if (people->cnum != 0) {
      confnum = people->cnum;
      inconf = 1;
   }

   if (NOT EMPTYCHX (people->namepart))
      people->ids = nluserid (people->namepart, inconf);

   /*** User wants *ALL* members of the conf. */
   else {
      if (NOT unit_lock (XCMD, READ, L(confnum), L(0), L(0), L(0), nullchix))
         DONE (A_DBERR);
      namdir = nlnode(4);
      ok = loadnmdir (XCMD, namdir);
      unit_unlk (XCMD);
      if (NOT ok)     { nlfree (namdir);           DONE (A_DBERR); }
 
      /*** Load each namepart file in turn, and display contents. */
      sysbrkclear();
      for (this=namdir->next;   this != null.nl;   this = this->next) {
         if (sysbrktest())    break;
         if (this->num <= 0)  continue;
         if (NOT unit_lock (XCMN, READ, L(confnum), L(this->num), L(0), L(0),
                            nullchix))
            continue;
         if (NOT loadnpart (XCMN, this->num, tbuf))
                                                { unit_unlk (XCMN); continue; }         unit_unlk (XCMN);                         

         for (i=0;   i<tbuf->l_used;   ++i) {
            if (sysbrktest())    break;
            if (chxtoken (id,  nullchix, 2, tbuf->line[i]) == -1) continue;
            if (choice == 0) {                             /* ALL */
               nlcat (people->ids, id, 0);
               continue;
            }
 
/*** Looks like this code is irrelevant, since choice is always 0. */
#if 0
            priv = allowed_on (id, null.nl, confnum);
            if (priv == 3 && choice == 1 ||           /* Organizer */
                priv == 2 && choice == 2 ||           /* Include   */
                priv == 1 && choice == 3 ||           /* Readonly  */
                priv == 0 && choice == 4)             /* Exclude   */
               nlcat (people->ids, id, 0);
#endif
         }
      }
   }
   DONE (A_OK);

done:
   confnum = savecnum;
   chxfree (id);
   free_textbuf (tbuf);
   return (success);
}
