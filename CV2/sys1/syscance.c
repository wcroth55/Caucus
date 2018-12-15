
/*** SYSCANCEL.   Tell user what key is <CANCEL>. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

#if SYSV | HPXA9 | LNX12
#include <termio.h>
#endif

#if BSD21 | SUN41 | FBS22
#include <sgtty.h>
#endif

extern union null_t null;

FUNCTION  Chix syscancel()
{
   static Chix cancel;


/*---------------------------------------BSD-------------------------*/
#if BSD21 | SUN41 | FBS22
   struct tchars  a;
   Chix   control;
   Chix   chx_t_intrc;

   ENTRY ("syscancel", "");

   if (NOT chxtype(cancel))
      cancel = chxalloc (L(30), THIN, "syscance cancel");  /* STATIC */

   control     = chxalloc (L(30), THIN, "syscance control");
   chx_t_intrc = chxalloc (L(5),  THIN, "syscance chx_t_intrc");

   if (EMPTYCHX (cancel)) {
      if (ioctl (0, TIOCGETC, &a) < 0)  a.t_intrc = 0;

      if      (a.t_intrc == 0177)   mdstr (cancel, "sys_Kdel", null.md);

      else if (a.t_intrc <   040) {
         mdstr   (control, "sys_Kctrl", null.md);
         chxcatval (chx_t_intrc, THIN, L(0100 + a.t_intrc));
         chxformat (cancel, CQ("%s%s"), L(0), L(0), control, chx_t_intrc);
      }

      else  {
         chxcatval (chx_t_intrc, THIN, L(0100 + a.t_intrc));
         chxformat (cancel, CQ("'%s'"), L(0), L(0), chx_t_intrc, nullchix);
      }
   }

   chxfree (control);
   chxfree (chx_t_intrc);
 
#endif

/*------------------------------MASSCOMP & A3----------------------------*/
#if SYSV | HPXA9 | LNX12
   struct termio a;
   Chix   control, chx_c_cc;

   ENTRY ("syscancel", "");

   if (NOT chxtype (cancel))
      cancel = chxalloc (L(30), THIN, "syscance cancel");  /* STATIC */
 
   control     = chxalloc (L(30), THIN, "syscance control");
   chx_c_cc    = chxalloc (L(5),  THIN, "syscance chx_t_intrc");
 
   if (EMPTYCHX (cancel)) {
      if (ioctl (0, TCGETA, &a) < 0)  a.c_cc[0] = 0;

      if      (a.c_cc[0] == 0177)  mdstr (cancel, "sys_Kdel", null.md);

      else if (a.c_cc[0] <   040) {
         mdstr   (control, "sys_Kctrl", null.md);
         chxcatval (chx_c_cc, THIN, L(0100 + a.c_cc[0]));
         chxformat (cancel, CQ("%s%s"), L(0), L(0), control, chx_c_cc);
      }

      else {
         chxcatval (chx_c_cc, THIN, L(0100 + a.c_cc[0]));
         chxformat (cancel, CQ("'%s'"), L(0), L(0), chx_c_cc, nullchix);
       }
   }

   chxfree (control);
   chxfree (chx_c_cc);
#endif

/*----------------------------------NT-----------------------------------*/
#if NUT40 | WNT40
   ENTRY ("syscancel", "");

   if (NOT chxtype(cancel))
      cancel = chxalloc (L(30), THIN, "syscance cancel");  /* STATIC */
 
   if (EMPTYCHX (cancel))  mdstr (cancel, "sys_Tcanctlc", null.md);
#endif

   RETURN (cancel);
}
