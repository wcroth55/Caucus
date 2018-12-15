
/*** UNIT_APPEND.  Open the file of TYPE for write/append. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:54 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include "caucus.h"
#include <stdio.h>
#include "systype.h"
#include "units.h"

extern struct unit_template  units[];
extern char                 *unittag[];
extern int                   debug;

FUNCTION  unit_append (n)
   int  n;
{
   char  *bugtell();

   ENTRY ("unitappend", "");
   if (debug & DEB_FILE)  bugtell ("APPEND", n, L(0), (n>=0 ? unittag[n] : ""));

   /*** Bug check: is this file N already open? */
   if (units[n].access != LOCKED) {
      buglist (bugtell ("Unitappe: ", n, L(0), "file is not locked!\n"));
      RETURN  (0);
   }

   /*** We can always append to the bit bucket. */
   if (n == XIBB)  RETURN (1);

#if UNIX | NUT40 | WNT40

   /*** Ensure Caucus privilege is not used for uncontrolled file access. */
   if (n < XURG)     sysprv(0);
   if (NOT sysaccess (units[n].nameis))  {
      if (n < XURG)  sysprv(1);
      RETURN (0);
   }

   if (syscheck (units[n].nameis)) {

#if UNIX | NUT40
      units[n].fp = fopen (units[n].nameis, "a");
#endif
#if WNT40
      units[n].fp = fopen (units[n].nameis, "at");
#endif
      units[n].fd = -1;
      if (n < XURG)  sysprv(1);
      if (units[n].fp == NULL)   RETURN (0);
      units[n].access = WRITE;
      RETURN (1);
   }

   if (n < XURG)  sysprv(1);
   RETURN (unit_make (n));
#endif
}
