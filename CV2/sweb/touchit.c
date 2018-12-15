
/*** TOUCH_IT.  Ensure IT globals are up to date.
/
/    ok = touch_it (cnum, reload, timeout);
/   
/    Parameters:
/       int4        cnum;    (conf number)
/       int         reload;  (force reload if true)
/       int         timeout; (force reload if timeout>0 and age is > timeout)
/
/    Purpose:
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  touchit.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  4/09/95 15:18 New function. */
/*: CR  3/20/00 13:30 Sanity check cnum arg. */
/*: CR  8/20/04 Add support for g_hidden.    */

#include <stdio.h>
#include <time.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

extern Cstat g_cstat;
extern Pstat g_pstat, g_hidden;

FUNCTION  int touch_it (int4 cnum, int reload, int timeout)
{
   char   temp[100];
   int    rc;
   ENTRY ("touch_it", "");

   if (cnum <= 0)  RETURN (0);

   /*** Reload g_cstat cache if conference number has changed or RELOAD. */
   if (g_cstat->cnum != cnum  ||  reload  ||  
       (timeout  &&  (time(NULL) - g_cstat->time > timeout))) {
      g_cstat->cnum = cnum;
      rc = a_get_cstat (g_cstat, P_EVERY, A_WAIT);
      if (rc != A_OK) {
         g_cstat->cnum = 0;    g_cstat->items = 0;
         g_pstat->cnum = 0;    g_pstat->items = 0;
         RETURN (0);
      }
   }

   /*** Reload g_pstat cache if conference number has changed. */
   /*   (Not sure about RELOAD yet... */
   if (g_pstat->cnum != cnum) {
      sysuserid (g_pstat->owner);
      g_pstat->cnum  = cnum;
      g_pstat->items = 0;
      if (a_get_pstat (g_pstat, P_EVERY, A_WAIT) != A_OK)  g_pstat->items = -1;
   }

   /*** Reload g_hidden cache if conference number has changed. */
   /*   (Not sure about RELOAD yet... */
   if (g_hidden->cnum != cnum) {
      g_hidden->cnum  = cnum;
      g_hidden->items = 0;
      rc = a_get_pstat (g_hidden, P_EVERY, A_WAIT);
      if (rc != A_OK)
          a_add_pstat (g_hidden, P_EVERY | P_OVERRIDE, A_WAIT);
   }

   RETURN (1);
}
