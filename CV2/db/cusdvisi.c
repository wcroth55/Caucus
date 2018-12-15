/*** CUS_DVISIB.   Customize the conference visibility.
/
/    ok = cus_dvisib (cnum, cname, value);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    cnum;        (conference number)
/       Chix   cname;       (conference name)
/       int    value;       (new value of visibility attribute)
/
/    Purpose:
/       Cus_dvisib() is the one true way of modifying the conference
/       visibility attribute.  (It is always called from Cus_dattr(),
/       however.)
/
/    How it works:
/       VALUE of 1 makes the conference "visible" or "listed".  A
/       VALUE of 0 makes it "invisible" or "unlisted".
/
/       Cus_dvisib() modifies both the conference local name and the
/       conference "true" name.  If this is a distributed conference
/       and MISC/confs001 does not have a true name entry for this
/       conference, one is added.
/
/       Cname must be the name of conference CNUM.
/
/       Cus_dvisib() does not alter the file library flag.
/
/    Returns: 1 on success
/             0 on any error condition
/
/    Error Conditions:
/  
/    Related functions:
/
/    Called by:  cus_dattr()
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/cusdvisi.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/19/91 14:56 New function. */
/*: CR 11/21/91 17:14 Use truename@orighost in setvisib() call. */
/*: WC 11/29/91 20:45 Expand capability of set_visib(). */
/*: DE  5/26/92 11:14 Fix chxformat args */
/*: JX  5/26/92 15:07 Chix clean. */
/*: CP  5/06/92 12:41 Add L(0) as 'item' arg to modnames() call. */
/*: JZ  9/28/93 15:29 Add file library flag. */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"
#include "derr.h"

extern union null_t null;

FUNCTION  cus_dvisib (cnum, cname, value)
   int    cnum, value;
   Chix   cname;
{
   Chix   localname, truename, orighost;
   int    success;

   ENTRY ("cus_dvisib", "");

   localname  = chxalloc (L(160), THIN, "cusdvisi localname");
   truename   = chxalloc (L(160), THIN, "cusdvisi truename");
   orighost   = chxalloc (L(160), THIN, "cusdvisi orighost");
   success    = 0;

   if (NOT get_trueconf (cnum, localname, truename, orighost))
      chxcpy (localname, cname);
   
   if (NOT set_visib (cnum, localname, value, 1))     FAIL;

   if (NOT EMPTYCHX (truename)) {
      chxcat (truename, CQ("@"));
      chxcat (truename, orighost);
      if (NOT set_visib (cnum, truename,  value, 0))  FAIL;
   }

   SUCCEED;

 done:
   chxfree (localname);
   chxfree (truename);
   chxfree (orighost);

   RETURN (success);
}

FUNCTION  set_visib (cnum, cname, value, local)
   int    cnum, value, local;
   Chix   cname;
{
   Namelist names, add, del, p, matches, nlnode();
   Chix     temp;
   int      success, lib;

   ENTRY ("set_visib", "");

   temp     = chxalloc (L(160), THIN, "set_visib temp");
   success  = 0;

   if (EMPTYCHX (cname))  FAIL;

   names = nlnode (1);
   nladd (names, cname, 0);
   matchnames (XSCD, XSCN, 0, nullchix, names, AND, &matches, 0);
   nlfree (names);

   /*** CNAME should have 0 or 1 exact matches in MATCHES.   Find it. */
   for (p=matches->next;   p!=null.nl;   p = p->next) {
      chxtoken  (temp, nullchix, 1, p->str);
      if (chxeq (temp, cname)) break;
   }

   /*** Change the visibility to open or closed, depending on VALUE, delete
   /    the old value, and insert the new.  Save the old file library flag. */
   del = nlnode (1);
   if (p != null.nl) {
      nladd (del, p->str, 0);
      chxtoken (temp, nullchix, 2, p->str);
      lib = (chxvalue (temp, L(8)) == 'L');
   }
   add = nlnode (1);
   chxclear (temp);
   chxformat (temp, chxquick("%s %s#%03d",0), L(cnum), L(0), cname, 
            (value ? chxquick("o",1) : chxquick("c",2)));
   chxformat (temp, chxquick("#%s#%s",0), L(0), L(0),
              (local ? chxquick("L",1) : chxquick("T",2)), 
              (lib   ? chxquick("L",3) : chxquick("C",4)));
   nladd (add, temp, 0);
   success = modnames (XSCD, XSCN, nullchix, 0, L(0), add, del, 0);
   nlfree (add);
   nlfree (del);

   nlfree (matches);

   SUCCEED;

 done:
   chxfree (temp);

   RETURN (success);
}
