
/*** NR_TO_RLIST.   Translate a named range to an Rlist of items.
/
/    rl = nr_to_rlist (range, cnum, first, lastitem);
/
/    Parameters:
/       Rlist  rl;          (returned Rlist)
/       Chix   range;       (string containing named range)
/       int    cnum;        (conference number)
/       int    first;       (1st resp # for unspecified items)
/       int    lastitem;    (number of last item in conference CNUM)
/
/    Purpose:
/       Nr_to_rlist() takes a named range string RANGE, and translates
/       it into an Rlist of items.
/
/    How it works:
/       Nr_to_rlist() knows how to translate the named ranges AUTHOR,
/       TITLE, PSUBJECT, OSUBJECT, and (item) SINCE.  It returns a newly
/       created Rlist for them.  (Note: if the named range does
/       not contain any items, nr_to_rlist() "successfully" returns 
/       an empty Rlist.)
/
/    Returns: new Rlist on success
/             NULL on error.
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  new_parse()
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  gen/nrtorlis.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CP  6/14/93 10:06 New function. */
/*: CP  6/22/93  0:05 Many changes. */
/*: CP  7/16/93 16:32 Remove unused 'c'. */
/*: CP  8/01/93 10:50 Add a_mak...() name arg. */
/*: CP  8/03/93 13:23 Add SINCE code, lastitem arg. */
/*: CK 10/07/93 14:26 More comments. */
/*: CB  1/18/94 17:43 Declare nlnames(), nlnode() in nr_author(). */

#include <stdio.h>
#include "caucus.h"
#include "api.h"

extern union null_t           null;

FUNCTION  Rlist  nr_to_rlist (range, cnum, first, lastitem)
   Chix   range;
   int    cnum, first, lastitem;
{
   static   Chix data_fields = nullchix;
   Rlist    rl, rlptr, rlold, add_to_rlist(), subparse(), and_isince();
   Namelist matches, p, names, nr_author(), nlnames(), nlnode();
   Chix     keyword, data, itemstr;
   int4     item;
   int      field;

   ENTRY ("nr_to_rlist", "");

   rl = rlptr = a_mak_rlist("nrtorlist rl");
   keyword = chxalloc (L(20), THIN, "nr_to_rl keyword");
   data    = chxalloc (L(60), THIN, "nr_to_rl data");
   itemstr = chxalloc (L(20), THIN, "nr_to_rl itemstr");

   /*** Get the dictionary string for named ranges. */
   mdtable ("gen_Anpdata", &data_fields);

   chxtoken (keyword,  nullchix, 1, range);
   chxtoken (nullchix, data,     2, range);
   if (EMPTYCHX (data))  goto done;

   field = tablematch (data_fields, keyword);

   switch (field) {
      case (0):  /* AUTHOR */
         /*** Get the namelist of userids (and item#s they created)
         /    from the author name.  Scan the list and add each item
         /    to the growing Rlist. */
         matches = nr_author (data, cnum);
         for (p=matches->next;   p!=null.nl;   p = p->next) {
            chxtoken (itemstr, null.chx, 2, p->str);
            chxnum   (itemstr, &item);
            rlptr = add_to_rlist (rlptr, (int) item, first, -1);
         }
         nlfree (matches);
         break;
  
      case (1):  /* TITLE */
         /*** Get the namelist of title words (and associated item#s)
         /    that match the given title.  Scan the list and add each 
         /    item to the growing Rlist. */
         names = nlnames (data, null.chx);
         matchnames (XCTD, XCTN, cnum, null.chx, names, AND, &matches, NOPUNCT);
         nlfree (names);
         for (p=matches->next;   p!=null.nl;   p = p->next) {
            chxtoken (itemstr, null.chx, 2, p->str);
            chxnum   (itemstr, &item);
            rlptr = add_to_rlist (rlptr, (int) item, first, -1);
         }
         nlfree (matches);
         break;

      case (2):  /* OSUBJECT */
         rlptr = subparse (rlptr, data, cnum, 0, first);
         break;

      case (3):  /* PSUBJECT. */
         rlptr = subparse (rlptr, data, cnum, 1, first);
         break;

      case (4):  /* SINCE. */
         /*** Make RL list of ALL items, then "and" with since RANGE. */
         rlptr = add_to_rlist (rlptr, 1, first, -1);
         rlptr-> i1 = lastitem;
         rlold = rl;
         rl = and_isince (rlold, range, cnum);
         a_fre_rlist (rlold);
         break;

      default:   /* unknown named range types. */
         a_fre_rlist (rl);
         rl = (Rlist) NULL;
         break;
   }

done:
   chxfree (keyword);
   chxfree (data);
   chxfree (itemstr);
   RETURN  (rl);
}

FUNCTION  Namelist nr_author (value, cnum)
   Chix   value;
   int    cnum;
{
   Namelist names, p;
   Namelist matches = null.nl;
   Chix     userid;
   int      c;
   Namelist nlnames(), nlnode();

   ENTRY ("nr_author", "");

   userid = chxalloc (L(20), THIN, "nr_author");

   /*** Get the userids that match people's names from VALUE. */
   names = nlnames (value, null.chx);
   c = matchnames (XSND, XSNN, 0, null.chx, names, AND, &matches, NOPUNCT);
   nlfree (names);

   if (c > 0) {
      /*** Create a namelist of these userids. */
      names = nlnode (1);
      for (p=matches->next;   p!=null.nl;   p = p->next) {
         chxtoken (userid, null.chx, 2, p->str);
         nladd (names, userid, 0);
      }
      nlrdup (names, 1);
      nlfree (matches);

      /*** Match the userid's against the userid/item# partfiles. */
      c = matchnames (XCAD, XCAN, cnum, null.chx, names, OR, 
                           &matches, NOPUNCT);
      nlfree (names);
   }

   chxfree (userid);
   RETURN  (matches);
}
