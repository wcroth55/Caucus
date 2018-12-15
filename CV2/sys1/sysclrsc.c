
/*** SYSCLRSCN.  Clear the screen for systems that support it. 
/
/    SYSCLRSCN is used only by "echo -c", which is used in turn
/    by the Caucus menus for some system to clear the screen before
/    displaying the menu.
/
/    Supported systems:
/
/      Vax/VMS: all ansi-style terminals (vt100, etc.), and vt52.
/               Uses VMS's knowledge of terminal type.
/
/      MSDOS:   just sends a ^L, seems to work OK on console.
/
/      UNIX:    uses Unix standard "clear" command.
/
/      AOS/VS:  sends ^L, Jeff V. says it works.
/  */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/25/89 15:21 New function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "sys.h"
#include "systype.h"

extern struct flag_template flag;

FUNCTION  sysclrscn()
{

#if NUT40 | WNT40 
   system ("clear");
}
#endif

#if UNIX
#define  MAXCLEAR   200
   static char clear[MAXCLEAR];
   Chix        userid;
   char        clrcom[200], clrfile[160];

   ENTRY ("sysclrscn", "");

   userid   = chxalloc (L(60), THIN, "sysclrsc userid");

   /*** On first call, try to get the character sequence that clears
   /    the screen and save it permanently in CLEAR. */
   if (NULLSTR(clear)) {

      /*** Try to put the sequence in temp file XITY. */
      sysuserid (userid);
      unit_name (clrfile, XITY, L(0), L(0), L(0), L(0), userid);

      /*** First, try the Unix 'clear' command with output redirection. */
      strconc   (clrcom, "clear >", clrfile);
      syscmd    (clrcom, SYS_WAIT);
      sysldclr  (clear, userid);

      /*** If that fails (e.g., SCO Unix), try 'tput clear'. */
      if (NULLSTR (clear)) {
         strconc  (clrcom, "tput clear >", clrfile);
         syscmd   (clrcom, SYS_WAIT);
         sysldclr (clear, userid);
      }
  
      /*** If that also fails, give up and guess ^L. */
      if (NULLSTR(clear))  strcpy (clear, "\014");
   }

   sysputstr (clear);

   chxfree ( userid );

   RETURN (1);
}

FUNCTION  sysldclr (clear, id)
   char  *clear;
   Chix   id;
{
   Chix  line;

   line   = chxalloc (L(200), THIN, "sysclrsc line");

   clear[0] = '\0';
   if (unit_lock (XITY, WRITE, L(0), L(0), L(0), L(0), id)) {
     if (unit_view (XITY)) {
        while (unit_read (XITY, line, L(0))) {
           if (NOT NULLSTR(clear)) strcat (clear, "\n");
           strcat (clear, ascquick(line));
           if (strlen (clear) >  MAXCLEAR - 80)  break;
        }
        unit_close (XITY);
     }
     unit_kill (XITY);
     unit_unlk (XITY);
  }

  chxfree ( line );
}
#endif
