
/*** caucus5.h    Include file for new, specific-to-Caucus-5 stuff. */

/*** Copyright (C) 2006 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/06/06 New file. */

/*** Error return codes for $resp_add(). */
#define RESP_ADD_NOCNUM    -1
#define RESP_ADD_NOITEM    -2
#define RESP_ADD_NOWRITE   -3
#define RESP_ADD_FAILED    -5
#define RESP_ADD_NORNUM    -6
#define RESP_ADD_NOLAST    -7

/*** Error return codes for $item_add(). */
#define ITEM_ADD_NOCNUM   -10
#define ITEM_ADD_IPARENT  -11
#define ITEM_ADD_RPARENT  -12
#define ITEM_ADD_INSERT   -13
#define ITEM_ADD_NEWITEM  -14
#define ITEM_ADD_GETITEM  -15
#define ITEM_ADD_TOODEEP  -16
#define ITEM_ADD_NOMAX    -17
#define ITEM_ADD_CONFHAS  -18
#define ITEM_ADD_NOPERM   -19
#define ITEM_ADD_AUTOINC  -20

/*** Error return codes for $resp_del(). */
#define RESP_DEL_NOCNUM   -30
#define RESP_DEL_NOITEM   -31
#define RESP_DEL_NORESP   -32
#define RESP_DEL_NOWRITE  -33
#define RESP_DEL_FAILED   -34

/*** Error return codes for $item_del(). */
#define ITEM_DEL_NOCNUM   -40
#define ITEM_DEL_NOITEM   -41
#define ITEM_DEL_NORESP   -42
#define ITEM_DEL_NOWRITE  -43
#define ITEM_DEL_FAILED   -44

/*** Error return codes for $resp_set(). */
#define RESP_SET_NORNUM   -50
#define RESP_SET_NOSET    -51

#define ITEM_ID_NOCNUM    -60
#define ITEM_ID_NOITEM    -61
