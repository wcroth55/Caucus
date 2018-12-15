/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** COMMON.H.  Definitions of structures common to 
/    client, server and standalone Caucus programs. */

/*: CR 11/16/88  0:17 Initial release of server code. */
/*: CR  3/22/89 12:18 Reduce MAXITEMS to 950 to accomodate DG server. */
/*: CR  4/27/89 15:47 Add lock-consistency check code. */
/*: CR 10/11/89 17:29 Raise HASHSIZE to 4507, mdrec_t.name to DICNAMELEN. */
/*: JV  3/29/91 18:00 Change header to make it accurate, move null_t here. */
/*: JV  5/03/91 14:51 Add detection of redundant includes of this file. */
/*: CR  6/17/91 17:22 master_template: new fields origitem[], orighost[]. */
/*: CR  6/25/91 14:54 Add 'cnum' field to master_template. */
/*: CR  7/01/91 18:36 Split things out to separate include files. */
/*: CR  9/16/91 13:05 Adjust pdir_t definition so we can use heap. */
/*: CX 10/20/91 16:07 Chixify null_t. */
/*: JV 11/06/91 10:21 Move null_t to null.h. */
/*: JV 12/16/91 17:06 Change pdir.lnum to lonng. */
/*: CX  4/20/92 15:12 Remove dictionary stuff to diction.h. */
/*: CI 10/04/92 23:40 Chix Integration. */

#ifndef COMMON
#define COMMON

#include "maxparms.h"
#include "master.h"
#include "partic.h"
#include "namelist.h"
#include "entryex.h"
#include "unitcode.h"
#include "null.h"

/*** Lock requests from the server send a "namecode" that contains
/    the arguments that SRVNAME needs to make up a filename. */
struct namecode_t {
   short item;     /* Item number. */
   short resp;     /* response number. */
   short lnum;     /* line number. */
   short confnum;  /* conference number. */
   short readwrit; /* read/write type of lock. */
   char  word[60]; /* userid or other "word" required in name. */
};

/*** Requests from syspdir->apdir are passed through a pinfo_t. 
/    (Results come back in a pdir_t.) */
struct pinfo_t {
   short item;         /* Item (or message) number. */
   short confnum;      /* Conference number. */
   char  userid[20];   /* owning userid of file if any. */
};

/*** Structure for item and response partfile directory listings. */
struct pdir_t {
   short  type;
   short  parts;
   short *resp;
   int4  *lnum;
};

#endif
