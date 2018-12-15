/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** TESTUID.  Program to test switching between real and effective uid's.
/
/    TESTUID should be permitted 4755.  When run, it gets the effective
/    and real uids, changes the effective uid to the real uid, and then
/    tries to change the effective uid back to the original effective
/    uid.  The output, when run from a userid that doesn't own the program,
/    should look something like:
/
/       eid=66, rid=50
/       eid=50, rid=50
/       eid=66, rid=50
/
/    If the final eid stays "66" (in the example), then the switch back
/    failed.  */

/*** Xenix and System V use setuid();  SunOS (and maybe BSD??) use
/    seteuid(). */

/*: CR  7/18/92 15:25 Carvmark the &*%$! thing! */

#define  SYSV  1
#define  BSD   0

#if SYSV
#define  SETUID   setuid
#endif

#if BSD
#define  SETUID   seteuid
#endif

main (argc, argv)
   int   argc;
   char *argv[];
{
   int   eid, rid;

   eid = geteuid();
   rid = getuid();
   printf ("eid=%d, rid=%d\n", eid, rid);

   SETUID (rid);
   printf ("eid=%d, rid=%d\n", geteuid(), getuid());

   SETUID (eid);
   printf ("eid=%d, rid=%d\n", geteuid(), getuid());
}
