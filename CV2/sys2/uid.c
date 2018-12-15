/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** uid.c    Test new form of getting username. */

#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>

main() {
   struct passwd *p;

   p = getpwuid (getuid());
   printf ("uid=%d, username='%s'\n", getuid(), p->pw_name);
}

