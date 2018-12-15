
/*** FILE_DATA.  Is data in file in map?
/
/    ok = file_data (arg);
/   
/    Parameters:
/       Chix       arg;     (function argument string)
/
/    Purpose:
/       Evaluate $file_data (file bytes map_range1 [map_range2...])
/
/    How it works:
/       $file_data scans the first BYTES characters of FILE, and looks
/       to see if they fall within the ranges in MAP_RANGE1, 2, etc.
/       Each MAP_RANGE is either a decimal number, or a pair of
/       decimal numbers separated by a dash (no blanks).
/       BYTES = -1 means the entire file.
/
/    Returns: 1 if the those characters fall within the map ranges,
/             0 otherwise (including no such file, empty file, no MAP_RANGES, 
/               and any other error conditions).
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sweb/filedata.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  1/21/98 23:45 New function. */
/*: CR  8/08/03 Return 0 if file exists but is empty. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

FUNCTION  int file_data (Chix arg)
{
   Chix     word1, word2;
   FILE    *fp;
   char     fname[256], maprange[100];
   int      success, cval, count, c0, c1, mr;
   int4     bytes;
   char     map[256];
   ENTRY ("file_data", "");

   word1 = chxalloc (L(80), THIN, "file_data word1");
   word2 = chxalloc (L(80), THIN, "file_data word2");

   /*** Get the filename. */
   chxtoken (word1, nullchix, 1, arg);
   ascofchx (fname, word1, L(0), L(256));

   /*** Make sure we have BYTES, at least one MAP_RANGE, and a file
   /    that exists. */
   success = 0;
   chxtoken (word1, nullchix, 2, arg);
   chxtoken (word2, nullchix, 3, arg);
   if (chxvalue (word2, L(0)) == 0)         goto done;
   if (NOT chxnum (word1, &bytes))          goto done;
   if ( (fp = fopen (fname, "r")) == NULL)  goto done;

   /*** Clear the MAP. */
   for (count=0;   count < 256;   ++count)  map[count] = 0;

   /*** Decode the MAP_RANGEs into 1's and 0's in MAP. */
   for (mr=3;   chxtoken (word1, nullchix, mr, arg) >= 0;   ++mr) {
      ascofchx (maprange, word1, L(0), L(100));
      if (strindex (maprange, "-") > 0)
         sscanf (maprange, "%d-%d", &c0, &c1);
      else {
         sscanf (maprange, "%d",    &c0);
         c1 = c0;
      }
      if (c0 <   0  ||  c1 <   0)  continue;
      if (c0 > 255)                continue;
      if (c1 > 255)     c1 = 255;

      for (count=c0;   count<=c1;   ++count)  map[count] = 1;
   }

   /*** Read through the BYTES in the file.  Any byte not in the
   /    MAP fails immediately. */
   if (bytes < 0)  bytes = 100000000;  /* -1 => "all bytes" => 100 meg max */
   for (success=0, count=0;   count < bytes;   ++count, success=1) {
      cval = fgetc (fp);
      if (cval < 0)                        break;
      if (NOT map[cval])  { success = 0;   break; }
   }
   fclose (fp);

done:
   chxfree (word1);
   chxfree (word2);
   RETURN  (success);
}
