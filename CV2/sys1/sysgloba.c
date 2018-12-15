/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SYSGLOBALS.  Declaration of global text buffer structures
/    and other system-dependent global variables. */

/*: AA  7/29/88 17:44 Source code master starting point */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */
/*: CR 10/12/05 Remove mdstr for C5. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "sys.h"

extern struct flag_template flag;

/*** System-dependent i/o control:
/    OS_Echo, Caucus_echo, cooked, 8bit; intr, erase, eof. */
#if UNIX | NUT40 | WNT40
struct sysio_t sysio = { {ON, ON, ON, OFF, ON, OFF}, -1, -1, -1 };
#endif

/* Terminal type for use by File Attachment software */
Chix att_ui_type = nullchix;

/* Where to store temporary copy of a file attachment. */
Chix att_copydir = nullchix;

/*** New user SET options.  Most systems default screensize to 23, VM
/    defaults to 0 because of the weird way VM terminals work. */
Chix newuseropts = nullchix;

FUNCTION  sysglobals()
{
   extern union null_t  null;
   static short first_call = 1;

   ENTRY ("sysglobals", "");

   if (first_call) {
      newuseropts = chxalloc (L(100), THIN, "sysglob newuseropts");
      att_copydir = chxalloc ( L(40), THIN, "sysglob att_copydir");
      att_ui_type = chxalloc ( L(40), THIN, "sysglob att_ui_type");
      first_call  = 0;
   }

#if UNIX | NUT40 | WNT40
   chxcpy (newuseropts, CQ("off 23 79 off caucus 8 12 default on 0 . off "
                           "wordwrap 0 later welcome off off ascii_unix 2 "
                           "kermit"));

#endif

   /*** Set the default for the temporary directory. */
#if UNIX | NUT40 | WNT40
   chxofascii (att_copydir, "/tmp");
#endif

   RETURN (1);
}
