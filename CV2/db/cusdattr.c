/*** CUS_DATTR.   Customize conference ATTRibutes.
/
/    ok = cus_dattr (cnum, cname, attr, value, error);
/
/    Parameters:
/       int    ok;          (Success?)
/       int    cnum;        (conference number)
/       Chix  *cname;       (conference name)
/       int    attr;        (attribute code: CUS_ADD, CUS_CHANGE, CUS_VISIB...)
/       int    value;       (new value of that attribute)
/       int   *error;       (return error code)
/
/    Purpose:
/       Cus_dattr() is the one true way of modifying the conference
/       attributes CUS_ADD, CUS_CHANGE, CUS_VISIBILITY, CUS_SUBJECT.
/       All Caucus and CaucusNet code that change these attributes 
/       must call this function.
/
/    How it works:
/       The precise meaning of VALUE depends on the attribute:
/
/       Attribute:        1                              0
/       CUS_ADD         users can add items        users cannot add items
/       CUS_CHANGE      users can change resp's    users cannot change resp's
/       CUS_VISIBILITY  conference is visible      conference is invisible
/       CUS_SUBJECT     users can change subjects  users cannot change subjects
/       CUS_ATTACH      users can add attachments  users cannot add attach's
/
/       Cname must be the name of conference CNUM.
/
/    Returns: 1 on success, sets ERROR to 0.
/             0 on any error condition, see below.
/
/    Error Conditions: sets ERROR accordingly:
/       DNOCONF     conference CNUM does not exist
/       DBADACT     ATTR has a bad attribute value
/  
/    Side effects: reloads global "master" structure.
/
/    Related functions:
/
/    Called by:  Caucus functions, XFU
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  db/cusdattr.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/27/91 17:25 New function. */
/*: CR  8/02/91 12:44 Add NOPUNCT as new last arg to modnames() call. */
/*: CR  8/02/91 14:00 Add punctuation argument to matchnames() call. */
/*: CR 11/07/91 14:18 Add confnum arg to matchnames() call. */
/*: CR 11/14/91 11:49 Pass master as arg to store_master() call. */
/*: CR 11/18/91 17:20 Add CNUM argument to modnames() call. */
/*: JV 11/18/91 17:42 Change NOPUNCT to 0. */
/*: CR 11/19/91 16:10 Split off cus_dvisib(). */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: CR  1/02/92 15:03 Add CUS_SUBJECT stuff. */
/*: JX  5/26/92 14:51 Chixify header. */
/*: JV  2/24/93 16:43 Add CUS_ATTACH. */
/*: JV  6/11/93 10:50 Fix CUS_ATTACH. */
/*: CP  2/07/94 16:06 Fix 'if (attr = CUS_VISIBILITY)...' --> "==". */

#include <stdio.h>
#include "caucus.h"
#include "xaction.h"
#include "derr.h"

extern struct master_template master;

FUNCTION  cus_dattr (cnum, cname, attr, value, error)
   int    cnum, attr, value;
   Chix   cname;
   int   *error;
{
   ENTRY ("cus_dattr", "");

   *error = NOERR;

   /*** CUS_CHANGE and CUS_ADD load the XCMA file, change it, and
   /    write it back out again. */
   if (attr == CUS_CHANGE  ||  attr == CUS_ADD  ||
       attr == CUS_SUBJECT ||  attr == CUS_ATTACH) {

      if (NOT unit_lock (XCMA, WRITE, L(cnum), L(0), L(0), L(0), nullchix)) 
                                                         *error = DNOCONF;
      else {
         if (NOT srvldmas (0, &master, cnum))            *error = DNOCONF;
         else {
            switch (attr) {
               case (CUS_CHANGE):   master.nochange = (value == 0);   break;
               case (CUS_ADD):      master.noenter  = (value == 0);   break;
               case (CUS_SUBJECT):  master.chgsubj  =  value;         break;
               case (CUS_ATTACH):   master.allow_attach  =  value;    break;
            }
            store_master (&master);
         }
         unit_unlk (XCMA);
      }
   }
         
   else if (attr == CUS_VISIBILITY) {
      if (NOT cus_dvisib (cnum, cname, value))  *error = DBADDB;
   }

   else *error = DBADACT;

   RETURN (*error == NOERR);
}
