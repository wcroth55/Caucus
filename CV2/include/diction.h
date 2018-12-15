/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** DICTION.H.   Definitions needed for Caucus dictionary access. */

/*: CR  4/20/92 15:25 New file. */
/*: JX  7/13/92 11:00 Add LANG_USER. */
/*: CR 12/22/92 16:22 Add expansion members to mdrec_t. */

#ifndef DICTION_H
#define DICTION_H

#define  DICNAMELEN  23

struct mdrec_t {
   int4   start;
   short  status;
   short  length;
   short  width;
   short  expand1;
   int4   expand2;
   char   name[DICNAMELEN+1];
};
#define  HASHSIZE  4507
#define  MDRES        1
#define  MDTABLE      2

#define  LANG_NORM    1    /* Leave bytes exactly as read in. */
#define  LANG_ISO     2    /* ISO 8859, for future work. */
#define  LANG_EUC     3    /* Japanese EUC  format, 2 bytes/char. */
#define  LANG_SJIS    4    /* Japanese SJIS format, 2 bytes/char. */
#define  LANG_USER    5    /* Prompt user for format. */

#define  LANG_STRING "ascii iso8859 euc sjis user"

#endif
