 
/*** MODIFIER.H.  Definitions of modifier slots, as used by new_parse(). */ 
  
/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
 
/*: CR  5/12/93 22:49 New file. */ 
/*: CP  6/22/93 16:44 Add MODIFIERS. */
/*: CP  8/03/93 12:34 Add M_SINCE. */
/*: CP  8/09/93  0:28 Add M_ATTACH, M_NOATTACH, M_WAIT. */
/*: CP  9/08/93 16:01 Add NOASK, NOCOMPLAIN. */
/*: CK  9/15/93 13:24 Add M_MULTIPLE. */
 
#ifndef MODIFIER_H 
#define MODIFIER_H 
 
#define M_ITEMS       0 
#define M_RESPONSES   1 
#define M_NEW         2 
#define M_UNSEEN      3 
#define M_NOFORGET    4 
#define M_FORGOTTEN   5 
#define M_FRESH       6 
#define M_FROZEN      7 
#define M_BRIEF       8 
#define M_PASS        9 
#define M_LIFO       10 
#define M_SINCE      11
#define M_ATTACH     12
#define M_NOATTACH   13
#define M_WAIT       14
#define M_NOWAIT     15
#define M_NOASK      16   /* If no instance given, don't ask user for one. */
#define M_NOCOMPLAIN 17   /* If no instances found, don't complain. */
#define M_MULTIPLE   18   /* Allow 'add resp' to multiple items. */

#define MODIFIERS    19   /* Must be 1 greater than last modifier. */

#endif
