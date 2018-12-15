
/*** MAXPARMS.H.   Definitions of maximum sizes of various
/    things.  */
 
/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/01/91 21:16 New file. */
/*: CR  9/13/91 12:17 Expand PDIRMAX from 300 to 600. */
/*: CR 10/22/91 23:00 Add MAXULIST. */
/*: JV 10/13/92 16:04 Increase MAXRESP, MAXITEMS, PDIRMAX. */
/*: JV 10/19/92 11:16 Increase PDIRMAX to 4000. */
/*: JV 11/17/92 17:09 Add MAXORIGHOST. */
/*: CT  5/28/93 11:37 Add MAXLNUM. */
/*: CR 10/07/01 15:00 Raise userid length. */
/*: CR 01/30/05 Raise userid length to 200. */

#ifndef MAXPARMS_H
#define MAXPARMS_H

#define  MAXPASS   20     /* Maximum size of "captive" password. */

/*** Maximum number of items, messages, responses. */
#define MAXMESS  1000
#define MAXRESP  9999
#define MAXLNUM  9999

/*** Maximum number of original hosts in global HOSTNAMES array. */
#define MAXORIGHOST 100

/*** Maximum string length expected from files, terminals, etc. ***/
#define  MAXLEN   200

/*** Size of the standard, temporary, dynamically-allocated string buffer.*/
#define TMPTEXTSIZE  1010

/*** Size of MDBUFFER. ***/
#define  MDBUFSIZE   1024

/*** Maximum sizes of various textbuffer things */
#define  BIGSHORT 32767  /* Largest SHORT INT, used as infinity */

/*** Maximum size of system ids (for systems that use hostname or nodename. */
#define  MAXSYSID   16

/*** Max sizes of various things. */
#define  MAXITEMS 9950        /* Max # of items/conference. */
#define  PDIRMAX  4000        /* Max # of partfiles returned by syspdir(). */
#define  MEMBUF   2000        /* Max size of server/client transfer buffers. */

/*** Max number of elements in a userlist. */
#define  MAXULIST  201

/*** Maximum length of a userid. */
#define  MAX_USERID   200

#endif
