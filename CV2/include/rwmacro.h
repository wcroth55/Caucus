
/*** RWMACRO.H -- macros for _READ, _WRITE, _LSEEK, _CLOSE. */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  6/13/97 15:35 New file. */

#ifndef RWMACRO_H
#define RWMACRO_H

#if UNIX | NUT40
#define  _OPEN3(a,b,c)  open(a,b,c)
#define  _OPEN(a,b)     open(a,b)
#define  _READ(a,b,c)   read(a,b,c)
#define  _LSEEK(a,b,c)  lseek(a,b,c)
#define  _WRITE(a,b,c)  write(a,b,c)
#define  _CLOSE(a)      close(a)
#endif

#if WNT40
#define  _OPEN3(a,b,c) _open(a,b,c)
#define  _OPEN(a,b)    _open(a,b)
#define  _READ(a,b,c)  _read(a,b,c)
#define  _LSEEK(a,b,c) _lseek(a,b,c)
#define  _WRITE(a,b,c) _write(a,b,c)
#define  _CLOSE(a)     _close(a)
#endif

#endif
