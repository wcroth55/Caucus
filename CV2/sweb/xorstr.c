
/*** XOR_STR.  Exclusive-or an unsigned 4 byte number with a string.
/
/    num = xor_str (num, str);
/   
/    Parameters:
/
/    Purpose:
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Home:  xorstr.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/15/97 17:40 New function. */

#define  FUNCTION
typedef  unsigned int   uint4;    /* 4 byte unsigned integer on this platform */

FUNCTION  uint4 xor_str (uint4 num, char *str)
{
   union { uint4 num;   char str[4]; } map;
   char  *p;
   int    i;

   map.num = 0;
   for (i=0, p=str;   *p  &&  i<4;   ++i, ++p)  map.str[i] = *p;

   return (num ^ map.num);
}
