/*** A_CACHE.   Caucus API low-level cache of conference access information.
/
/    ok = a_cache (verb, cnum, access, additem, chgresp, chgsubj, cname);
/
/    Parameters:
/       int       ok;       (success?)
/       int       verb;     (V_GET or V_CLR)
/       int       cnum;     (conference number)
/       int      *access;   (returned access level)
/       int      *additem;  (can users add items?)
/       int      *chgresp;  (can users change responses?)
/       int      *chgsubj;  (can users change osubjects?)
/       Chix      cname;    (conference name);
/
/    Purpose:
/       Many api functions (especially a_get_resp(), a_add_resp(), etc.)
/       need to know what the user is allowed to do in a particular
/       conference.   "Allowed to do" means:
/          access     (0=excluded,    1=readonly, 2=include, 3=organizer)
/          additem    (0=no adding,   1=users can add items)
/          chgresp    (0=no changing, 1=users can change own responses)
/          chgsubj    (0=no changing, 1=users can change osubjects)
/
/       Fetching this information from the database each time
/       it is needed is expensive.  So a_cache() provides a means
/       to cache and access this information inexpensively.
/
/    How it works:
/       If verb is V_GET, a_cache() looks in its internal cache for
/       the information about CNUM.  If it is not found, a_cache()
/       gets the information from disk and stores it in the cache.
/
/       If verb is V_CLR, a_cache() clears out the entry for this
/       conference from its internal cache.  This should be used
/       when an API function is modifying any of the information
/       about this conference.  (Thus forcing the next V_GET call
/       to a_cache() to look it all up anew.)
/
/       If CNAME is not nullchix, copies the name of this conference
/       into CNAME.
/
/    Returns: 1 on success
/             0 on database error
/
/    Error Conditions:
/  
/    Side effects: 
/
/    Related functions:
/
/    Called by:  api functions only
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  api/acache.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 12/03/92 15:38 New function. */
/*: CR 12/03/92 23:45 Fail if conference does not exist. */
/*: CR 12/04/92 15:42 Fix allowed_on call. */
/*: CR  2/12/93 09:27 Add CHGSUBJ, CNAME arguments. */
/*: CP  8/01/93  9:37 Add debugging stuff. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"

#define   MAXCACHE  10

FUNCTION  a_cache (verb, cnum, access, additem, chgresp, chgsubj, cname)
   int    verb, cnum;
   int   *access, *additem, *chgresp, *chgsubj;
   Chix   cname;
{
   struct cache_t {
      int   cnum;
      int   access;
      int   additem;
      int   chgresp;
      int   chgsubj;
      int4  lastuse;
      Chix  cname;
   };
   static struct cache_t cache[MAXCACHE];
   static short  first_call = 1;
   int      i, slot;
   Chix     userid;
   Namelist groups, nlnode();
   Conf     conf;
   int4     systime();

   /*** Check argument validity. */
   if (cnum <= 0)                     return (0);
   if (verb!=V_GET  &&  verb!=V_CLR)  return (0);

   /*** Initialize cache on first call. */
   if (first_call) {
      for (i=0;   i<MAXCACHE;   ++i)  {
         cache[i].cnum = 0;
         cache[i].cname = chxalloc (L(30), THIN, "a_cache cname");
      }
      first_call = 0;
   }

   /*** Find SLOT containing CNUM. */
   for (slot=(-1), i=0;   i<MAXCACHE;   ++i) {
      if (cache[i].cnum == cnum) { slot=i;     break; }
   }

   /*** Handle V_CLR clearing a cache slot. */
   if (verb == V_CLR) {
      if (slot >= 0) cache[slot].cnum = 0;
      return (1);
   }


   /*** If CNUM is not in the cache, fetch it. */
   if (slot < 0) {

      /*** Get an empty slot.  If none, use the oldest. */
      for (slot=i=0;   i<MAXCACHE;   ++i) {
         if (cache[i].cnum    == 0)                  { slot = i;   break; }
         if (cache[i].lastuse <  cache[slot].lastuse)  slot = i;
      }
   
      /*** From here, fetch info from disk and put in cache. */
      userid = chxalloc (L(20), THIN, "a_chg_co userid");
      sysuserid (userid);
      groups  = nlnode (1);
      allowed_on (nullchix, (Namelist) NULL, -1);  /* Flush cache. */
      cache[slot].access = allowed_on (userid, groups, cnum);
      unit_wipe (LCNF);
      nlfree  (groups);
      chxfree (userid);
   
      /* No conf! */
      if (cache[slot].access < 0)                               return (0); 
      if ( (conf = a_mak_conf("acache conf")) == (Conf) NULL)   return (0);
   
      conf->cnum = cnum;
      a_get_conf (conf, P_FCHANGE | P_FADD | P_NAME, A_WAIT);
      cache[slot].cnum    = cnum;
      cache[slot].additem = conf->add;
      cache[slot].chgresp = conf->change;
      cache[slot].chgsubj = conf->subject;
      chxcpy (cache[slot].cname, conf->lname);
      a_fre_conf (conf);
   }

   /*** Now we know the information is cache'd either way.  Hand
   /    it to the caller and return. */
   *access  = cache[slot].access;
   *additem = cache[slot].additem;
   *chgresp = cache[slot].chgresp;
   *chgsubj = cache[slot].chgsubj;
   if (cname != nullchix)  chxcpy (cname, cache[slot].cname);
   cache[slot].lastuse = systime();

   return (1);
}
