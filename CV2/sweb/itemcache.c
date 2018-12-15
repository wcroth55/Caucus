
/*** ITEM_CACHE.   Handle caching of item titles, owners, and authors.
/
/    slot = item_cache (cnum, inum, cache, resp);
/   
/    Parameters:
/       int     slot;   (returned slot in CACHE)
/       int4    cnum;   (conference number)
/       int4    inum;   (item number)
/       Vartab  cache;  (cache in use)
/       Resp    resp;   (response object)
/
/    Purpose:
/       Handle caching of item title and owner information.
/
/    How it works:
/       Returns slot number in Vartab CACHE that contains the
/       title (cache->name[slot]), owner (first word of cache->value[slot])
/       and full author name (remaining words of cache->value[slot])
/       of conference CNUM, item INUM.
/
/       Does all the work necessary to update the CACHE appropriately.
/       The caller presumably declared CACHE to be static.
/
/       Keeps the conference number with each slot, so a cache may actually
/       contain data from several different conferences.
/
/       Looks at and uses RESP to improve caching (in case caller
/       has already loaded this title in RESP).
/
/    Returns: slot number, on success.
/             -1 if no such item.
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/itemcache.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/14/96 13:08 New function, based on old title_of(). */
/*: CR  4/13/00 12:44 Use new resp->exists member. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "maxparms.h"
#include "api.h"
#include "null.h"

FUNCTION  item_cache (int4 cnum, int4 inum, Vartab cache, Resp resp)
{
   static Chix   empty = nullchix;
   int           slot;
   int           method;
   ENTRY ("item_cache", "");

   if (empty == nullchix)  empty = chxalloc (L(4), THIN, "item_cache empty");

   /*** Sanity check arguments. */
   if (cache == (Vartab) NULL)         RETURN (-1);
   if (cnum < 0  ||  cnum >     9999)  RETURN (-1);
   if (inum < 0  ||  inum > MAXITEMS)  RETURN (-1);

   /*** Make sure cache is big enough for this item #. */
   while (inum > cache->max - 3) {
      if (NOT vartab_exp (cache))      RETURN (-1);
   }

   /*** Make sure this slot in the cache exists. */
   if (cache->name[inum] == nullchix) {
      cache->name [inum]  = chxalloc (L(60), THIN, "item_cache name[slot]");
      cache->value[inum]  = chxalloc (L(20), THIN, "item_cache value[slot]");
      if (inum > cache->used)  cache->used = inum+1;
   }

   /*** Which method did we use to get this data? (For debugging.) */
   method = 0;

   /*** If RESP has desired data, put it into the cache. */
   if (cnum==resp->cnum  &&  inum==resp->inum  &&  0==resp->rnum  && 
       resp->exists) {
      chxcpy    (cache->name [inum], resp->title);
      chxcpy    (cache->value[inum], resp->owner);
      chxcatval (cache->value[inum], THIN, L(' '));
      chxcat    (cache->value[inum], resp->author);
      cache->code[inum] = (short) cnum;
      method += 1;
   }

   /*** Do we have the desired data in the cache? */
   if (chxvalue (cache->name[inum], L(0))  &&  cache->code[inum] == cnum) {
      method += 2;
      goto done;
   }


   /*** Nope.  Load RESP via the API, and then use that. */
   resp->cnum = cnum;
   resp->inum = inum;
   resp->rnum = 0;
   if (a_get_resp (resp, P_EVERY, A_WAIT) != A_OK)  RETURN (-1);
   removeid (resp->author, resp->author, resp->owner, empty);

   /*** Put it in the cache. */
   chxcpy    (cache->name [inum], resp->title);
   chxcpy    (cache->value[inum], resp->owner);
   chxcatval (cache->value[inum], THIN, L(' '));
   chxcat    (cache->value[inum], resp->author);
   cache->code[inum] = (short) cnum;
   method += 4;

done:
   RETURN (inum);
}
