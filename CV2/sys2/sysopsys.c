
/*** SYSOPSYS.   Return operating system identification string.
/
/    sysopsys (str);
/
/    Parameters:
/       char *str;    (put operating system string here)
/
/    Purpose:
/       Return a string that identifies this operating system.
/
/    How it works:
/       The first word of STR is either "unix" or "nt".
/       The second, etc. words identify which particular flavor
/       or version of that system.
/
/    Returns: 
/
/    Error Conditions:
/  
/    Known bugs:
/
/    Home:  sys/sysopsys.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 10/20/97 16:42 New function. */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"

FUNCTION  void sysopsys (char *str)
{

#if WNT40
   strcpy (str, "nt 4.0");
#endif

#if AIX41
   strcpy (str, "unix aix 4.1");
#endif

#if BSD21
   strcpy (str, "unix bsd 2.1");
#endif

#if DEC32
   strcpy (str, "unix osf/1 4.0");
#endif

#if FBS22
   strcpy (str, "unix freebsd 2.2");
#endif

#if HPXA9
   strcpy (str, "unix hpux a.9");
#endif

#if IRX53
   strcpy (str, "unix irix 5.3");
#endif

#if LNX12
   strcpy (str, "unix linux 1.2.13");
#endif

#if SOL24
   strcpy (str, "unix solaris 2.3");
#endif

#if SOL25
   strcpy (str, "unix solaris 2.5");
#endif

#if SUN41
   strcpy (str, "unix sunos 4.1");
#endif

   return;
}
