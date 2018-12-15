
/*** INPUT_CONTROL.  Get or control text input.
/
/    status = input_control (verb, line);
/
/    Parameters:
/       int     status;  (resulting status)
/       int4    verb;    (control verb)
/       Chix    line;    (line of read data placed here)
/
/    Purpose:
/       Get a line of input OR control where input comes from.
/
/       Most of the time Caucus reads command, data, and menu input
/       directly from the keyboard.  However, the "<", "<<", and
/       "_callmenu Name Vn <file" commands temporarily need to redirect
/       input to come from various sources for different purposes.
/
/       Input_control() attempts to provide a single access and control
/       point for reading data and controlling where the input comes
/       from.
/
/    How it works:
/       There are four "kinds" of input:
/          IC_SESS    Caucus session input (keyboard or FILEINPUT file)
/          IC_DATA    command and/or data input
/          IC_COMM    command *only* input
/          IC_MENU    menu choice input (for _callmenu command)
/
/       There are four things you can do with each kind:
/          IC_RED     redirect the input to come from an open file
/          IC_GETLIN  read a line of text from the input source
/          IC_CLOSE   close redirection
/          IC_ISUNIT  get unit number currently in use
/
/       Each of these "things" is described below:
/       
/       REDIRECTION:
/          To redirect a "kind" to take its input from a file (that is
/          already locked and open) on "unit":
/             input_control (IC_RED | kind | unit, nullchix);
/
/       INPUT:
/          To read a line of input, use one of:
/             status = input_control (IC_GETLIN | kind, line);
/             status = input_control (IC_GETEOF | kind, line);
/   
/          Both forms read from the unit associated with "kind".  If
/          the kind was redirected and the end of file is reached, the
/          file is closed and unlocked, and the "kind" is reverted to
/          the appropriate default (see "DEFAULTS", below).
/   
/          If the EOF condition occurs with IC_GETLIN, input_control()
/          reads a line from the default unit and puts it in LINE and
/          returns 1.  In the same case With IC_GETEOF, no line is read
/          and input_control() returns 0.  (The *next* call to input_control
/          will get a line from the default input source.)
/
/       CLOSE:
/          To revert (or "close") a kind back to its default unit, 
/          (before reaching the EOF):
/             input_control (IC_CLOSE | kind, nullchix);
/
/       UNIT:
/          To find out what unit number the "kind" is using:
/             unit = input_control (IC_UNIT | kind, nullchix);
/
/       DEFAULTS:
/          There is a heirarchy of "kinds":
/             If IC_COMM is not redirected, it reads from IC_DATA.
/             If IC_DATA is not redirected, it reads from IC_SESS.
/             If IC_MENU is not redirected, it reads from IC_SESS.
/             IC_MENU is either XKEY (the keyboard) or XISF (the
/                "session" file from FILEINPUT), and once set, never
/                reverts.
/
/    Returns: 1 normally
/             0 if EOF encountered on IC_GETEOF
/             0 if EOF encountered and kind is IC_SESS
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:
/
/    Operating system dependencies:
/
/    Known bugs:      none
/
/    Home:  inout/inputcon.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/15/92 18:14 New function. */

#include <stdio.h>
#include "caucus.h"
#include "inputcon.h"

#define  NONE        -200
#define  INDEX(x)    ((x) / IC_SESS - 1)

FUNCTION  input_control (verb, line)
   int4   verb;
   Chix   line;
{
   static struct { int unit;   int next; } 
          kindof[4] = { {XKEY, 0}, {NONE, 0}, {NONE, 1}, {NONE, 0} };
   int4   action;
   int    unit, kind, row;

   ENTRY ("input_control", "");

   unit   =       (verb & IC_UNIT);
   kind   = INDEX (verb & IC_KIND);
   action =       (verb & IC_VERB);

   if (action==IC_GETLIN  ||  action==IC_GETEOF) {

      while (1) {
         /*** Step down through KINDOF until we find what unit is the
         /    current default for this KIND. */
         for (row=kind;   kindof[row].unit == NONE;   row = kindof[row].next) ;

         /*** Read a line from that unit. */
         if  (unit_read (kindof[row].unit, line, L(0)))  RETURN (1);

         /*** If we can't, close any redirection for this unit, and
         /    either return immediately (IC_SESS or any IC_GETEOF),
         /    or loop back and try again with whatever the next kind
         /    in line is. */
         if (row    == INDEX(IC_SESS))     RETURN (0);
         input_close (&kindof[row].unit);
         if (action == IC_GETEOF)          RETURN (0);
      }
   }

   /*** Redirect a KIND.  (If it was already redirected, close the
   /    old unit.  This may be a mistake... or maybe the calling code
   /    will have been careful enough to do an IC_CLOSE). */
   if (action==IC_RED) {
      input_close (&kindof[kind].unit);
      kindof[kind].unit = unit;
   }

   if (action==IC_CLOSE)    input_close (&kindof[kind].unit);

   if (action==IC_ISUNIT) {
      for (row=kind;   kindof[row].unit == NONE;   row = kindof[row].next) ;
      RETURN (kindof[row].unit);
   }

   RETURN (1);
}


FUNCTION  input_close (unit)
   int   *unit;
{
   if (*unit==NONE  ||  *unit==XKEY)  return (1);

   unit_close (*unit);
   unit_unlk  (*unit);
   *unit = NONE;
   return (1);
}
