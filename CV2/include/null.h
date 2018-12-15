/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** NULL.H    Definition of structure that holds nulls. */

/*: JV 11/06/91 10:21 Create this. */
/*: JX  2/22/92 16:00 Add dummy null.chx for now. */
/*: CX  2/24/92 16:48 Chixify. */

#ifndef NULL_H
#define NULL_H

/*** NULL defintions to make Lint happy. */
union null_t {
   char              *str;
   Chix              *md;
   struct namlist_t  *nl;
   struct partic_t   *par;
   Chix               chx;
};

#endif
