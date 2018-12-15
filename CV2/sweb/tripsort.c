
/*** TRIP_SORT.   Sort a triplet item list.
/
/    trip_sort (result, arg);
/   
/    Parameters:
/       Chix       result;  (append resulting value here)
/       Chix       arg;     (function argument)
/
/    Purpose:
/       Evaluate $triplet_sort(): sort a triplet item list.
/
/    How it works:
/
/    Returns: 
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  tripsort.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  9/23/97 17:01 New function */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "namelist.h"
#include "null.h"

extern union null_t null;

FUNCTION   int trip_sort (Chix result, Chix arg)
{
   Chix    word;
   int     count, slot, direction;
   char    temp[60];
   int4    pos;
   typedef struct Tlist_t * Tlist;
   struct  Tlist_t { int cnum;   int inum;   int rnum;   Tlist next; };
   Tlist   list, item, node, post, gone;
   char   *sysmem();
   ENTRY ("trip_sort", "");

   word = chxalloc (L(40), THIN, "trip_sort word");

   pos = 0;
   direction = chxint4 (arg, &pos);
   for (count=0;   chxnextword (word, arg, &pos) >= 0;   ++count) ;
   count = count / 3;

   list = (Tlist) sysmem (sizeof(struct Tlist_t), "trip_sort");
   if (list == (Tlist) NULL)  RETURN (0);

   list->cnum = -1;
   list->next = (Tlist) NULL;

   pos = 0;
   chxint4 (arg, &pos);
   for (slot=0;   slot < count;   ++slot) {
      item = (Tlist) sysmem (sizeof(struct Tlist_t), "ts item");
      item->cnum = chxint4 (arg, &pos);
      item->inum = chxint4 (arg, &pos);
      item->rnum = chxint4 (arg, &pos);

      for (node=list;   node!=(Tlist) NULL;   node=node->next) {
         post = node->next;
         if (post != (Tlist) NULL) {
            if (direction > 0) {
               if      (item->cnum >  post->cnum)       continue;
               else if (item->cnum == post->cnum) {
                    if      (item->inum >  post->inum)  continue;
                    else if (item->inum == post->inum) {
                         if (item->rnum >  post->rnum)  continue;
                    }
               }
            }
            else {
               if      (item->cnum <  post->cnum)       continue;
               else if (item->cnum == post->cnum) {
                    if      (item->inum <  post->inum)  continue;
                    else if (item->inum == post->inum) {
                         if (item->rnum <  post->rnum)  continue;
                    }
               }
            }
         }

         node->next = item;
         item->next = post;
         break;
      }
   }

   for (node=list->next;   node!=(Tlist) NULL;   node=node->next) {
      sprintf (temp, "%d %d %d ", node->cnum, node->inum, node->rnum);
      chxcat  (result, CQ(temp));
   }

   for (node=list;   node!=(Tlist)NULL;  ) {
      gone = node;
      node = node->next;
      sysfree ( (char *) gone);
   }

   chxfree (word);
      
   RETURN  (1);
}
