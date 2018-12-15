
/*** UNIT_WRITE.  Write text in STR to file open on unit N.
/
/    unit_write (n, str);
/
/    Parameters:
/       int   n;    (unit number of open file)
/       Chix  str;  (text to be written)
/
/    Purpose:
/       Write text of chix STR to file open on unit N.
/
/    How it works:
/       STR may contain any text, including multiple lines.
/
/       Special considerations:
/          Unit N may be an open file, the bit bucket (XIBB), or
/          one of the unit codes that goes to the terminal (XITT,
/          XWTX, etc.)
/
/          If the 1st character of STR is '\01' (NEWCPAGE), this
/          signifies "start a new page" to files & devices for which
/          this has meaning.
/
/          Output sent to the terminal will be word-wrapped according
/          to the value of the global screen.width.
/
/       Unit_write() calls unit_control() to find out what kind of
/       character set is used to write the output.
/
/       Output to the terminal passes through the following chain
/       of functions:
/          unit_write()      (decide where output is going)
/          unit_put()        (perform word-wrapping and do paging)
/          unit_term()       (convert to proper character representation)
/          sysputstr()       (really write to the terminal)
/       
/    Returns:
/
/    Error Conditions:
/
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: none
/
/    Known bugs:
/
/    Home:  unit/unitwrit.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  5/12/92 15:49 Extensively rewritten for chix. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "units.h"
#include "diction.h"

/*** Define PUTS macro for writing bytes to a file. */
#if UNIX | NUT40 | WNT40
#define  PUTS(s)  fputs (s, units[n].fp)
#endif

extern struct unit_template units[];
extern struct flag_template flag;
extern Userreg              thisuser;

static char firstpage = 1;

FUNCTION  unit_write (n, str)
   int  n;
   Chix str;
{
#define   BUFSIZE  L(512)
   char   buf[BUFSIZE];
   int    lang, inc;
   int4   pos;
   char  *bugtell();

   ENTRY ("unitwrite", "");

   if (n <= XITT) {
      n = unit_control (n, UNIT_GETSUB);
      if (syshngtest()  &&  NOT flag.hangout)  n = XIBB;
   }

   /*** Writes to the bit bucket are thrown away. */
   if (n == XIBB)   RETURN (1);

   if (n > XITT  &&  units[n].access != WRITE)
      buglist (bugtell ("Unitwrite: ", n, L(0), "writing to unopened file!\n"));

   if (n==XKEY  ||  n==XWTZ)   RETURN(1);

   /*** New page indicator?  If so, and output is going to the printer
   /    unit (XIPT), and this is not the very first newpage to be sent
   /    (i.e., printer starts at top of form!), write the user's selected
   /    new page character. */
   if (chxvalue (str, L(0)) == '\01') {
      if (n==XIPT  &&  thisuser->newpage>1  &&  NOT firstpage) {
         buf[0] = thisuser->newpage;
         buf[1] = '\0';
         PUTS (buf);
      }
      firstpage = 0;
      RETURN (1);
   }

   /*** If one of the terminal unit codes, have unit_put() do the
   /    dirty work. */
   if      (n==XWTY  ||  n==XWPM)  { unit_put (str, 0);   RETURN (1); }
   else if (n<=XITT)               { unit_put (str, 1);   RETURN (1); }

   else if (EMPTYCHX (str))        RETURN (1);

   /*** Otherwise, if there's anything at all to write, convert it
   /    to bytes a BUFSIZE at a time, and write it out with PUTS. */
   lang = unit_control (XIRE, UNIT_GET);
   for (pos=0;   (1);   pos += inc) {
      switch (lang) {
         case (LANG_NORM):
         case (LANG_ISO):  inc = ascofchx  (buf, str, pos, BUFSIZE);  break;
         case (LANG_EUC):  inc = eucofjix  (buf, str, pos, BUFSIZE);  break;
         case (LANG_SJIS): inc = sjisofjix (buf, str, pos, BUFSIZE);  break;
         default:          inc = -1;                                  break;
      }
 
      PUTS (buf);
      units[n].written += strlen(buf);
      if (inc <= 0)  break;
   }

   RETURN (1);
}
