 
/*** REDIRECT.H.   Definition of Redirect object. */
  
/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
 
/*: CP  8/09/93 15:30 New file. */
 
#ifndef REDIRECT_H 
#define REDIRECT_H 
 
struct Redirect_t {
   int   unit;          /* Caucus unit code number */
   short inout;         /* 0=input, 1=output, 2=append. */
   short locked;        /* 0=not locked, 1=locked. */
   char  fname[200];    /* file name, for files that have names. */
};

typedef struct Redirect_t Redirect;

#endif
