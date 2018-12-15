
/*** SYSRDLINE.   Read a LINE of bytes from the user's terminal.
/
/    code = sysrdline (buf, maxbuf);
/
/    Parameters:
/       int   code;      (return status code)
/       char  buf[];     (buffer to place bytes read from terminal)
/       int   maxbuf;    (maximum size of BUF)
/
/    Purpose:
/       Sysrdline() reads an entire line of bytes from the user's terminal
/       and places them in BUF, aint4 with a final 0 byte.
/
/    How it works:
/       If there is a terminating newline, it is thrown away.
/       Only the first MAXBUF-1 bytes are placed in BUF; the rest
/       are thrown away.
/
/       For most systems, sysrdline() is intended to be used with input
/       character processing ("cooking" and echoing) handled by the operating
/       system.
/
/       The AOS/VS version of sysrdline() handles input one keystroke at
/       a time, and also handles special function key interrupts that invoke
/       other services.
/
/    Returns:
/       0   normal return, input line in BUF.
/      -1   normal return, input was longer than MAXBUF, input truncated
/      -2   end-of-file
/      -3   cancel/interrupt
/      -4   hangup or catastrophic error
/      -5   other/unknown error
/
/    Error Conditions:
/
/    Side effects:
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies: lots
/
/    Known bugs: 
/
/    Home:  sys/sysrdlin.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CX  5/29/92 16:22 New function, from old sysread(). */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "sys.h"

extern struct sysio_t       sysio;

/*** Cases that must be considered.  N is # of characters read.
/    (a) <CANCEL> found.  Empty line and go home.
/    (b) n==0.  End-of-file-char(?) at start of line, EOT==1EOF.
/    (c) n==0.  All other cases.  Ignore?  Cause error?  Which?
/    (d) n <0.  Hangup... or worse? 
/    (e) n==1.  Normal character, < maxlen chars.
/    (f) n==1.  Normal character, > maxlen chars.  Echo?  Beep?  What?
/    (g) n==1.  Control character or function key.  */


/*------------------------------- Unix, and VM-----------------------*/
#if SYSV | HPXA9 | NUT40 | WNT40

FUNCTION sysrdline (line, maxlen)
   char  line[];
   int   maxlen;
{
   int    c, last;

   ENTRY ("sysrdline", "");

   if (maxlen == 0)  maxlen = 10;

   if (syshngtest())  { line[0] = '\0';   RETURN (-4); }

   /*** Normally, we read up to MAXLEN chars from the terminal.  If
   /    the read fails, it's either hangup, cancel, or EOF. */
   if (fgets (line, maxlen, stdin) == NULL)  {
      line[0] = '\0';
      sysputstr ("\n");
      if (run_away_proc(1))  sysexit(0);
      if (syshngtest())      RETURN (-4);
      if (sysbrktest())      RETURN (-3);
      RETURN (-2);
   }

   /*** Was the line ended cleanly? */
   run_away_proc (0);
   last = strlen(line) - 1;
   if (line[last] == '\n')  { line[last] = '\0';   RETURN (0); }

   /*** No, we truncated it.  Throw away everything up to real end of line. */
   line[maxlen-1] = '\0';
   do  { c = getchar(); }  UNTIL (c < 0  ||  c=='\n');
   if (syshngtest())   RETURN (-4);
   if (sysbrktest())   RETURN (-3);
   RETURN (-1);
}
#endif


/*---------------------------------Berkeley Unix-------------------------*/
#if SUN41 | LNX12 | BSD21 | FBS22
#include <setjmp.h>

FUNCTION sysrdline (line, maxlen)
   char  line[];
   int   maxlen;
{
   extern int     sysb_flag, sysb_read;
   extern jmp_buf sysb_env;
   int    ended, n,len;
   char   c;

   ENTRY ("sysrdline", "");

   if (maxlen == 0)  maxlen = 10;

   if (syshngtest())  { line[0] = '\0';   RETURN (-4); }

   /*** Save the current ENVironment.  If we come back from SETJMP with a non-
   /    zero return, a read from the terminal was interrupted by CANCEL. */
   if (setjmp (sysb_env)) { 
      line[0] = '\0';
      if (run_away_proc(1))  sysexit(0);
      if (syshngtest())      RETURN (-4);
      if (sysbrktest())      RETURN (-3);
      RETURN (0); 
   }

   /*** Read the line; we may get it in pieces and have to assemble it
   /    into one line.  */
   run_away_proc (0);
   len = 0;
   do {
      sysb_read = 1;
      n  = read (0, line+len, maxlen-len-1);
      sysb_read = 0;

      if (n < 0  ||  len+n == 0) {
         line[0] = '\0';
         sysputnl();
         RETURN  (-2);
      }

      len += n;
      line[len] = '\0';
      if (len > 0) ended = (line [len-1] == '\012');
      else         ended = 0;
   } while (NOT ended  &&  len < maxlen-1);

   if (ended) { line[len-1] = '\0';    RETURN (0); }

   /*** Here if we got more than MAXLEN-1 chars.  Read and toss
   /    everything until we see a newline. */
   line[maxlen-1] = '\0';
   while  (read (0, &c, 1) == 1   &&   ( (c & 0177) != '\012') ) ;
   if (syshngtest())  RETURN (-4);
   if (sysbrktest())  RETURN (-3);
   RETURN (-1);
}
#endif
