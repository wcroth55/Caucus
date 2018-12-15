
/*** FUNC_PAGE.   Handle $page_xxx()  functions.
/
/    func_page (result, what, arg);
/   
/    Parameters:
/       Chix       result;  (append resulting value here)
/       char      *what;    (function name)
/       Chix       arg;     (function argument)
/
/    Purpose:
/       Handle stack related functions:
/
/       $page_save (which pagename args #anchor title)
/        Returns index to page table where this page is saved.
/             which      (which arg in ARGS is page number to touch?)
/             pagename   name of page (e.g., "viewitem.cml")
/             args       list of args (e.g., $arg(2)+$arg(3)+...)
/             #anchor    anchor to save, else just "#"
/             title      title of page; any number of words, including none
/
/       $page_get  (n)
/        Returns entire page string (pagename args #anchor title)
/             n          index of saved page
/
/       $page_caller (which n)
/        Returns index of page which called page N
/             which      (which arg is index of caller page)
/             n          index of saved page
/
/       $page_return (n #override empty)
/        Returns full page address.
/             n          index of saved page
/             #override  overrides anchor if supplied
/             empty      use this if no page is saved at N
/
/    How it works:
/
/    Returns: void
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/funcpage.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/18/96 13:53 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

#define  PAGES  200

FUNCTION  func_page (Chix result, char *what, Chix arg)
{
   static struct { Chix name;   int4 touch; } pages[PAGES];
   static int4   touched = L(0);
   static int    next    = 0;
   static Chix   word;
   int           slot, touch_me, this, na;
   char          temp[200];
   int4          pos, cval;
   int4          unique();
   ENTRY ("func_page", "");
   
   /*** Initialization. */
   if (touched == L(0)) {
      for  (slot=0;   slot<PAGES;   ++slot) {
         pages[slot].name  = chxalloc (L(60), THIN, "func_page name[]");
         pages[slot].touch = L(0);
      }
      touched = PAGES;
      word = chxalloc (L(20), THIN, "func_page word");
   }
   
   /*** $page_get (n)  */
   if (streq (what, "page_get")) {
      pos  = L(0);
      slot = chxint4 (arg, &pos);
      chxcat (result, pages[slot].name);
      pages[slot].touch = ++touched;
   }

   /*** $page_caller (arg n) */
   else if (streq (what, "page_caller")) {
      pos = L(0);
      touch_me = chxint4 (arg, &pos);
      slot     = chxint4 (arg, &pos);

      pages[slot].touch = ++touched;
      chxtoken (word, nullchix, 2, pages[slot].name);

      for (pos=this=0, na=1;   na <= touch_me;   ++na) {
         this = chxint4 (word, &pos);
         while ( (cval = chxvalue (word, pos))  &&  (cval != '+') )  ++pos;
      }
      sprintf (temp, "%d", this);
      chxcat  (result, CQ(temp));
   }


   /*** $page_save (touch pagename args #anchor title) */
   else if (streq (what, "page_save")) {
      pos = L(0);

      /*** "Touch" a page if it is referenced by the page being saved. */
      touch_me = chxint4 (arg, &pos);
      chxtoken (word, nullchix, 3, arg);
      for (pos=this=0, na=1;   na <= touch_me;   ++na) {
         this = chxint4 (arg, &pos);
         while ( (cval = chxvalue (arg, pos))  &&  (cval != '+') )  ++pos;
      }
      if (this) pages[this].touch = ++touched;

      /*** Find the next slot in PAGES that is safe to (re-)use. */
      while (pages[next].touch > touched - PAGES/2)  
         next = (next + 1) % PAGES;

      /*** Save this page there.  */
      chxtoken (nullchix, pages[next].name, 2, arg);
      pages[next].touch = ++touched;

      /*** And return the page index. */
      sprintf (temp, "%d", next);
      chxcat  (result, CQ(temp));
   }


   /*** $page_return (n [#anchor] )  */
   else if (streq (what, "page_return")) {
      pos  = L(0);
      this = chxint4 (arg, &pos);
      pages[this].touch = ++touched;

      /*** Build the full page name: "name?unique+args#anchor". */
      chxtoken  (word, nullchix, 1, pages[this].name);
      if (chxvalue (word, L(0))) {
         chxcat    (result, word);
         chxcatval (result, THIN, L('?'));
         sprintf   (temp, "%d+", unique());
         chxcat    (result, CQ(temp));
         chxtoken  (word, nullchix, 2, pages[this].name);
         chxcat    (result, word);
         chxtoken  (word, nullchix, 2, arg);
         if (chxlen(word) <= 1)
            chxtoken (word, nullchix, 3, pages[this].name);
   
         if (chxlen (word) > 1)   chxcat (result, word);
      }

      /*** If no page saved, use default 'empty' value. */
      else {
         chxtoken (word, nullchix, 3, arg);
         chxcat   (result, word);
      }
   }
      

   RETURN (1);
}
