/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** NET_ARGCHK.   Parse and set command line options.
/
/    ok = net_argchk (argc, argv, argstart);
/
/    Parameters:
/       int   ok;
/       int  *argc;     (address of main's argc)
/       char *argv[];   (main's argv)
/       int   argstart; (number of arg to start checking from)
/
/    Purpose:
/      Parse certain command line options which are common to all CaucusLink
/      progams.
/
/    How it works:
/      NET_ARGCHK scans the command line args in argv[], starting
/      with argv[argstart], ending at the last argument. 
/
/      For each option that it understands, it does the appropriate thing
/      and removes all traces of the option from argc and argv[][].
/
/      NET_ARGCHK ignores arguments which it does not understand.
/
/      See NET_OPTCHK for details about how to use "options file" options.
/
/      Note that since NET_ARGCHK is called before sysexit(-1) initialization,
/      it *must* use hard-coded strings (instead of getting strings from
/      the dictionary), and *must* use printf() instead of unit_write().
/
/      The complete list of options is shown below, aint4 with the
/      flag name for each.
/
/ **NOTE: any arg which uses the next word must increment 'i' in its case
/         statement.  See -d or -y for examples.
/
/Flag Name       Value  Default  Meaning
/---  ----------  -----  -------  -----------------------------------------
/ -d  DEBUG       [#]         1   Debug level
/ -y  OPTFILE     file     none   Name of options file
/
/    Returns: 1
/   
/    Error Conditions:
/
/    Side effects: changes argc, argv, net_flag.optfile, debug
/
/    Related functions: net_optchk()
/
/    Called by:  xfp, xfu
/
/    Operating system dependencies:
/
/    Known bugs:   none
/
/    Home: xaction/netargch.c
/     */

/*: JV 10/22/91 16:45 Create net_argchk. */
/*: JV  1/03/92 15:19 Declare strcpy(). */
/*: DE  6/04/92 15:40 Chixified*/
/*: J8  8/02/93 10:36 Remove args we process from argv and argc. */

#include <stdio.h>
#include "handicap.h"
#include "xfp.h"

extern struct net_flag_t   net_flag;
extern int                 debug;

#define COMPLAIN  printf ("Bad value for command line option '-%c'.\n", key)
#define CLEARARG(x)   (*argc)--; for (j=x; j < *argc; j++) argv[j] = argv[j+1];

FUNCTION  net_argchk (argc, argv, argstart)
   int   *argc, argstart;
   char  *argv[];
{
   int4  value;
   int   i, j, is_number;
   Chix  nextword;
   char  key, *strcpy();

   ENTRY ("net_argchk", "");
   
   nextword = chxalloc (L(40), THIN, "netargch nextword");

   /*** Initialize default values. */
   debug = 0;
   chxclear (net_flag.optfile);
   
   /*** Check each argument... */
   for (i=argstart;   i<*argc;  ) {  /* Increment only on switch default. */

      /*** And if it begins with "-", pick up the KEY letter and the
      /    next word, if any. */
      if (*argv[i] != '-')  continue;
      key = *(argv[i]+1);
      chxclear(nextword);
      is_number   =   0;
      if (i+1 < *argc  &&  *argv[i+1] != '-') {
         chxofascii (nextword, argv[i+1]);    /* Let case incr if necessary */
         is_number = chxnum (nextword, &value);
      }

      /*** Decide what to do depending on the key letter. */
      switch (key) {

         case ('d'):  /* Debug flag. */
            debug = -1;
            CLEARARG (i); /* Next time, look at new string in this cell. */
            if (is_number) {
               debug = (int) value;
               CLEARARG (i+1);     /* Increment i */
            }
            break;
 
         case ('y'):  /* Name of master.opt file. */
            CLEARARG (i);        /* Look at new string in this cell. */
            if (NOT EMPTYCHX(nextword)) {
               chxcpy (net_flag.optfile, nextword);
               CLEARARG (i+1);
            }
            else                         COMPLAIN;
            break;
 
         default:
            ++i;
            break;
      }
   }

   RETURN (1);
}

