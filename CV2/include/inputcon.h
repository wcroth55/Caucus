/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** INPUTCON.H.   Definition of constants used by input_control(). */

/*: CR 10/15/92 18:17 New file. */

#ifndef INPUTCON_H
#define INPUTCON_H

#define IC_VERB   0xF0000 /* Verb mask. */
#define IC_GETLIN 0x10000 /* read a line of text. */
#define IC_GETEOF 0x20000 /* read a line, with EOF warning. */
#define IC_RED    0x30000 /* redirect input. */
#define IC_CLOSE  0x40000 /* close, revert to normal unit. */
#define IC_ISUNIT 0x50000 /* report on unit in use. */

#define IC_KIND   0x0F000 /* "Kind" mask. */
#define IC_SESS   0x01000 /* read from unit for entire session.  */
#define IC_DATA   0x02000 /* read from unit for data & commands. */
#define IC_COMM   0x03000 /* read from unit for commands only.   */
#define IC_MENU   0x04000 /* read from unit for menu input only. */

#define IC_UNIT   0x00FFF /* mask to get unit number. */

#endif
