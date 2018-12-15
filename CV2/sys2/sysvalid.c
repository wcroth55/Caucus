
/***  SYSVALID.  Validate a CMI mail address for this operating system.
/
/     Checks the mail address in ADDRESS.
/     This code is pretty lenient, ie, it only looks for offending
/     addresses.  It does not attempt to restrict addresses to a 
/     particular grammar. It does not attempt to strictly enforce security.
/     */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  1/03/90 10:23 Created function. */
/*: NT  4/08/97 16:00 Revamp for Caucus 3.2. */

#include "caucus.h"
#include "systype.h"

FUNCTION sysvalid (address)
Chix address;
{

   ENTRY ("sysvalid","");

#if UNIX
   chxsimplify (address);
   switch ((int) chxvalue(address,0L)) {
      case '>':
      case '<':
      case '&':
      case ';':
      case '(':
      case ')':
      case '|':
      case '\\':
      case '\'':
      case '"':
      case '^':   RETURN (0);

      default:    break;
   }
#endif

   RETURN (1);
}
