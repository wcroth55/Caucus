
/*** CO_PRIV.    COnference PRIVilege.   Maps priv name to priv level number.
/
/    priv = co_priv (privname);
/
/    Parameters:
/       int      priv;      (returned priv level)
/       Chix     privname;  (privilege name or init string)
/
/    Purpose:
/       Map a conference privilege name to a priv level.
/       E.g. "organizer" maps to 40, "exclude" to 0, and so on.
/
/    How it works:
/       If "privname" begins with a leading colon (":"), it is removed.
/
/       The mapping is:
/          "organizer 40  instructor 30  include 20  readonly 10  exclude 0"
/
/
/    Returns: priv level number, or 0 if no match.
/
/    Home:  conf/allowedo.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 04/12/04 New function. */
/*: CR 05/02/04 Make default same as old Caucus. (3 2 1 0) */
/*: CR 09/02/05 Changed for static C5 values. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"

#define MAPSIZE  40
#define MAPWIDE  20
static char mapnames[MAPSIZE][MAPWIDE];
static int  maplevel[MAPSIZE];
static int  maps = 0;


FUNCTION  int co_priv_str (char *privstr) {
   static Chix   privname = nullchix;

   if (privname == nullchix)  
      privname = chxalloc (L(100), THIN, "co_priv privname");

   chxofascii (privname, privstr);
   return (co_priv (privname));
}

FUNCTION  int co_priv (Chix privname) {
   static Chix word  = nullchix;
   char   priv[MAPWIDE];
   int    i;

   if (word == nullchix)  word = chxalloc (L(100), THIN, "co_priv word");

   /*** If a regular call, make sure we have at least a default map. */
   if (maps == 0) {
      chxofascii   (word, 
         "organizer 40 instructor 30 include 20 readonly 10 exclude 0");
      co_priv_load (word);
   }

   /*** Clean up the input privname, match it against our stored map,
   /    return the matching priv level number. */
   chxtoken  (word, nullchix, 1, privname);
   jixreduce (word);
   ascofchx  (priv, word, 0, MAPWIDE);
   if (priv[0] == ':')  ascofchx (priv, word, 1, MAPWIDE);
   for (i=0;   i<maps;   ++i)  {
      if (streq (priv, mapnames[i]))  return (maplevel[i]);
   }
   return (0);
}

FUNCTION  int co_priv_load (Chix mapstr) {
   int4   pos, value;
   int    i;
   Chix   w;

   w = chxalloc (L(100), THIN, "co_priv w");

   for (maps=0, pos=0;   chxnextword (w, mapstr, &pos) >= 0;   ) {
      jixreduce (w);
      ascofchx  (mapnames[maps], w, 0, MAPWIDE);
      chxnextword (w, mapstr, &pos);
      chxnum (w, &value);
      maplevel[maps] = value;
      ++maps;
   }

   chxfree (w);
   return  (1);
}
