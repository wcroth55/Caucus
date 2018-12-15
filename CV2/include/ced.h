/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** CED.H  Include file for Caucus EDitor. */

/*: AA  7/29/88 17:47 Source code master starting point */
/*: JX  3/28/92 09:43 Replace ced_template. */
/*: CR 10/21/92 14:16 Remove commented-out old ced_template. */
/*: CK  9/20/93 16:37 Raise MAXCED to 5000. */

#ifndef CED_H
#define CED_H

#define MAXCED 5000

/*** Template for a Caucus Editor session. */
typedef struct ced_template {
   Chix line[MAXCED];   /* Lines of text. */
   int  size;           /* Current number of lines. */
   int  mark;           /* Line currently being worked on; where the mark is */
} Ced;

#endif
