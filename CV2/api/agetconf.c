/*** A_GET_CONF.   Get basic information about a conference.
/
/    ok = a_get_conf (conf, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Conf      conf;     (object to fill with conference info)
/       int4      mask;     (mask of properties to get)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Load basic information about a conference into the CONF object.
/
/    How it works:
/       Caller must fill in conf->cnum.
/       Mask must be one or more of:
/          P_OWNER, P_FADD,  P_FCHANGE, P_FCHGSUB  (from masteres)
/          P_NAME,  P_FVIS,  P_FLIB                (from MISC/confsNNN)
/          P_TEXT,  P_GREET, P_USERLIST            (from XCIN, XCGR, XCUS)
/
/       Uses srvldmas(), get_trueconf(), and XCIN/XCGR/XCUS to get the
/       requested properties into CONF.
/
/    Returns: A_OK on success
/             A_DBERR + unit code # on database error
/
/    Error Conditions:
/      If more than one property is requested, if any one of them has
/      a database error, a_get_conf() returns A_DBERR -- even if the
/      rest of the properties are ok.
/ 
/    Side effects:
/      (a) For now, we use the global "master".  We may eventually want
/          to replace this with a master struct allocated & freed locally.
/
/    Related functions:
/
/    Called by:  UI
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/agetconf.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/15/92 17:47 New function. */
/*: CR 11/17/92 23:10 Complete function. */
/*: CR 11/19/92 20:40 Fix bugs. */
/*: CR 11/20/92 18:38 Add visibility code. */
/*: CR 11/27/92 19:18 Better argument checking. */
/*: CR 12/06/92 22:58 Clear conference access cache. */
/*: CR 12/07/92 10:44 Use 'nullint'. */
/*: CR 12/13/92 22:02 Add comments. */
/*: CL 12/11/92 13:46 Long Chxvalue N. */
/*: CR  2/22/93 23:31 Add new args to a_cache() calls. */
/*: JZ  9/28/93 15:05 Add support for file library. */
/*: JZ  9/28/93 16:45 Use 2nd word:6th char for Local, not 1st wrd:5th char. */
/*: JZ  9/29/93 10:50 Add more header comments. */
/*: JZ 10/01/93 10:53 DB Consistency Checking. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"

#define  nullint   ( (int *) NULL)

extern int             debug;
extern Chix            confid;
extern short           confnum;
extern union  null_t   null;

extern struct master_template master;

FUNCTION  a_get_conf (Conf conf, int4 mask, int wait)
{
   int      get_mast, success, size, errors;
   Chix     value, lname;
   Namelist names, add, nlnode();

   if (conf == (Conf) NULL  ||  conf->tag != T_CONF)  return (A_BADARG);
   if (conf->cnum <= 0)                               return (A_NOCONF);

   lname = chxalloc (L(20), THIN, "agetconf lname");

   get_mast = (mask & P_OWNER) || (mask & P_FCHANGE) || (mask & P_FADD) ||
              (mask & P_FCHGSUB);

   conf->ready = 0;

   if (get_mast)
      if (NOT srvldmas (1, &master, conf->cnum))  DONE (A_NOCONF);

   /*** Clear the internal conference access cache. */
   a_cache (V_CLR, conf->cnum, nullint, nullint, nullint, nullint, nullchix);

   success = A_OK;
   if (mask & P_OWNER)    chxcpy (conf->owner, master.organizerid);
   if (mask & P_FCHANGE)  conf->change  =  NOT master.nochange;
   if (mask & P_FADD)     conf->add     =  NOT master.noenter;
   if (mask & P_FCHGSUB)  conf->subject =      master.chgsubj;

   if ((mask & P_NAME)  ||  (mask & P_FVIS) || (mask & P_FLIB))
      if (NOT get_trueconf (conf->cnum, lname, nullchix, nullchix))
         success = A_DBERR + XCTC;

   if (mask & P_NAME)  chxcpy (conf->lname, lname);

   if (mask & P_GREET) {
      if (unit_lock (XCGR, READ, L(conf->cnum), L(0), L(0), L(0), nullchix)) {
         if (a_get_text (conf->greet, XCGR) >= A_DBERR)  
            success = A_DBERR + XCGR;
         unit_unlk  (XCGR);
      }
      else success = A_DBERR + XCGR;
   }

   if (mask & P_TEXT) {
      if (unit_lock (XCIN, READ, L(conf->cnum), L(0), L(0), L(0), nullchix)) {
         if (a_get_text (conf->intro, XCIN) >= A_DBERR)  success = A_DBERR + XCIN;
         unit_unlk  (XCIN);
      }
      else success = A_DBERR + XCIN;
   }

/*
   if (mask & P_USERLIST) {
      if (unit_lock (XCUS, READ, L(conf->cnum), L(0), L(0), L(0), nullchix)) {
         if (a_get_text (conf->userlist, XCUS) >= A_DBERR)  success = A_DBERR + XCUS;
         unit_unlk  (XCUS);
      }
      else success = A_DBERR + XCUS;
   }
*/

   if (mask & P_USERLIST) {
      chxclear (conf->userlist->data);
      if (NOT load_ulist (XCUS, conf->cnum, conf->userlist->data, 
                                conf->owner, &errors))
         success = A_DBERR + XCUS;
   }

   /*** To get the visibility or type of the conference, we have to look up the
   /    conf/library name in MISC/confs001. */
   if (mask & P_FVIS || mask & P_FLIB) {
      /*** Get the current entry for this conference. */
      names = nlnode (1);
      nladd  (names, lname, 0);
      matchnames (XSCD, XSCN, 0, nullchix, names, AND, &add, NOPUNCT);
      nlfree (names);

      /*** If there were two matches, make sure we're using the Local one. */
      size = nlsize (add);
      if (size < 1)                                      DONE (A_DBERR + XSCN);
      value = chxalloc (L(4), THIN, "agetconf value");
      chxtoken (value, nullchix, 2, add->next->str);

      if (size > 1 && chxvalue (value, L(6)) != 'L')
         chxtoken (value, nullchix, 2, add->next->next->str);
  
      if (size > 0) {
         /*** Decide if it's open or closed. */
         if (mask & P_FVIS) 
            conf->visible = (chxvalue (value,0L) == 'o' ? 1 : 0);

         /*** Decide whether it's a conf or a file library. */
         if (mask & P_FLIB) 
            conf->library = (chxvalue (value,8L) == 'L' ? 1 : 0);
      }
      else success = A_DBERR + XSCN;
      chxfree (value);

      nlfree (add);
   }

done:
   chxfree (lname);
   conf->ready = 1;
   return (success);
}
