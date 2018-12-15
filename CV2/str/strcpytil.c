/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** STRCPYTIL.   Copy everything from A to B until char C is found,
/                 not inclusive. */

/*: CR  4/02/02 12:40 New function. */

#define  FUNCTION

FUNCTION  int  strcpytil (char *b, char *a, char c) {
   while (*a  &&  *a != c)  *b++ = *a++;
   *b = '\0';
   return (1);
}
