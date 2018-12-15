/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** STRTRIM.  Trim any trailing blanks off of STR. */

/*: AA  7/29/88 17:53 Source code master starting point */

strtrim (str)
   char  str[];
{
   int   len;

   for (len=strlen(str);   len>0  &&  str[len-1]==' ';)  --len;
   str[len] = '\0';
   return (1);
}
