
/*** ENTRYEX.H.  Entry and Exit macros used for doing our own
/    stack tracing. */
 
/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/01/91 21:25 New file. */
/*: CR  7/29/91 23:49 Add ENTRYCHECK stuff. */
/*: JV  9/08/93 09:57 Add ENTRYLOG. */

/*** ENTRY/EXIT test control.  If 1, entry_test() gets called on entry to
/    every function, and exit_test() on return from every function. */
#ifndef ENTRYEXIT
#define ENTRYEXIT    0
#define ENTRYCHECK   0    /* Use compiler to ensure ENTRY/RETURN pairs. */

/* ENTRYLOG logs entry/exit points to a file, does *not* print to screen. */
#define ENTRYLOG     0

#if ENTRYEXIT & ENTRYCHECK
#define ENTRY(x,y)       int entry_check;  entry_test(x,y)
#define RETURN(z)     {  exit_test();      entry_check;       return(z); }
#endif

#if ENTRYEXIT & (! ENTRYCHECK)
#define ENTRY(x,y)       entry_test(x,y)
#define RETURN(z)     {  exit_test();                         return(z); }
#endif

#if ! ENTRYEXIT
#define ENTRY(x,y)
#define RETURN(z)     {                      return(z); }
#endif

#endif
