/*** CUS_VISIBLE.  CUSTOMIZE command, VISIBILITY option.
/
/    Is this a visible conference, i.e. should it appear
/    in the list of publicly visible conferences?
/
/    The global CONFNAME is the Local Confname of the conference.
/
/    Return 1 if successful, 0 if an error occurred. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/06/89 14:19 New function. */
/*: CR  8/28/90 14:32 Replace nlnames() call with nlnode()/nladd(). */
/*: CR  3/29/91 16:37 Use AND/OR in matchnames() call. */
/*: CR  7/27/91 17:37 Use cus_dattr() to actually change visibility. */
/*: CR  7/30/91 13:51 Replace license check with is_distributed(). */
/*: CR  8/02/91 14:00 Add punctuation argument to matchnames() call. */
/*: JV 11/01/91 14:59 Deal with Local Confname vs. True Confname. */
/*: CR 11/07/91 14:18 Add confnum arg to matchnames() call. */
/*: JV 11/13/91 22:57 Add dest_chn to cusxattr(). */
/*: DE  3/16/92 17:40 Chixified */
/*: JX  5/19/92 20:37 Fix Chix. */
/*: JX  8/03/92 10:12 CHN's are chix. */
/*: CL 12/11/92 13:46 Long Chxvalue N. */
/*: JZ  9/28/93 17:03 Fix Local vs True bug. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"

extern Chix                    confname;
extern short                   confnum;
extern union  null_t           null;

FUNCTION  cus_visible (answer)
   Chix   answer;
{
   static Chix pubtable;
   struct namlist_t *names, *add, *nlnode();
   Chix   def, value;
   int    result, visible, success, objgiven, error1, error2;

   ENTRY ("cus_visible", "");

   def       = chxalloc (L(40), THIN, "cusvisib def");
   value     = chxalloc (L(40), THIN, "cusvisib value"); 
   success   = 0;

   /*** Get the current entry for this conference. */
   names = nlnode (1);
   nladd  (names, confname, 0);
   matchnames (XSCD, XSCN, 0, nullchix, names, AND, &add, NOPUNCT);
   nlfree (names);
   if (nlsize (add) == 0) {
      mdwrite (XWTX, "chg_Ebadcname", null.md);
      nlfree  (add);
      FAIL;
   }
   /*** If there were two matches, make sure we're using the Local one. */
   if (nlsize (add) == 2) {
      chxtoken (value, nullchix, 2, add->next->str);
      if (chxvalue (value, L(6)) != 'L')
         chxcpy (add->next->str, add->next->next->str);
   }

   /*** Decide if it's open or closed, and ask the user what to
   /    change it to.  Default is current status. */
   chxtoken  (value, nullchix, 2, add->next->str);
   visible = (chxvalue(value,0L) == 'o' ? 1 : 0);
   mdtable   ("cus_Acuspub", &pubtable);
   chxtoken  (def, nullchix, visible+1, pubtable);
   objgiven = (EMPTYCHX (answer) ? 0 : 3);
   nlfree (add);
   result   = mdprompter ("cus_Pcuspub", pubtable, 2 + objgiven, "cus_Hcuspub", 
                                       10, answer, def);
   if (result < 0)         FAIL;
   if (result == visible)  SUCCEED;

   success = cus_dattr (confnum, confname, CUS_VISIBILITY, result, &error1);
   if (success  &&  is_distributed (confnum))
      cusxattr (confnum, CUS_VISIBILITY, result, nullchix, &error2);

 done:

   chxfree (def);
   chxfree (value);

   RETURN (success);
}
