/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** OPTCHK.  Parse and set flags from master.opt "options" file. 
/  
/    This is a new function, for Caucus 2.2+.  It fulfills the  
/    functionality put into argchk() (for Caucus 2.1) that
/    parses the options file (the so-called 'master.opt' file).
/    This new method allows certain options to be specified on
/    the command line, and the rest to be specified in the options
/    file.  This seperation was necessary for the client/server
/    versions, to allow server options to be set before Caucus
/    could access the dictionary.  After the client and server
/    are connected, the client can access the strings for the text
/    used in the options file, allowing foreign language options files.
/
/    The use of an options file is specified on the command line
/    by the '-y' argument.  If found, it scans the options specified
/    in the options file "filename" and process them.  OPTCHK returns
/    1 if the options file was found and parsed successfully, and
/    0 otherwise.
/
/    **NOTE: the options file must reside in the Caucus home 
/    directory, and should be specified on the command line
/    without the home directory prefix!!**
/
/    The complete list of options is shown below, aint4 with the
/    old flag name for the relevant old command line option flag.
/    Some of the old flag names are no longer valid.  The options
/    file should be used for specifying these options. 
/
/    All options require a value, which must be one of:
/
/       the word ON or the word OFF
/       an integer number  (shown as "n", below)
/       the name of a file (shown as "filename", below)
/       a choice from a list (show in upper-case, below)
/
/ Old    Option
/Flag #  name       Value  Default  Meaning
/ -- -- ----------  -----  -------  -----------------------------------------
/ -c 03 COMMAND     ON/OFF   (OFF)  allow "!" command escaping
/ -q 20 CAPLOOP     ON/OFF   (OFF)  Captive login cycles, never exits 
/ -s 15 CAPTIVE     ON/OFF   (OFF)  Use Caucus captive login shell
/    35 DISKFORMAT  ASCII    (ASCII) Format of data on disk
/                   EUC
/                   ISO8859
/                   SJIS
/ -f 06 ECHO        ON/OFF   (ON)   Echoing of characters typed by user
/    33 EDITCOM     ON/OFF   (ON)   Allow use of EDIT command at AND NOW
/ -e 05 EDITLIST    ON/OFF   (OFF)  Restrict editors to M_editlist names
/ -a 01 ENTRYFILE   filename        Text displayed when Caucus starts up
/ -x 18 EXITFILE    filename        Text displayed when Caucus exits
/ -w 23 EXPIRATION  ON/OFF   (OFF)  Warn users about expiration date
/ -z 19 FASTPROMPT  ON/OFF   (OFF)  Do not display 1 or 2 char prompts
/ -i 08 FILEINPUT   filename        User input read from file, not terminal
/    34 HANGUP      ON/OFF   (ON)   Detect & handle hangups (ON), ignore (OFF)
/    42 HANGOUT     ON/OFF   (ON)   Display prompts & output after hangup?
/ -j 22 JAPANESE    ON/OFF   (OFF)  Use JIX internally, assume sort by 1st name
/ -u 26 JISTRANS    ON/OFF   (OFF)  (Obsolete)
/                                   Japanese JIS coding scheme translation
/ -w 27 KANJITERM   n        (Obsolete) Type of Kanji (japanese) terminal
/ -l 09 LOGUSER     filename        Log user entry/exit in file
/ -u 24 LOGCONF     ON/OFF   (OFF)  Log user entry/exit by conference
/ -n 11 NEWUSER     ON/OFF   (ON)   Allow captive users to register as NEWUSER
/    31 ONEUSER     ON/OFF   (ON)   Restrict to one person per id at a time
/ -p 13 PRINT       n        (0)    Use PRINT modifier? (0=no, 1=sys, 2=user)
/ -g 28 RAWIO       ON/OFF   (OFF)  (Obsolete)
/                                   Caucus does i/o "raw", 1 char at a time
/ -r 14 REDIRECTION ON/OFF   (OFF)  Allow input/output redirection
/    36 TERMFORMAT  ASCII    (ASCII) Format of data from terminal
/                   EUC
/                   ISO8859
/                   SJIS
/    43 WEBPASSWD   file            Use web password file for captive use
/    44 SWEB        file            swebd.conf configuration file
/     */

/*: JV  8/04/89 11:36 Created optchk(). */
/*: CR  6/07/89 15:44 Ensure opt>=0 before calling optset(). */
/*: CR 10/06/89 17:53 Clean up logic, add return value. */
/*: CR 10/10/89 23:17 Move default initialization to argchk(). */
/*: CR 10/30/89 12:24 Add declaration of strtoken(). */
/*: CR 11/14/89 17:39 Use XSMO instead of XIRE. */
/*: JV 12/06/89 16:15 Added warning about pathnames in header!! argh! */
/*: CR  1/09/90 10:50 Expand size of line[]. */
/*: JV  2/20/90 07:35 Added ONEUSER option. */
/*: JV  2/27/90 08:29 Add better error handling. */
/*: CR  3/26/90 18:31 Don't complain if no optfile. */
/*: CJ 11/22/90 17:23 Add EDITCOM to comments. */
/*: CR  2/01/91 16:53 Add HANGUP to comments. */
/*: CR  3/02/91 11:38 Resolve conflicting options. */
/*: CR  4/08/91 18:49 Add new unit_lock argument. */
/*: JV 10/22/91 16:00 Fix diagnostics. */
/*: UL 12/11/91 18:42 Make unit_lock args 3-6 int4.*/
/*: CR  1/02/92  9:43 Map tabs in master.opt to spaces. */
/*: CR  1/10/92 21:15 Remove global onofftable. */
/*: JX  5/14/92 14:50 Chixify. */
/*: JX  6/06/92 18:48 Add DISKFORMAT TERMFORMAT; rm jistrans rawio kanjiterm */
/*: JX 10/01/92 11:13 Don't lowercase arg VALUE to optset(). */
/*: CL 12/10/92 14:52 Long chxalter args. */
/*: CL 12/11/92 13:47 Long Chxvalue N. */
/*: CR  7/31/95 16:00 Add HANGOUT option. */
/*: CR  5/29/96 16:23 Add webpasswd option. */

#include <stdio.h>
#include "caucus.h"

extern struct flag_template flag;
extern union  null_t        null;

FUNCTION  optchk ()
{
   static Chix optiontab;
   Chix   on, off, line, option, value, optonoff, low_line;
   int    opt, success;
   char  *bugtell();

   ENTRY ("optchk", "");
   
   off       = chxalloc (L(40),  THIN, "optchk off");
   on        = chxalloc (L(40),  THIN, "optchk on");
   line      = chxalloc (L(152), THIN, "optchk line");
   value     = chxalloc (L(60),  THIN, "optchk value");
   option    = chxalloc (L(60),  THIN, "optchk option");
   low_line  = chxalloc (L(100), THIN, "optchk low_line");
   optonoff  = chxalloc (L(100), THIN, "optchk optonoff");
   flag.webpasswd = nullchix;
   flag.sweb      = nullchix;
   success   = 0;

   mdstr (optonoff, "optonoff", null.md);
   chxtoken (off, null.chx, 2, optonoff);
   chxtoken (on,  null.chx, 1, optonoff);

   /*** Process optfile, if it exists. */
   if (EMPTYCHX (flag.optfile))   FAIL;

   if (NOT mdtable ("gen_Aopttab", &optiontab)  ||
       NOT unit_lock (XSMO, READ, L(0), L(0), L(0), L(0), flag.optfile)) {
          buglist (bugtell ("No optiontab/can't lock XSMO.", 
                    XSMO, L(0), "")); 
          FAIL; 
       }

   if (unit_view (XSMO)) {
      while (unit_read (XSMO, line, L(0))) {
         if (chxvalue(line,0L) == '#')  continue;
         while (chxalter (line, L(0), chxquick("\t",0), chxquick(" ",1))) ;
         chxcpy (low_line, line);

         jixreduce (low_line);
         chxtoken (option, null.chx, 1, low_line);
         if ( (opt = tablematch (optiontab, option)) < 0) continue;

         chxtoken (value,  null.chx, 2, line);
         optset (opt, value);
      } 
      unit_close (XSMO);
   } else buglist (bugtell ("Optchk", XSMO, L(0), "Couldn't unitview.\n"));
   unit_unlk (XSMO);

   /*** Resolve conflicting options.  Specifically:
   /    The "-v" autologin forces CAPLOOP OFF.  */
   if (NOT EMPTYCHX(flag.autologin))  flag.loop = 0;

   SUCCEED;

 done:
   chxfree (off);
   chxfree (on);
   chxfree (line);
   chxfree (value);
   chxfree (option);
   chxfree (low_line);
   chxfree (optonoff);

   RETURN    (success);
}
