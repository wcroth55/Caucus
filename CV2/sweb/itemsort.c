
/*** ITEM_SORT.  Sort a triplet list of items by title, author, or 
/                date of last response.
/
/    item_sort (result, arg);
/   
/    Parameters:
/       Chix                result;  (append resulting value here)
/       Chix                arg;     (function argument)
/
/    Purpose:
/       Evaluate $item_sort (direction code triplet_list)
/
/    How it works:
/       direction > 0 means ascending sort, < 0 is descending.
/       code 1 means title, 2 means author, 3 means date of
/              last response.
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/itemsort.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/10/97 14:16 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

extern int    cml_debug;
extern Vartab g_icache;

extern Cstat  g_cstat;

FUNCTION  item_sort (Chix result, Chix arg)
{
   static Resp   resp  = (Resp)  NULL;
   int   *inums, *slots;
   int4  *epoch;
   int4   t1, t2;
   int    direction, code, cnum, items, ip, nx, swap, rnum, status;
   int4   pos, pos_t, pos0;
   char   temp[100];
   char  *sysmem();
   int4   epoch_of();
   void   bubswap1(), bubswap3();
   ENTRY ("item_sort", "");

   /*** Initialization of static Resp object. */
   if (resp  == (Resp) NULL) {
      resp   = a_mak_resp ("itemsort resp");
   }

   pos = 0;
   direction = chxint4 (arg, &pos);
   code      = chxint4 (arg, &pos);
   pos_t = pos;

   /*** Count number of ITEMS. */
   for (items=0;   (1);   ++items) {
      chxint4 (arg, &pos);
      chxint4 (arg, &pos);
      pos0 = pos;
      chxint4 (arg, &pos);
      if (pos == pos0)  break;
   }
   if (items == 0)  RETURN(1);


   /*** Get conference number. */
   pos  = pos_t;
   cnum = chxint4 (arg, &pos);
   touch_it (cnum, 0, 0);

   /*** Allocate arrays for sorting items. */
   if (code == 3) epoch = (int4 *) sysmem(L(sizeof(int4)) * (items+2), "epoch");
   else           slots = (int  *) sysmem(L(sizeof(int))  * (items+2), "slots");
   inums = (int *) sysmem (L(sizeof(int)) * (items+2), "inums");
   if (inums == NULL)  RETURN(1);

   /*** Fill INUMS with item number, and either SLOTS with
   /    title & author, or EPOCH with date of last response. */
   pos = pos_t;
   for (ip=0;   ip < items;   ++ip) {
      chxint4 (arg, &pos);
      inums[ip] = chxint4 (arg, &pos);
      chxint4 (arg, &pos);

      /*** Codes 1, 2 get title and author. */
      if (code < 3) {
         slots[ip] = item_cache (cnum, inums[ip], g_icache, resp);
         continue;
      }

      /*** Code 3 finds last undeleted response, and parses date. */
      if (inums[ip] > g_cstat->items)  epoch[ip] = 0;
      else {
         epoch[ip]  = 0;
         for (rnum = g_cstat->resps[inums[ip]];   rnum >= 0;   --rnum) {
            resp->cnum = cnum;
            resp->inum = inums[ip];
            resp->rnum = rnum;

            status = a_get_resp (resp, 0, A_WAIT);
            if (status                      != A_OK)  continue;
            if (chxvalue (resp->date, L(0)) ==    0)  continue;
            epoch[ip] = epoch_of (resp->date);
            break;
         }
      }
   }

   /*** Bubble sort by title, author, or date. */
   for (ip=0;   ip < items-1;   ++ip) {
      for (nx=1;   nx < items;   ++nx) {
         switch (code) {
            case 1: bubswap1 (direction, inums, slots, g_icache->name,  0, nx);
                      break;
            case 2: bubswap1 (direction, inums, slots, g_icache->value, 2, nx);
                      break;
            case 3: bubswap3 (direction, inums, epoch,                     nx);
                      break;
         }
      }
   }


   /*** Output sorted triplet list. */
   for (ip=0;   ip < items;   ++ip) {
      sprintf (temp, "%d %d 0 ", cnum, inums[ip]);
      chxcat  (result, CQ(temp));
   }

   sysfree ( (char *) inums);
   if (code == 3) sysfree ((char *) epoch);
   else           sysfree ((char *) slots);

   RETURN (1);
}



FUNCTION  void bubswap1 (int direction, int *inums, int *slots, 
                         Chix *what, int token, int nx)
{
   static Chix first = nullchix;
   static Chix second;
   int         swap;

   if (first == nullchix) {
      first  = chxalloc (L(80), THIN, "bubswap1 first");
      second = chxalloc (L(80), THIN, "bubswap1 second");
   }

   if (token == 0) {
      chxcpy (first,  what[slots[nx-1]]);
      chxcpy (second, what[slots[nx  ]]);
   }
   else {
      chxtoken (nullchix, first,  token, what[slots[nx-1]]);
      chxtoken (nullchix, second, token, what[slots[nx  ]]);
   }

   jixreduce (first);
   jixreduce (second);

   if (direction * chxcompare (first, second, ALLCHARS) > 0) {
      swap = inums[nx-1];   inums[nx-1] = inums[nx];  inums[nx] = swap;
      swap = slots[nx-1];   slots[nx-1] = slots[nx];  slots[nx] = swap;
   }

   return;
}


FUNCTION  void bubswap3 (int direction, int *inums, int4 *epoch, int nx)
{
   int         iswap;
   int4        lswap;

   if ( (direction > 0  &&  epoch[nx-1] > epoch[nx])  ||
        (direction < 0  &&  epoch[nx-1] < epoch[nx]) ) {
      iswap = inums[nx-1];   inums[nx-1] = inums[nx];  inums[nx] = iswap;
      lswap = epoch[nx-1];   epoch[nx-1] = epoch[nx];  epoch[nx] = lswap;
   }

   return;
}
