/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** SEQUENCE.H   Include file for the macros for sequence number ops.
*/

/*: JV  9/19/91 09:02 Create this. */
/*: CR 11/22/91 13:16 Raise all sequence # limits to 10000. */
/*: JV 12/16/91 13:47 Raise all sequence # limits to 1,679,616. */

#define   SEQ_TO_MOD    1679616L
#define   SEQ_THRU_MOD  1679616L
#define   SEQ_FROM_MOD  1679616L
#define   SEQ_ACK_MOD   1679616L
#define   SEQ_PARS_MOD  1679616L
#define   SEQ_CHNK_MOD  1679616L

#define   INC(x, y)    (increment (x,  y))
#define   DEC(x, y)    (increment (x-2, y))
#define   LTEQ(a,b,c)  (NOT greater_than (a, b, c))
#define   GT(a,b,c)    (    greater_than (a, b, c))
#define   LT(a,b,c)    (    greater_than (b, a, c))
 
int4 increment();

/* #define   INC(x)     (increment (x,   SEQ_MOD)) */

