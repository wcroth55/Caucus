/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** DEBUG.H:  This file lists all of the names of the debug levels.
/    Each level is independent of all other levels so that they may be
/    chosen individually.
/
/    Their purposes are:
/
/    DEB_NONE:  No Debugging.
/    DEB_FLOW:  Diagnostics that show what major functional blocks
/               are being performed.
/    DEB_OBJC:  Print out objects as they are retrieved and stored.
/               Examples: people, xactions.
/    DEB_DICT:  Print out dictionary strings as they are retrieved.
/    DEB_LOCK:  Print out file locking/unlocking information.
/    DEB_WAIT:  Display "waiting for lock..." information.
/    DEB_FILE:  Print out file open/close/kill   information.
/    DEB_FUNC:  Print out information every time a function is called
/               or returns.
/    DEB_COMM:  Print command lines, before & after macro substitution.
/    DEB_MENU:  Print input lines read by _callmenu.
/    DEB_PRV:   Print sysprv() calls.
/
/    Use these in the form:
/       if (debug & DEB_LOCK) printf ("Lock information.\n");
/
/    Since the global variable 'debug' is an int, on many machines
/    the highest single value is 2^32 = 4,294,967,296 
/    but on some machines it's 2^16 = 0200 000 = 65536.
/
/    Values above 0x800 are special, program-dependent values,
/    with names DEB_SPCn, where n= {1..4}.
/
/    Some of the most-used debug values, in decimal:
/
/    Some correlations between codes and use:
/       sysudnum: DEB_OBJC
*/

/*: JV  8/13/91 08:10 Create this. */
/*: JV  8/26/91 11:55 Add more docs. */
/*: JV 10/18/91 10:11 Add DEB_SPCn values. */
/*: CR 10/31/91 17:38 Change DEB-SPC to DEB_SPC. */
/*: CP 11/21/93 18:38 Add DEB_COMM, readjust #ing to hex. */
/*: CR 12/10/93 14:52 Add DEB_WAIT. */

#ifndef DEBUG_H
#define DEBUG_H
                                           /* Cumulative decimal value */
#define DEB_NONE        0
#define DEB_LOCK    0x001
#define DEB_FILE    0x002
#define DEB_OBJC    0x004
#define DEB_DICT    0x008
#define DEB_PRV     0x010  /*    16 */
#define DEB_FLOW    0x020  /*    32 */
#define DEB_CHIX    0x040  /*    64 */
#define DEB_FUNC    0x080  /*   128 */
#define DEB_COMM    0x100  /*   256 */
#define DEB_MENU    0x200  /*   512 */
#define DEB_WAIT    0x400  /*  1024 */

#define DEB_SPC1   0x1000  /*  4096 */
#define DEB_SPC2   0x2000  /*  8192 */
#define DEB_SPC3   0x4000  /* 16384 */
#define DEB_SPC4   0x8000  /* 32768 */

#endif
