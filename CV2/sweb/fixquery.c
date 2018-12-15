
/*** FIX_QUERY.  Decode elements in QUERY string.
/
/    fix_query (query);
/   
/    Parameters:
/       char   *query;      (query string to be fixed)
/
/    Purpose:
/       Arguments to CML scripts are passed as a HTTP "query string".
/       The arguments are curiously encoded, and require decoding
/       before they are used.  Fix_query() decodes the contents
/       of QUERY, in place.
/
/    How it works:
/
/    Returns:
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  fixquery.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  3/10/95 17:48 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"

FUNCTION  fix_query (char *query)
{
   int    pos, val;
   char   old[6], new[6];
   ENTRY ("fix_query", "");

   for (pos=0;   query[pos];   ++pos) {

      /*** Work-around to fix IE 3.0 bug parsing "http://.../x.cml?a+b#here". */
      if (query[pos] == '#') { query[pos] = '\0';   break; }

      if (query[pos] == '+')   query[pos] = ' ';

      if (query[pos] == '%') {
         old[0] = '%';
         old[1] = query[pos+1];
         old[2] = query[pos+2];
         old[3] = '\0';
         sscanf (old+1, "%2x", &val);
         new[0] = (char) val;
         new[1] = '\0';
         stralter (query+pos, old, new);
      }
   }

   RETURN (1);
}
