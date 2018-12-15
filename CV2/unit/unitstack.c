/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

unitstack()
{
   char temp[4096];
   extern char cstack[200][16];
   int    i;

   sprintf (temp, "stack: ");
   for (i=0;   (1);   ++i) {
      if (! cstack[i][0]) break;
      strcat (temp, cstack[i]);
      strcat (temp, "\n    ");
   }
   strcat (temp, "\n");
   buglist (temp);
}
