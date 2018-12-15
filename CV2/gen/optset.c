/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** OPTSET.  Set Flag options.
/
/ */

/*: AA  7/29/88 17:41 Source code master starting point*/
/*: CR  4/04/89 17:33 Integrate with Jcaucus. */
/*: CR  4/05/89 13:52 More Jcaucus integration fixes. */
/*: CR  4/07/89 13:20 Still more Jcaucus integration fixes. */
/*: CR  8/04/89 13:39 Do not set flag.nolower if JAPAN ON. */
/*: JV  2/20/90 07:45 Add ONEUSER option. */
/*: JV  2/23/90 05:00 Changed sysecho() to turn_echo(). */
/*: CR  4/09/90 17:31 Replace turn_echo() with systurn(). */
/*: CR  4/12/90 14:51 Eight-bit flag goes through systurn() now. */
/*: CR  5/02/90 12:35 Add break after flag.rawio. */
/*: CJ 11/22/90 17:20 Add editcom option. */
/*: CJ 11/22/90 22:35 Use new form of systurn() call. */
/*: CJ 11/26/90 16:14 RAWIO option must turn off cooking! */
/*: CJ 11/26/90 17:14 Add new OS_ECHO systurn() stuff. */
/*: CR  2/01/91 16:57 Add code for HANGUP characteristic. */
/*: CR  1/10/92 21:16 Remove global onofftable. */
/*: CR  9/04/92 14:18 Add logfeature handling. */
/*: DE  4/01/92 11:48 Chixified */
/*: JX  5/29/92 13:27 Remove flag.xlateio, rawio, japan, kanji. */
/*: CX  6/05/92 13:13 FILEINPUT option changes u_keybd. */
/*: JX  6/06/92 18:52 Add DISKFORMAT, TERMFORMAT options. */
/*: JX  7/13/92 10:49 Put JAPAN back in, allow TERMFORMAT USER. */
/*: JX 10/01/92 11:20 optchk() doesn't lowercase VALUE any more. */
/*: CI 10/07/92 22:34 Chix Integration. */
/*: TP 10/19/92 15:41 Use input_control() to set session file. */
/*: CR 10/30/92 12:37 Remove systurn()s, use flags instead; FILEOUTPUT. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: JD  7/26/93 10:39 Add flag.diskfull. */
/*: CP 12/04/93 14:02 Add underscore option. */
/*: CR  7/31/95 16:00 Add HANGOUT option 42. */
/*: CR  5/29/96 16:23 Add webpasswd option. */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"
#include "diction.h"
#include "units.h"

extern struct flag_template flag;
extern struct termstate_t   term_in_caucus;
extern int                  debug;
extern union  null_t        null;

FUNCTION  optset (option, value)
   int    option;
   Chix   value;
{
   int    on, index;
   int4   num;
   Chix   optonoff, low_value;

   optonoff   = chxalloc (L(16), THIN, "optset optonoff");
   low_value  = chxalloc (L(16), THIN, "optset low_value");

   /*** This is done later in utilglobals(), but do it here just in case. */
   null.str = NULL;
   chxcpy    (low_value, value);
   jixreduce (low_value);

   mdstr (optonoff, "optonoff", null.md);
   on = (tablematch (optonoff, low_value) == 0 ? 1 : 0);
   if (NOT chxnum (value, &num))  num = on;

   switch (option) {
      case ( 3):   flag.escape     = on;     break;
      case ( 5):   flag.editlist   = on;     break;
      case (11):   flag.newuser    = on;     break;
      case (12):   flag.port       = on;     break;
      case (14):   flag.redirect   = on;     break;
      case (15):   flag.login      = on;     break;
      case (17):   flag.verify     = on;     break;
      case (19):   flag.no1prompt  = on;     break;
      case (20):   flag.loop       = on;     break;
      case (23):   flag.expwarn    = on;     break;
      case (24):   flag.logbyconf  = on;     break;
      case (31):   flag.oneuser    = on;     break;
      case (33):   flag.editcom    = on;     break;
      case (34):   syshnginit (on);          break;
      case (37):   flag.logfeature = on;     break;
      case (40):   flag.diskfull   = on;     break;
      case (41):   flag.underscore = on;     break;
      case (42):   flag.hangout    = on;     break;

      case (38):
         flag.iocontrol = on;
         if (NOT flag.iocontrol)  systurn (&term_in_caucus, IS_AFILE, ON);
         break;

      case (21):   flag.eightbit   = on;     break;
/*       if (on)   systurn (&term_in_caucus, EIGHT_BIT, ON);  */

      case (22):   flag.japan = on;          break;

      case ( 6):   flag.echo       = on;     break;
/*       if (NOT on) systurn (&term_in_caucus, CAUCUS_ECHO + OS_ECHO, OFF);*/

      case ( 2):   flag.baud       = (int) num;      break;
      case ( 4):   debug           = (int) num;      break;
      case (13):   flag.print      = (int) num;      break;
      case (25):   flag.loctimeout = (int) num;      break;
      case (29):   flag.remtimeout = (int) num;      break;

      case ( 1):   flag.entrfile = chxalloc (L(20), THIN, "flag.entrfile");
                   chxcpy (flag.entrfile, value);    break;

      case (39):   /* FILEOUTPUT option redirects XITT to XIOF. */
         if (unit_lock (XIOF, WRITE, L(0), L(0), L(0), L(0), value)) {
            if (unit_make (XIOF)) {
               flag.outfile = chxalloc (L(40), THIN, "flag.outfile");
               chxcpy (flag.outfile,  value);
               unit_control (XITT, UNIT_SETSUB + XIOF);
            }
         }
         break;

      case ( 8): /* Take ALL input from input session file. */
         if (unit_lock (XISF, READ, L(0), L(0), L(0), L(0), value)) {
            if (unit_view (XISF))
                     input_control (IC_RED | IC_SESS | XISF, nullchix);
            else     unit_unlk (XISF);
         }
         break;

      case ( 9):   flag.logfile = chxalloc (L(20), THIN, "flag.logfile");
                   chxcpy (flag.logfile,  value);       break;
      case (18):   flag.exitfile = chxalloc (L(20), THIN, "flag.exitfile");
                   chxcpy (flag.exitfile, value);       break;

      case (43):   flag.webpasswd = chxalloc (L(60), THIN, "flag.webpasswd");
                   chxcpy (flag.webpasswd,  value);     break;

      case (44):   flag.sweb      = chxalloc (L(60), THIN, "flag.sweb");
                   chxcpy (flag.sweb,       value);     break;

      case (16):
         if (chxvalue (low_value,0L) == 'i')   flag.pctype = 0;
         if (chxvalue (low_value,0L) == 'z')   flag.pctype = 0;
         break;

      case (35): index = tablematch (CQ(LANG_STRING), low_value); /* DISK */
                 if (index >= 0) unit_control (0, UNIT_SET + index + 1);
                 break;

      case (36): index = tablematch (CQ(LANG_STRING), low_value); /* TERM */
                 if (index == LANG_USER)
                 { printf ("This doesn't work yet.\n"); break; }
                 unit_control (XITT, UNIT_SET + index + 1);
                 break;

      case (-1):
      default:      break;
   }
 
   chxfree (optonoff);
   chxfree (low_value);
}
