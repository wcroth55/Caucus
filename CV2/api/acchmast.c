
/*** A_CCH_MASTER.  Handle API caching of 'Master' objects.
/
/    master = a_cch_master (verb, cnum);
/
/    Parameters:
/       Master  master;    (resulting Master, from internal cache)
/       int     verb;      (action to take: V_GET, V_CLR, V_SIZE, V_FREE)
/
/    Purpose:
/       Provide caching of Master's for top-level API functions.
/
/    How it works:
/       V_SIZE is used once, during API initialization: it sets the size
/       of the cache (number of Master's in the cache) to the value of CNUM.
/
/       V_FREE is used once, during API closure; it frees all of the
/       entries in the cache.
/
/       V_GET returns the master for conference CNUM.  If no master
/       in the cache contained that partfile, the oldest master gets
/       loaded with the info for conference CNUM.
/
/       V_CLR clears a master.  This is used when the caller "knows"
/       that the cache'd master may be out of date and should not be used.
/
/    Returns: the appropriate master (V_GET)
/             NULL (all other verbs)
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by: API top-level functions (a_chg_conf, etc.)
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  api/acchmast.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  2/28/93 23:08 New function. */
/*: CK  3/01/93 11:48 Cast sysmem appropriately. */
/*: CK  3/01/93 16:48 sysmem requires int4 arg. */

#include <stdio.h>
#include "chixuse.h"
#include "api.h"
#include "master.h"
#define  NULLMASTER   ( (Master) NULL)

FUNCTION   Master a_cch_master (int verb, int cnum)
{
   static  Master *cache = NULL;
   static  int     size;
   static  int4    used  = 0;
   int     i, old;
   Master  master;

   if (verb==V_SIZE  &&  cache!=NULL)   return (NULLMASTER);

   if (verb==V_SIZE) {
      size  = cnum;
      cache = (Master *) sysmem (SIZE(Master) * size, "*Master");
      for (i=0;   i<size;   ++i) {
         cache[i] = (Master) sysmem (SIZE (struct master_template), "Master");
         make_master (cache[i], MAXITEMS);
      }
   }

   if (cache==NULL)  return (NULLMASTER);

   if (verb==V_FREE) {
      for (i=0;   i<size;   ++i)  {
         free_master (cache[i]);
         sysfree ( (char *) cache[i]);
      }
      sysfree ( (char *) cache);
      cache = NULL;
   }

   if (verb==V_GET) {

      /*** Find the master that contains this cnum; else,
      /    find the oldest master. */
      for (old=i=0;   i<size;   ++i) {
         master = cache[i];
         if (master->lastused < cache[old]->lastused)  old = i;
         if (cnum != master->cnum)   continue;

         /*** This appears to be the desired master. */
         master->lastused = used++;
         return (master);
      }

      /*** Use the oldest master, and load it with conf CNUM's info. */
      master = cache[old];
      master->lastused = used++;
      srvldmas (1, master, cnum);
      return  (master);
   }

   if (verb==V_CLR) {
      for (i=0;   i<size;   ++i) {
         master = cache[i];
         if (cnum != master->cnum)   continue;

         master->cnum     = 0;
         master->lastused = 0;
         break;
      }
   }

   return (NULLMASTER);
}
