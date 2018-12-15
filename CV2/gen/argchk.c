/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** ARGCHK.   Parse and set command line options.
/
/    This is the new (Caucus 2.2+) version of ARGCHK.  Most Caucus options
/    are now specified in the OPTIONS file.  Backwards compability is
/    allowed for most 2.1 options as well.  See OPTCHK for details about
/    how to use those options.
/
/    ARGCHK scans the command line args in argv[], starting
/    with argv[argstart].
/
/    These flags cannot be set in the options file.
/
/    Note that since ARGCHK is called before sysexit(-1) initialization,
/    it *must* use hard-coded strings (instead of getting strings from
/    the dictionary), and *must* use printf() instead of unit_write().
/
/    The complete list of options is shown below, aint4 with the
/    flag name for each.
/
/Flag Name       Value  Default  Meaning
/---  ----------  -----  -------  -----------------------------------------
/ -a  ENTRYFILE   file            Text displayed when Caucus starts up
/ -b  BAUDRATE    #               Caucus/IM remote baud rate (999=autobaud)
/ -c  COMMAND              OFF    Allow "!" command escaping
/ -d  DEBUG       #        (0)    Internal debugging output
/ -e  REMOTETO    #        120    Remote timeout value for client/server
/ -f  LOCALTO     #        10     Local  timeout value for client/server
/ -g  VCMETHOD    #        0      Communication method for client/server
/ -i  FILEINPUT   file            User input read from file, not terminal
/ -l  LOGUSER     file            Log user entry/exit in file
/ -n  NEWUSER              (ON)   Allow captive users to register as NEWUSER
/ -o  PORT        #        (1)    Caucus/IM port control: COM1 or COM2
/ -p  PRINT       #        (0)    Use PRINT modifier? (0=no, 1=sys, 2=user)
/ -r  REDIRECTION          (OFF)  Allow input/output redirection
/ -s  CAPTIVE              (OFF)  Use Caucus captive login shell
/ -t  PCTYPE      I/Z      (I)    Caucus/IM, type of PC (Ibm, Zenith)
/ -u  LOGCONF              (OFF)  Log user entry/exit by conference
/ -v  auto login  n:p             Automatic login of name:password
/ -w  EXPIRATION           (OFF)  Warn users about expiration date
/ -x  EXITFILE    file            Text displayed when Caucus exits
/ -y  OPTFILE     file     none   Name of options file
/ -z  FULLSCREEN           (ON)   Use full screen interface for VM/CMS.
/ -8  EIGHTBITS            (OFF)  Eight bit (European) operation.
/     */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: CR  8/22/88 13:43 Allow "-d" or "-d n" (n=0,1,2...) */
/*: CR 01/16/89 22:04 Init comlintab, check mdtable() return. */
/*: CR  1/17/89 11:09 Add -w expiration warning. */
/*: CR  2/14/89 16:57 Add code for -u log-by-conference option. */
/*: JV  2/14/89 14:58 Added -g comm line option (local timeout). */
/*: CR  2/23/89 17:14 case (-1) stuff for Waterloo C bugs. */
/*: CR  3/14/89 15:16 Fix loctieout -> loctimeout. */
/*: CR  4/04/89 17:40 Integrate with Jcaucus (including jargchk) */
/*: CR  4/05/89 13:51 Jcaucus integration fixes.*/
/*: CR  4/06/89 22:40 More Jcaucus integration fixes.*/
/*: JV  8/03/89 10:52 Changed header doc from '-o' to '-y'. */
/*: JV  8/04/89 11:36 Moved all the options stuff to OPTCHK. */
/*: CR 10/06/89 17:37 Rewrite as switch/case, remove extra -g case. */
/*: CR 10/10/89 23:17 Add in many old 2.1 command line options. */
/*: JV  2/20/90 08:10 Add flag.oneuser default. */
/*: CR  4/12/90 14:39 Add "-8" option back in. */
/*: CR  6/05/90  1:17 Add flag.fullscreen. */
/*: CJ 11/22/90 22:35 Use new form of systurn() call. */
/*: CR  2/28/91 22:20 Add -v automatic login. */
/*: DE  3/31/92 11:20 Chixified */
/*: JX  5/26/92 15:15 Fixify, remove flag.rawio initialization. */
/*: CX  6/05/92 13:10 -i option changes u_keybd. */
/*: JX  6/16/92 09:31 Switch value of first (key) letter, not key itself! */
/*: CR  9/04/92 14:18 Initialize flag.logfeature. */
/*: CI 10/05/92 16:46 Chix Integration. */
/*: JX 10/09/92 17:45 Chixify strcpy. */
/*: TP 10/19/92 15:41 Use input_control() to set session file. */
/*: CR 10/30/92 12:38 Init eightbit, echo, iocontrol, outfile; remove systurn.*/
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: JV  7/21/93 13:08 Allocate chixen in FLAG. */
/*: JD  7/26/93 12:27 Add flag.diskfull. */
/*: CT  5/28/93 16:56 COMPLAIN now uses a chix. */
/*: CR  7/31/95 16:00 Add HANGOUT option. */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"

extern struct flag_template flag;
extern struct termstate_t   term_in_caucus;
extern int                  debug;

#define  COMPLAIN  printf ("Bad value for command line option '-%c'.\n", \
         (int) chxvalue (key, L(0)))

FUNCTION  argchk (argc, argv, argstart)
   int    argc, argstart;
   char  *argv[];
{
   int4  value;
   int   i, is_number;
   Chix  key;
   Chix  nextword;
   int   success;

   ENTRY ("argchk", "");
  
   key          = chxalloc (L(40), THIN, "argchk key");
   nextword     = chxalloc (L(40), THIN, "argchk nextword");
   success      = 0;

   /*** Initialize default values. */
   flag.pctype       =   0 ;   /* 0=IBM PC;   1=Z-100 */
   flag.port         =   0;    /* IBM PC: 0=>COM1, 1=>COM2. */
   flag.loctimeout   =  10;
   flag.remtimeout   = 120;
   flag.vcmethod     =   0;    /* 0=IPC, 1=MEM */
   flag.optfile      =   nullchix;
   flag.newuser      =   1;
   flag.no1prompt    =   0;
   flag.logfile      =   nullchix;
/* flag.infile       =   nullchix; */
   flag.outfile      =   nullchix;
   flag.editlist     =   0;
   flag.loop         =   0;
   flag.expwarn      =   1;
   flag.logbyconf    =   0;
   flag.oneuser      =   0;      /* Allow multiple users per id. */
   flag.fullscreen   =   1;      /* Use full screen interface for VM/CMS? */
   flag.editcom      =   1;      /* Allow use of EDIT command at AND NOW? */
   flag.logfeature   =   0;
   flag.autologin    =   nullchix;/* Automatic login from -v on command line. */
   flag.echo         =   1;
   flag.eightbit     =   0;
   flag.iocontrol    =   1;      /* Allow ioctl of terminal state. */
   flag.diskfull     =   1;
   flag.hangout      =   1;
  
   /*** Check each argument... */
   for (i=argstart;   i<argc;   ++i) {

      /*** And if it begins with "-", pick up the KEY letter and the
      /    next word, if any. */
      if (*argv[i] != '-')  continue;
      chxclear  (key);
      chxcatval (key, THIN, L(*(argv[i]+1)));
      chxclear  (nextword);
      is_number =  0;
      if (i+1 < argc  &&  *argv[i+1] != '-') {
         chxcpy (nextword, CQ(argv[++i]));
         is_number = chxnum (nextword, &value);
      }

      /*** Decide what to do depending on the key letter. */
      switch ((char) chxvalue (key, L(0))) {

         case ('a'):  /* "Entry file" displayed on startup. */
            flag.entrfile = chxalloc (L(16), THIN, "flag entrfile");
            chxcpy (flag.entrfile, nextword);
            break;
 
         case ('b'):  /* Baud setting. */
            if (is_number)  flag.baud = (int) value;
            else            COMPLAIN;
            break;
 
         case ('c'):  /* Command escaping. */
            flag.escape = 1;
            break;

         case ('d'):  /* Debug flag. */
            debug = 1;
            if (is_number)  debug = (int) value;
            break;
 
         case ('e'):  /* Remote timeout. */
            if (is_number)  flag.remtimeout = (int) value;
            else            COMPLAIN;
            break;
 
         case ('f'):  /* Local timeout. */
            if (is_number)  flag.loctimeout = (int) value;
            else            COMPLAIN;
            break;
 
         case ('g'):  /* VC Method. */
            if (is_number)  flag.vcmethod = (int) value;
            else            COMPLAIN;
            break;
 
         case ('i'):  /* Take ALL input from input session file. */
            if (unit_lock (XISF, READ, L(0), L(0), L(0), L(0), nextword)) {
               if (unit_view (XISF))
                       input_control (IC_RED | IC_SESS | XISF, nullchix);
               else    unit_unlk (XISF);
            }
            break;
 
         case ('l'):  /* Logging file. */
            flag.logfile = chxalloc (L(16), THIN, "flag.logfile");
            chxcpy (flag.logfile, nextword);
            break;
 
         case ('n'):  /* Newuser allowed? */
            flag.newuser = 0;
            break;
 
         case ('o'):  /* IBM PC Comm port. */
            flag.port = 1;
            break;
 
         case ('p'):  /* Print setting. */
            if (is_number)  flag.print = (int) value;
            else            COMPLAIN;
            break;
 
         case ('r'):  /* I/O Redirection. */
            flag.redirect = 1;
            break;

         case ('s'):  /* "Captive Caucus" login shell. */
            flag.login    = 1;
            break;

         case ('t'):  /* IBM PC type. */
            if ( chxvalue ( nextword, L(0 )) == 'z'  ||
                 chxvalue ( nextword, L(0 )) == 'Z' )  flag.pctype = 1;
            break;
 
         case ('u'):  /* Log by conference. */
            flag.logbyconf = 1;
            break;

         case ('v'):  /* Automatic login. */
            flag.autologin = chxalloc (L(16), THIN, "flag.autologin");
            chxcpy (flag.autologin, nextword);
            break;

         case ('w'):  /* Expiration warning. */
            flag.expwarn = 0;
            break;

         case ('x'):  /* "Exit file" displayed on leaving Caucus. */
            flag.exitfile = chxalloc (L(16), THIN, "flag exitfile");
            chxcpy (flag.exitfile, nextword);
            break;
 
         case ('y'):  /* Name of master.opt file. */
            if (NOT EMPTYCHX(nextword)) {
               flag.optfile = chxalloc (L(16), THIN, "flag.optfile");
               chxcpy (flag.optfile, nextword);
            }
            else                        COMPLAIN;
            break;
 
         case ('z'):  flag.fullscreen = 0;        break;

         case ('8'):  flag.eightbit = 1;          break;

         default:
            printf ("Unknown command line option '%s'.\n", argv[i]);
            break;
      }
   }

   SUCCEED;

 done:

   chxfree (nextword);
   chxfree (key);

   RETURN (success);
}
