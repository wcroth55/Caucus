
/*** urlDecode.   Decode url-encoded string.
/
/    Purpose:
/       URL-encoded strings replace some special characters with "%xx",
/       where xx is the two hexadecimal digit representation of the
/       ASCII value of that character.  urlDecode decodes such encoded strings.
/
/    Usage:
/       urlDecode <input >output
/
/       A more typical sequence would be:
/          echo "encoded string..." | urlDecode
/
/ CR 09/29/2009 First version.
/--------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>

main() {
   int c, h1, h2;

   while ( (c = getchar()) >= 0) {
      if (c == '%') {
         h1 = getchar();
         h2 = getchar();
         if (h1 < 0  ||  h2 < 0)  break;
         c = (16 * valueOfHexDigit(h1) + valueOfHexDigit(h2));
      }
      
      putchar(c);
   }
}

int valueOfHexDigit(int digit) {
   int upper = toupper (digit);
   if (upper >= 'A')  return (upper - 'A' + 10);
   return (upper - '0');
}
