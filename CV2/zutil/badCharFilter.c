
/*** badCharFilter.  Remove "bad" characters in Caucus text.
/
/    Purpose:
/       Remove "bad" characters, such as MSWord fancy quotes, that 
/       users cut-and-paste into Caucus.  Replace them with their
/       simple ASCII equivalents, where possible.
/
/    Usage:
/       badCharFilter <input >output
/
/       A more typical sequence would be:
/          mysqldump -u site -p caucus_site | badCharFilter >site.sql
/          mysql     -u site -p caucus_site -e "source site.sql"
/
/ CR 09/28/2009 First version.
/--------------------------------------------------------------*/

#include <stdio.h>

main() {
   int c;

   while ( (c = getchar()) >= 0) {
      if      (c == 0223) c = '"';
      else if (c == 0224) c = '"';
      else if (c == 0226) c = '-';
      else if (c == 0222) { putchar ('\\');   c = '\''; }
      else if (c == 0364) c = 'o';
      
      putchar(c);
   }
}
