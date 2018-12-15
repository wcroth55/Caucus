
/*** useridTimeHasher.   Verify correct hash of userid & time.
/
/    Usage:
/       useridTimeHasher userid+hashcode
/
/    Purpose:
/       Verify that 'hashcode' is a valid MD5 hash of the concatentation of:
/          userid
/          current epoch time in seconds / 10000
/
/       To allow for (slightly) mismatched clocks on different servers,
/       it also tries the current time + 10000 and - 10000 seconds.
/
/    Output:
/       '1' written to stdout on success, else '0'.
/
/    Notes: build with "-lssl".
/
/:CR 07/04/09 New program.
/ */

/*** Copyright (C) 2009 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

#include <stdio.h>
#define  STRMAX 256
#define  FUNCTION
#define  NOT     !
#define  SUCCESS 1
#define  FAILURE 0

FUNCTION main (int argc, char *argv[]) {
   int  delta, plusPos;
   long timeNow;
   char userid[STRMAX], hashExpected[STRMAX], hashInput[STRMAX],
        hashResult[STRMAX], timeDigits[STRMAX];

   if (argc != 2)  exitWith(FAILURE);

   strcpy (userid, argv[1]);
   plusPos = strindex (userid, "+");
   if (plusPos < 0)  exitWith(FAILURE);
   userid[plusPos] = '\0';
   strunescape(userid);
   strcpy (hashExpected, userid + plusPos + 1);
   
   timeNow = time(NULL)/10000;
   for (delta=(-1);   delta<=1;  ++delta) {
      strcpy  (hashInput, userid);
      sprintf (timeDigits, "%ld", timeNow + delta);
      strcat  (hashInput, timeDigits);
      sysmd5  (hashResult, hashInput);
      if (streq (hashExpected, hashResult))  exitWith(SUCCESS);
   }

   exitWith(FAILURE);
}

FUNCTION exitWith (int result) {
   printf ("%d\n", result);
   exit    (1);
}

/*
FUNCTION int sysmd5 (char *result, char *input) {
   char md[16];
   int  i;

   MD5 (input, strlen(input), md);

   for (i=0;   i<16;   ++i)  sprintf (result+i+i, "%02x", md[i]);
   result[32] = '\0';

   return (1);
}
*/

FUNCTION  strindex (s, t)
   char  s[], t[];
{
   register int  i, j, k;

   for (i=0;  s[i] != '\0';  i++) {
      for (j=i, k=0;  t[k] != '\0' && s[j]==t[k];  j++, k++) ;
      if (t[k] == '\0')  return (i);
   }
   return (-1);
}

FUNCTION  streq (s, t)
   char *s, *t;
{
   if (s == NULL  ||  t == NULL)  return (0);

   while (*s == *t)  {
      if (NOT *s)  return (1);
      ++s;
      ++t;
   }
   return (0);
}
