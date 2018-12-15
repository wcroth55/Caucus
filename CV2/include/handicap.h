
/*** Include file HANDICAP.H for Caucus source code.  Defines
/    easy-to-read keywords.  Use only for stuff *anybody* might use. */
 
/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: JV  9/18/89 15:08 Officically added this file. */
/*: CR 10/06/89 12:37 Adjust header, FIRE_AFM interpret() call. */
/*: CR 11/13/89 16:46 FIRE_AFM now returns value of interpret(). */
/*: JV  2/23/90 05:00 Added ON and OFF. */
/*: CR  4/09/90 16:04 Added ECHO_INPUT, COOK_INPUT, EIGHT_BIT. */
/*: CR  6/21/90 11:37 Move LSYS, LUSR, LCNF in from units.h. */
/*: CR  7/02/90 12:44 EIGHT_BIT got removed somehow, put back. */
/*: CR  9/11/90 15:26 Add MAXPASS. */
/*: CJ 11/22/90 20:55 Move ECHO, COOK, EIGHTBIT macros to caucus.h. */
/*: CR  2/28/91 22:14 Add EXIT_... codes. */
/*: CR  3/29/91 15:45 Add AND/OR codes. */
/*: JV  2/21/90 20:15 Alter the way HANDI is handled, change docs. */
/*: JV  4/05/91 19:18 Add TRUE, FALSE. */
/*: CR  4/06/91 17:04 Integrate recent changes. */
/*: JV  5/04/91 19:34 Add ENTRY/EXIT stuff here. */
/*: JV  6/14/91 14:38 Add LHST, LSHT, LRCV for Net. */
/*: JV  6/17/91 11:41 Add conf customize attribute macros. */
/*: CR  7/30/91 18:09 Add NULLSITE. */
/*: JV 11/06/91 10:46 Add ENTRY/EXIT include file. */
/*: UL 12/11/91 18:34 Add L(x) to expand X to long. */
/*: JV  1/14/92 14:36 Move NEWPAGE from caucus.h to here. */
/*: DE  4/13/92 13:33 Recast NULLSITE to Chix */
/*: CX  6/05/92 13:47 Add SYSTYPE_... codes. */

#ifndef  HANDI                   /* #endif at end of file */
#define  HANDI             1     /* Marks "This file has been included." */

#include "entryex.h"

#define  FUNCTION
#define  NOT               !
#define  UNTIL(x)          while(!(x))
#define  NULLSTR(s)        (!(s[0]))
#define  EMPTY             -1
#define  ON                1
#define  OFF               0
#define  TRUE              1
#define  FALSE             0
#define  NULLSITE          ((Chix *) NULL)
#ifndef  L
#define  L(x)              ( (int4) x )
#endif

/*** AND/OR codes for use by function matchnames(). */
#define  AND           1
#define  OR            0

#define NEWPAGE    "\001"

/*** System type codes returned by systype(). */
#define SYSTYPE_OTHER   0
#define SYSTYPE_VMCMS   1

#endif             /* HANDI */
