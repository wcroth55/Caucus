/*** A_CHG_CONF.   Change basic information about a conference.
/
/    ok = a_chg_conf (conf, mask, wait);
/
/    Parameters:
/       int       ok;       (return status)
/       Conf      conf;     (object to fill with conference info)
/       int4      mask;     (mask of properties to get)
/       int       wait;     (synchronicity flag)
/
/    Purpose:
/       Replace basic information about a conference with the selected
/       properties from the CONF object.
/
/    How it works:
/       Caller must fill in conf->cnum, plus properties selected by MASK.
/
/    Returns: A_OK on success
/             A_NOCONF if no such conference
/             A_NOTOWNER if no organizer ability for this user.
/             A_DBERR on database error
/
/    Error Conditions:
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
/    Home:  api/achgconf.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 11/27/92 17:38 New function. */
/*: CR 12/06/92 22:58 Clear conference access cache. */
/*: CR 12/07/92 10:43 Use 'nullint'. */
/*: CR 12/13/92 22:04 Add comments. */
/*: CR  2/22/93 23:31 Add new args to a_cache() calls. */
/*: CP  8/01/93  9:37 Add debugging stuff. */
/*: CR  4/12/04 Use co_priv_str() to check access levels. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"
#include "xaction.h"

#define  nullint  ( (int *) NULL)

extern struct master_template master;
extern union  null_t   null;
extern int             debug;

FUNCTION  a_chg_conf (Conf conf, int4 mask, int wait)
{
   Namelist groups, nlnode();
   int      error, success, access, status, value, attr;
   Conf     ctemp;
   Chix     userid, destchn, temp, cname;

   if (disk_failure (0, &error, NULL))                return (A_DBERR);

   if (conf == (Conf) NULL  ||  conf->tag != T_CONF)  return (A_BADARG);
   if (conf->cnum <= 0)                               return (A_NOCONF);

   userid  = chxalloc (L(20), THIN, "a_chg_co userid");
   destchn = chxalloc ( L(4), THIN, "a_chg_co destchn");
   temp    = chxalloc (L(20), THIN, "a_chg_co temp");
   cname   = chxalloc (L(20), THIN, "a_chg_co cname");
   sysuserid (userid);
   success = A_OK;

   /*** Make sure this user has organizer status. */
   if (NOT (mask & P_OVERRIDE)) {
      groups  = nlnode (1);
      allowed_on (nullchix, null.nl, -1);  /* Flush cache. */
      access = allowed_on (userid, groups, conf->cnum);
      unit_wipe (LCNF);
      nlfree (groups);
      if (access < co_priv_str("organizer"))  DONE (A_NOTOWNER);
   }

   /*** Clear the internal conference access cache. */
   a_cache (V_CLR, conf->cnum, nullint, nullint, nullint, nullint, nullchix);

   /*** Only the primary organizer can change the primary organizer... */
   if (mask & P_OWNER) {
      if (NOT (mask & P_OVERRIDE)) {
         ctemp   = a_mak_conf("achgconf conf");
         ctemp->cnum = conf->cnum;
         status = a_get_conf (ctemp, P_OWNER, A_WAIT);
         chxcpy (temp, ctemp->owner);
         a_fre_conf (ctemp);
         if (status != A_OK)  DONE (A_NOCONF);
         if (NOT equal_id (userid, temp, null.chx))  DONE (A_NOTOWNER);
      }

      if (NOT cus_dporg (conf->cnum, conf->owner, &master, &error))
         DONE (A_DBERR);

   }

   /*** Change the conference introduction text. */
   if (mask & P_TEXT) {
      if (NOT cus_dtext (conf->cnum, CUS_INTRO, conf->intro->data, &error))
         DONE (A_NOCONF);
   }

   /*** Change the conference greeting text. */
   if (mask & P_GREET) {
      if (NOT cus_dtext (conf->cnum, CUS_GREET, conf->greet->data, &error))
         DONE (A_NOCONF);
   }

   /*** Change the conference userlist. */
   if (mask & P_USERLIST) {
      if (NOT cus_dusers (conf->cnum, conf->userlist->data, 
                          conf->owner, &error))  DONE (A_NOCONF);
   }

   if ((mask & P_FCHANGE)  ||  (mask & P_FADD)  ||  (mask & P_FVIS)  ||
       (mask & P_FCHGSUB)) {
      if (mask & P_FCHANGE)  { attr = CUS_CHANGE;      value = conf->change; }
      if (mask & P_FCHGSUB)  { attr = CUS_SUBJECT;     value = conf->subject; }
      if (mask & P_FADD)     { attr = CUS_ADD;         value = conf->add; }

      if (mask & P_FVIS) {
         attr  = CUS_VISIBILITY;
         value = conf->visible;
         if (NOT get_trueconf (conf->cnum, cname, nullchix, nullchix))
            DONE (A_DBERR);
      }

      if (NOT cus_dattr (conf->cnum, cname, attr, value, &error))
         DONE (A_NOCONF);

   }

done:
   chxfree (userid);
   chxfree (destchn);
   chxfree (temp);
   chxfree (cname);
   return  (success);
}
