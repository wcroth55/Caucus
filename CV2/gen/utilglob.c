/*** UTILGLOBALS.   Global data structures used by many Caucus
/    "utility" functions.
/
/    utilglobals();
/
/    Parameters: None.
/
/    Purpose:
/       There are several different programs (Caucus, caumnt, xfu, xfp)
/       which call a common set of Caucus utility functions (such as 
/       loadnpart(), unit_write(), etc.).  These utility functions share 
/       a set of global data structures, which in turn need a single place
/       where they are defined.  Unitglobals() is that place.
/
/       Utilglobals() also performs some minimal initialization of some
/       of these data structures.  It may be called many times without
/       harm to the contents of those data structures.
/
/    How it works:
/
/    Returns: 1
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Called by:  main programs
/
/    Operating system dependencies:
/
/    Known bugs:
/
/    Home:  gen/utilglob.c.
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/02/91 15:22 New function. */
/*: JX  5/13/92 14:53 Chixify. */
/*: CX  6/05/92 16:51 Main() allocates confid. */
/*: JX  6/18/92 10:06 Main() allocates confname. */
/*: CR 10/15/92 18:03 Comment out unused i. */
/*: JV 11/17/92 16:22 Add HOSTNAMES array and NUMHOSTS int. */
/*: CR 11/27/92 13:32 Declare sysmem(). */
/*: CL 12/11/92 15:52 Long sysmem arg. */
/*: JV  3/09/93 09:43 Add attach_def. */
/*: CP  8/01/93 14:32 Add 'name' arg to make_textbuf() call. */
 
#include <stdio.h>
#include "caucus.h"

/*** The Textbuf textbuffer structure replaces the TEXT, LINE, and BLOCK
/    structures (which are commented out below). */
Textbuf tbuf;


/*** The globals TEXT, LINE, and BLOCK make up the "text buffer"
/    structures that contain the in-memory copies of text partfiles
/    (such as response partfiles, namelist partfiles, etc.) */
/* static char  textbuf  [SYSTEXT];      buf for partfiles 
/  static short lineptrs [SYSLINE];      ptrs to each line in textbuf
/  static short bstart   [SYSBLOCK];     ptrs to each {resp, mess, etc}
/  static short bstop    [SYSBLOCK];     ptrs to end of each bstart

/  struct text_template  text  = {SYSTEXT,  0, textbuf};
/  struct line_template  line  = {SYSLINE,  0, lineptrs};
/  struct block_template block = {SYSBLOCK, 0, 0, 0,0,0, bstart, bstop};  */


/*** Globals that reference important information about the
/    current conference. */
Chix  confname;      /* Name of conference, : "demo". */
short confnum;       /* Number of conference. */


/*** Temporary display-text building areas. */
/*  char   mtu   [1900];   Removed for chixification.
/   char   format[1000];  */


/*** Information about the user's terminal, since many utility
/    functions must interact with the user. */
struct  screen_template  screen;  /* size & # lines used on screen so far. */
struct  screen_template  used;
struct  termstate_t      term_original,   /* Terminal state before Caucus. */
                         term_in_caucus;  /* Terminal state during Caucus. */


/*** NULL's for all occasions to make all Lints happy. */
union null_t null;


/*** Path to Caucus directory.  Used by many low-level utility functions
/    to generate file names, directory access, etc. */
Chix  confid;  /* ID of conference owner, : "/usr/caucus"            */


/*** Information about the current user.  Mostly the utility functions
/    should *not* know about the current user, but many cheat--and,
/    for example, get the number of the dictionary to use from the
/    current user's default dictionary.  (Eventually, we may just pass
/    in the dictionary number as an argument.) */
Userreg thisuser;    /*** This replaces the next line. */
/*  struct  userreg_t     thisuser;      User participation information */


/*** Command line argument flags.  Affects some low-level utility function
/    operation, such as flag.japan (string functions) and flag.fullscreen
/    (sysclrscn). */
struct flag_template flag;


Chix *hostnames;   /* Array of hostnames listed as original hosts for items. */
int   numhosts = 0;

FUNCTION  utilglobals()
{
   static short first_call = 1;
/* short i;  */
   Textbuf make_textbuf();
   Userreg make_userreg();
   char   *sysmem();

   ENTRY ("utilglobals", "");

   /*** On first call, initialize several things:
   /      in-memory partfile data structures (block)  */
   if (first_call) {
      first_call = 0;

      tbuf     = make_textbuf(TB_RESPS, TB_LINES, "utilglob tbuf");
      thisuser = make_userreg();
      hostnames = (Chix *) sysmem ( L((sizeof (Chix))) * (MAXORIGHOST + 1), "hosts");

/*    for (i=0;   i<SYSBLOCK;   ++i)  block.orighost [i] = nullchix;  */

      null.str = NULL;
   }

   RETURN (1);
}
