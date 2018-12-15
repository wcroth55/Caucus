/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** NET_OPTCHK.  Parse and set flags from net.opt "options" file. 
/  
/    Net_optchk is the same as gen/optchk(), but for net.opt.
/
/    The use of an options file is specified on the command line by
/    the '-y' argument.  If found, it scans the options specified in
/    the options file "filename" and process them.  NET_OPTCHK returns
/    1 if the options file was found and parsed successfully, and
/    0 otherwise.
/
/    **NOTE: the options file must reside in the Caucus home 
/    directory, and should be specified on the command line
/    without the home directory prefix!!**
/
/    All options require a value, which must be one of:
/
/       the word ON or the word OFF
/       an integer number  (shown as "n", below)
/       the name of a file (shown as "filename", below)
/
/    Option
/ #  name          Value  Default  Meaning
/-- ----------     -----  ------- 
          -------------------------------------------
/01 LOGHOSTMAILIN  ON/OFF   (OFF)  Log mail coming from each host
/02 LOGHOSTMAILOUT ON/OFF   (OFF)  Log mail going  to   each host
/03 LOGHOSTCONFIN  ON/OFF   (OFF)  Log conf stuff coming from each host
/04 LOGHOSTCONFOUT ON/OFF   (OFF)  Log conf stuff going  to   each host
/05 LOGPERSIN      ON/OFF   (OFF)  Log person info coming from each host
/06 LOGPERSOUT     ON/OFF   (OFF)  Log person info going  to   each host
/07 LOGUSERCONF    ON/OFF   (OFF)  Log conf stuff added by each user   
/08 LOGUSERMAILIN  ON/OFF   (OFF)  Log mail sent by each person
/09 LOGUSERMAILOUT ON/OFF   (OFF)  Log mail received by each person
/10 LOGCONFIN      ON/OFF   (OFF)  Log conf stuff received for each conf
/11 LOGCONFOUT     ON/OFF   (OFF)  Log conf stuff sent     for each conf
/12 XFPTIME        ON/OFF   (OFF)  Log starts and stops of packager
/13 XFUTIME        ON/OFF   (OFF)  Log starts and stops of unpackager
/     */

/*: JV  8/04/89 11:36 Created net_optchk(). */
/*: JV 10/30/91 12:04 Clean up. */
/*: UL 12/11/91 18:44 Make unit_lock args 3-6 int4.*/
/*: JV  1/03/92 15:02 Remove unused var 'optiontab'. */
/*: CR  1/10/92 21:19 Remove global onofftable. */
/*: CR  1/13/92 17:06 Add onoff arg to net_optset(). */
/*: DE  6/04/92 16:13 Chixified */
/*: JX  9/01/92 15:37 Fixify. */
/*: CL 12/11/92 13:48 Long Chxvalue N. */

#include <stdio.h>
#include "chixuse.h"
#include "done.h"
#include "handicap.h"
#include "unitcode.h"
#include "xfp.h"

extern struct net_flag_t    net_flag;
extern union  null_t        null;

FUNCTION  net_optchk ()
{
   static Chix net_opttab; 
   Chix   line, option, value, optonoff;
   Chix   off, on;
   int    opt, success;
   char  *bugtell();

   ENTRY ("net_optchk", "");
   
   if (NOT chxtype(net_opttab))
      net_opttab = chxalloc (L(230), THIN, "netoptch net_opttab");

   on         = chxalloc (L(40),  THIN, "netoptch on");
   off        = chxalloc (L(40),  THIN, "netoptch off");
   line       = chxalloc (L(152), THIN, "netoptch line");
   value      = chxalloc (L(60),  THIN, "netoptch value");
   option     = chxalloc (L(16),  THIN, "netoptch option");
   optonoff   = chxalloc (L(100), THIN, "netoptch optonoff");
   success    = 0;

   chxofascii (net_opttab, "loghostmailin loghostmailout loghostconfin \
      loghostconfout logpersin logpersout   loguserconf    logusermailin \
      logusermailout logconfin logconfout   xfptime        xfutime");
   mdstr (optonoff, "optonoff", null.md);
   chxtoken (off, null.chx, 2, optonoff);
   chxtoken (on,  null.chx, 1, optonoff);

   if (NOT unit_lock (XSMO, READ, L(0), L(0), L(0), L(0), net_flag.optfile)) {
      buglist (bugtell ("OPTCHK: Can't lock XSMO.", XSMO, L(0), ""));
      FAIL;
   }

   if (unit_view (XSMO)) {
      while (unit_read (XSMO, line, L(0))) {
         if (chxvalue(line,0L) == L('#'))  continue;
         jixreduce (line);
         chxtoken (option, null.chx, 1, line);
         chxtoken (value,  null.chx, 2, line);
         if ( (opt = tablematch (net_opttab, option)) < 0) continue;
         net_optset (opt, value, optonoff);
      } 
      unit_close (XSMO);
   } else buglist (bugtell ("Net_optchk", XSMO, L(0), "Couldn't unitview.\n"));
   unit_unlk (XSMO);

   SUCCEED;
 done:
   chxfree ( line );
   chxfree ( value );
   chxfree ( optonoff );
   chxfree ( on );
   chxfree ( off );

   RETURN ( success );
}
