/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** CHN: main for the program CHN_X, which just prints out
/    the chn for this system or the ch# for a neighbor.
/
/    This is used by some of the shipping and receiving methods.
/
/    Invocation:
/       chn_x confid [option]
/
/    Arguments:
/       confid       The usual: home of the Caucus account
/
/    Options:
/       -s           Print out this system's CHN
/       -n neighbor  Print out a neighbor's CH#
/ */

/*: JV 10/27/92 12:07 Create this. */
/*: JV 12/04/92 13:10 Clean up unused code. */

#include <stdio.h>
#include <chixuse.h>
#include "handicap.h"
#include "systype.h"
#include "xaction.h"
#include "unitcode.h"

extern Chix                    confid;

FUNCTION  main (argc, argv)
   int    argc;
   char  *argv[];
{
   Chix neighbor, choice, userid;
   int  optpos;

   choice   = chxalloc (L(4),  THIN, "chn_x choice");
   confid   = chxalloc (L(40), THIN, "chn_x confid");
   neighbor = chxalloc (L(40), THIN, "chn_x neighbor");
   userid   = chxalloc (L(40), THIN, "chn_x userid");


   /*** Get the confid. */
   if (argc <  2)   exit (1);
   chxcpy (confid, CQ(argv[1]));
   optpos = 2;

   /*** Get CHOICE. */
   chxcpy (choice, CQ(argv[optpos]));
   if (chxeq (choice, CQ("-n"))) 
      if (argc>3) exit(1);
      else        chxofascii (neighbor, argv[++optpos]);
   ++optpos;

   utilglobals();
   optchk ();

   /*** Miscellaneous initialization. */
   unitglobals();
   sysbrkinit();
   sysglobals();
   sysuserid (userid);
   sysdirchk (userid);

   /* Print my CHN. */
   if (chxeq (choice, CQ("-s")))  {
      get_chn (neighbor, XSNF, 0);
      printf ("%s", ascquick(neighbor));
   }
   if (chxeq (choice, CQ("-n"))) 
      printf ("%3d", hnumber (neighbor));

   return  (0); /* Make Lint happy. */
}
