/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** GOODLINT.H.   Declarations of common functions and other
/    things that make Lint happy.  (I.e., reduce the number
/    of complaints that Lint makes, especially about mismatched
/    return values of functions.) */

/*: CR  7/01/91 23:28 New file. */
/*: JV  7/04/91 17:49 Add exit(). */

#ifndef GOODLINT_H
#define GOODLINT_H

/*** Declarations of common functions to make LINT happy. */
/*long  lseek();*/
char  *strcpy(), *strcat();
void  exit();

#endif
