/*** SYS.H.   Include file for Caucus SYS functions. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  4/09/90 16:01 New file. */
/*: CJ 11/22/90 20:53 New form for sysio_t. */
/*: JV  3/18/93 11:42 Add SYS_WAIT, SYS_NOWAIT. */
/*: JV  5/03/93 15:15 Move termstate_t from caucus.h to here. */

#ifndef SYS_H
#define SYS_H

/*** Terminal state control block. */
struct termstate_t {
   short   o_echo;
   short   c_echo;
   short   cooked;
   short   eightbit;
   short   lineedit;
   short   is_afile;
};

struct sysio_t {
   struct  termstate_t  state;
   short   c_intr;     /* Interrupt key */
   short   c_erase;    /* Backspace key */
   short   c_eof;      /* End-of-file key */
};

#define SYS_WAIT   1
#define SYS_NOWAIT 2

#endif
