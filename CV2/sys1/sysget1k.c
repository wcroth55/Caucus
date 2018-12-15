
/*** SYSGET1KEY.   Get a key-stroke from the user's terminal.
/
/    success = sysget1key (key);
/
/    Parameters:
/       int   success;   (Success/failure code.)
/       int4 *key;       (Key-stroke value.)
/
/    Purpose:
/       Read a single key-stroke from the keyboard, and return it in KEY.
/       Return a success or failure code value.  The key-stroke is *not*
/       echoed; that is up to the caller.
/
/       Function keys (that may send multiple bytes) are read as one "key"
/       and return a single value.
/
/       Characters (or keys) that otherwise send multiple bytes (such as
/       Japanese characters) are read one byte at a time, for each call
/       to sysget1key().
/
/    How it works:
/       Sysget1key() assumes that the caller has set the terminal driver
/       to "raw" mode, i.e. the characters are not interpreted by the
/       operating system.
/
/       If the key-stroke was a function key, sysget1key() puts a value
/       of 70000 + a special code, into KEY.  Sysget1key() does a lot of
/       special processing to detect such function keys.
/
/       Otherwise, the 1-byte value of the key, from 0 to 255, is placed
/       in KEY.
/
/    Returns:
/       0 if successful.
/      -2 if the local end-of-file key was pressed
/      -3 if the local interrupt ("<CANCEL>") key was pressed
/      -4 on hang-up, or catastrophic i/o error
/      -5 on indecipherable function key
/
/    Function key values:
/       Each of the "function key" values 70000-70199 have a special meaning.
/       They are defined in the tables below.  Note that the values shown
/       below are all offsets from 70000.  Not all keyboards can produce
/       all codes.
/ 
/       Table 1: keys common to many terminals
/                        Normal   Shift     CTRL    Shift-CTRL
/             Key Name   Value    Value     Value    Value
/          -----------------------------------------------------------
/             up arrow     101      121      141       161
/           left arrow     102      122      142       162
/           down arrow     103      123      143       163
/          right arrow     104      124      144       164
/
/       Table 2: vt100-specific keys
/                        Normal   Shift     CTRL    Shift-CTRL
/             Key Name   Value    Value     Value    Value
/          -----------------------------------------------------------
/            PF1           111      131      151       171
/            PF2           112      132      152       172
/            PF3           113      133      153       173
/            PF4           114      134      154       174
/
/    Error Conditions:  none
/ 
/    Side effects:      none
/
/    Related functions:
/
/    Called by:  Application.
/
/    Operating system dependencies:
/       Needs extern struct sysio_t sysio, which contains information
/       about how the terminal has been set.
/
/    Known bugs:
/       Sysget1key() gets confused if it sees an ESC followed immediately
/       by a legal ESC sequence.  It reports all of the characters as an
/       unknown ESC sequence.
/
/       CR would like to see more protection against possible sign extension
/       of the high order bit of "char c".
/
/    Home:  sys/sysget1k.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  2/02/90 17:29 Created function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include <stdio.h>
#include "handicap.h"
#include "caucus.h"
#include "systype.h"
#include "sys.h"
#if UNIX
#include <setjmp.h>
#endif

#define  TAPELEN 6     /* Length of virtual tape on virtual tapedrive. */

#if UNIX | NUT40 | WNT40
#define  ESC    '\033'   /* Escape key. */
#define  CTLH   '\010'   /* Control-H, backspace. */
#define  DEL    '\177'   /* Del key. */
#define  BELL   "\007"   /* String containing bell (^G). */
#define  RS     '\036'   /* ASCII RS code. */
#endif

#define ERASE(tape)  {for (i=0; i<TAPELEN; i++) tape[i]='\0';   tapehead=0; }

/*** The READCHAR macro handles actually reading a single byte from
/    the terminal for a particular operating system. */

#if UNIX
#define  READCHAR(n,c)    n = read (0, &c, 1)
#endif

#if SUN41
#undef   READCHAR
#define  READCHAR(n,c)    sysb_read=1, n = read (0, &c, 1), sysb_read=0
#endif

extern struct sysio_t       sysio;

/*** Full-window systems have a no-op for sysget1key(). -------------- */
#if NUT40 | WNT40
FUNCTION  sysget1key (c)
   int4  *c;
{
   ENTRY  ("sysget1key", "");
   if (syshngtest())  RETURN (-4);
   RETURN (0);
}
#endif


/*** Here's the real code for most normal operating systems. ---------- */
#if UNIX
FUNCTION  sysget1key (key)
   int4  *key;
{
   static char  tapedrive[TAPELEN] = {'\0', '\0', '\0', '\0', '\0', '\0'};
   static short tapesize=0;    /* Size of valid data on tape. */
   static short tapehead=0;    /* Position of tape r/w head. */
   short  i;
   short  byte;
   char   c;
   register int n;

#if SUN41           /* Only need stupid longjmp'ing stuff for BSD. */
   extern int     sysb_flag, sysb_read;
   extern jmp_buf sysb_env;
#endif

   ENTRY ("sysget1key", "");

/*** SYSGET1KEY is a simple automaton, in the classic computer
/    science sense.  It reads one byte at a time from the keyboard,
/    returning single-byte keystrokes to the caller.
/    It attempts to interpret multiple-byte keystrokes and translate
/    them into one of the codes above.
/
/    Result:
/       1) Function Keys: if the first byte is one of a very select
/          few (sort of like the Marines) we continue reading
/          bytes until we can decide we have a valid function key
/          or not.  If it's a valid function key, we either copy
/          a value from the above tables into KEY or take some
/          appropriate action.  If it's not a valid function key,
/          we've got a problem.  We copy the first byte into KEY
/          and store the later bytes for later calls to SYSKET1KEY.
*/
   if (syshngtest())  RETURN (-4);

   /*** SYSGET1KEY() uses a "virtual tapedrive" to record bytes from
   /    a multiple-byte keystroke.  If the keystroke is decipherable,
   /    put the f-key value into KEY and return.  Otherwise, return
   /    individual bytes from the keystroke one-byte-per-call, FIFO. */

   if (tapesize) {            /* If anything from the last keystroke... */
      *key = tapedrive[tapehead++];     /* Get byte from FIFO tape and... */
      tapesize--;                                      /* move tape head. */
      if (NOT tapesize) ERASE (tapedrive);    /* If we've read last byte. */
      RETURN (0);
   }


#if SUN41
   /*** Save the Current ENVironment.  If we come back from SETJMP with a non-
   /    zero return, a read from the terminal was interrupted by CANCEL. */
   if (setjmp (sysb_env)) { sysbrkset(); tapesize = 0; RETURN (-3); }
#endif

   /*** Read bytes until we've gotten a single, ordinary character, or
   /    we're at the end of a multi-byte function key. */
   while (1) {
      READCHAR (n, c);
      byte = c & 0xFF;
  
      if (n == 0  &&  run_away_proc(1))  sysexit(0);      /* Runaway process */
      if (sysbrktest())                  RETURN (-3);     /* <CANCEL> */
      if (n == 0)                        RETURN (-2);     /* EOF */
      if (syshngtest())                  RETURN (-4);     /* Catastrophic Error. */
      if (n <  0)                        RETURN (-4);     /* Other Catastrophic Error. */
  
      run_away_proc(1);
      if (sysio.state.eightbit <= 0)  byte = byte & 0177;
  
      /*** If a regular ASCII character, get it out quick. */
      if (tapesize == 0  &&  byte > 31  &&  byte < 127) {
         *key = byte;
         RETURN (0);
      }
  
      /*** Test for interrupt character. */
      if (byte == sysio.c_intr)               { sysbrkset();   RETURN (-3); }
  
      /*** Test for end-of-file character. */
      if (byte == sysio.c_eof)                                 RETURN (-2);
  
      /*** Any remaining non-ESC control characters. */
      if (tapesize == 0  &&  byte != ESC)  { *key = byte;      RETURN (0); }

      /*** The remaining code handles the vt100 function keys, which produce
      /    multiple-byte sequences that begin with ESC.  The sequences are
      /    stored in a "virtual tapedrive". */

      /*** An ESC by itself starts a multi-byte function key. */
      if (tapesize == 0  &&  byte == ESC) {
         tapedrive[0] = ESC; tapesize = 1;
         tapehead = 1;
         continue;
      }
  
      /*** PF1 through PF4. */
      if (tapesize == 1) {
         if (byte == 'P')  { *key = L(70111);   break; }
         if (byte == 'Q')  { *key = L(70112);   break; }
         if (byte == 'R')  { *key = L(70113);   break; }
         if (byte == 'S')  { *key = L(70114);   break; }
      }

      /*** 2nd byte of 3-byte keys. */
      if (tapesize == 1  &&  (byte == '[' || byte == 'O')) {
         tapedrive[1] = byte; tapesize = 2;
         tapehead = 2;  continue;
      }

      /*** Arrow keys. */
      if (tapesize == 1  ||  tapesize == 2) {
         if (byte == 'A')  { *key = L(70101);   break; }
         if (byte == 'B')  { *key = L(70103);   break; }
         if (byte == 'C')  { *key = L(70104);   break; }
         if (byte == 'D')  { *key = L(70102);   break; }
      }

      /*** If we've gotten here, this is a multi-byte sequence that we
      /    can't interpret.  Use the virtual tapedrive to send all the
      /    bytes back to the caller, one per call. */
      tapedrive[tapehead] = byte;                  /* Write this byte. */
      *key = tapedrive[0];                         /* Read first byte. */
      tapehead=1;                    /* Next time read from next byte. */
      RETURN (0);                        /* TAPESIZE remains the same. */
   }

   /*** If we broke out of the loop, erase the virtual tapedrive and return. */
   ERASE (tapedrive);
   tapesize = 0;
   RETURN (0);
}
#endif
