/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** TOKEN.H.   Definition of Token data type and macros. */

/*: CT  5/27/93 14:05 New file. */
/*: CP  6/13/93 22:34 Add Token.hascolon. */
/*: CK 10/06/93 14:38 Add Token.pos, rearrange members, add comments. */

#ifndef TOKEN_H
#define TOKEN_H

struct Token_t {
   Chix   str;        /* Value of token, if a string. */
   int    type;       /* Type of token. */
   int    num0;       /* 1st value of token, if a number or numeric range. */
   int    num1;       /* 2nd value of token, if a numeric range. */
   int    hascolon;   /* Does this token "have" a following colon? */
   int    pos;        /* Character position in original str of this token. */
};

typedef struct Token_t  Token;

#define  T_END   1000
#define  T_NUM   1001    /* Number or numeric range. */
#define  T_DASH  1002
#define  T_COLON 1003
#define  T_WORD  1004    /* Any kind of keyword. */
#define  T_DATA  1005    /* Data for a named range. */
#define  T_NAME  1006    /* Entire named range. */

#endif
