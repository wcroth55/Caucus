
/*** CREATE_CONF.   Create a new conference: $create_conf ()
/
/    create_conf (result, arg, conf);
/   
/    Parameters:
/       Chix          result;  (append resulting value here)
/       Chix          arg;     (function argument)
/       SWconf        conf;    (server configuration info)
/
/    Purpose:
/       Implement CML function $create_conf (name priorg [2nd orgs])
/
/    How it works:
/       Evaluates to: new conference number
/                     0   if no permission
/                    -1   if conf exists already
/                    -2   no primary org id supplied
/                    -3   database error
/                    -4   bad name
/
/    Returns: 
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  createco.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/04/97 17:36 New function. */

#include <stdio.h>
#include "unitcode.h"
#include "done.h"
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

extern int   cml_debug;

FUNCTION  create_conf (Chix result, Chix arg, SWconf conf)
{
   int    cnum, success, op, error, pos;
   char   temp[100], ascname[100];
   Chix   name, word, priorg, orgs, lowname, cnums;
   ENTRY ("create_conf", "");

   name    = chxalloc (L(40), THIN, "create_co name");
   lowname = chxalloc (L(40), THIN, "create_co lowname");
   cnums   = chxalloc (L(40), THIN, "create_co cnums");
   word    = chxalloc (L(40), THIN, "create_co word");
   priorg  = chxalloc (L(40), THIN, "create_co priorg");
   orgs    = chxalloc (L(80), THIN, "create_co orgs");

   /*** Make sure this user can create confs. */
   if (mgr_in_group ("conferenceCreate conferenceAllPrivileges "
                     "systemAllPrivileges") == 0)                DONE(0);
   
   /*** If this conference already exists, return -1. */
   chxtoken   (name, nullchix, 1, arg);
   chxcpy     (lowname, name);
   jixreduce  (lowname);
   if (okconf (name) <= 0)  DONE(-4);
   func_cl (cnums, "cl_list", name, conf);

   for (pos=0;   chxnextword (word, cnums, &pos) >= 0;   ) {
      cnum = atoi(ascquick(word));
      cl_name  (ascname, 100, cnum);
      strlower (ascname);
      if (streq (ascname, ascquick(lowname)))  DONE(-1);
   }

   /*** Get the primary org. */
   chxtoken (priorg, nullchix, 2, arg);
   if (chxvalue (priorg, L(0)) == 0)  DONE(-2);

   /*** Assemble the list of other organizers. */
   chxtoken (nullchix, orgs, 3, arg);

   success = make_conf (name, priorg, 0, 0, orgs, &error);
   if (NOT success)  DONE(-3);

done:
   sprintf (temp, "%d", success);
   chxcat  (result, CQ(temp));

   chxfree (name);
   chxfree (lowname);
   chxfree (cnums);
   chxfree (word);
   chxfree (priorg);
   chxfree (orgs);
   RETURN  (1);
}
