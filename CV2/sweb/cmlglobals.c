
/*** CML_GLOBALS.  Define and initialize global data caches for CML.
/
/    cml_globals();
/   
/    Parameters:
/
/    Purpose:
/       Initialize and define place for common global API caches.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  cmlglobals.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  4/08/95 22:23 New function. */
/*: CR  5/30/02 Remove call to func_my(), too early (before cookie auth) */
/*: CR  8/20/04 Add g_hidden */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "api.h"

Person g_person = (Person) NULL;     /* current cached Person. */
Cstat  g_cstat  = (Cstat)  NULL;     /* current conf status. */
Pstat  g_pstat  = (Pstat)  NULL;     /* current personal status. */
Pstat  g_pstat2 = (Pstat)  NULL;     /* secondary pers status. */
Pstat  g_hidden = (Pstat)  NULL;     /* "hidden items" record. */
Vartab g_icache = (Vartab) NULL;     /* item title & owner cache. */
int    g_wrap   =             0;     /* default word-wrap value. */
int    g_mytext =             0;     /* how to treat "my" text? */

FUNCTION  cml_globals(void)
{
   static short first_call = 1;
   Chix         word1, word2;
   ENTRY ("cml_globals", "");

   if (first_call) {
      first_call = 0;
      word1 = chxalloc (L(20), THIN, "cml_glob word1");
      word2 = chxalloc (L(20), THIN, "cml_glob word2");

      g_person = a_mak_person ("cml_glob g_person");
      g_cstat  = a_mak_cstat  ("cml_glob g_cstat");
      g_pstat  = a_mak_pstat  ("cml_glob g_pstat");
      g_pstat2 = a_mak_pstat  ("cml_glob g_pstat2");
      g_hidden = a_mak_pstat  ("cml_glob g_hidden");
      g_icache = a_mak_vartab ("cml_glob g_icache");

/*    func_my (word1, "my_text", word2); */

      chxfree (word1);
      chxfree (word2);
   }

   RETURN (1);
}
