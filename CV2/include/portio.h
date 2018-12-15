/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** PORTIO.H: Definitions of macros to do simple I/O operations
/    in Unix or VMS. */

/*: JV  5/11/93 13:45 Create this. */

#ifndef PORTIO
#define PORTIO

#if UNIX
#define OPEN_ASCII_READ(a)  fopen(a, "r")
#define OPEN_ASCII_WRITE(a) fopen(a, "w")
#define CLOSE_ASCII(fp)     fclose(fp)

#define ASCII_READ(s,l,fp)  fgets(s, l, fp)
#define ASCII_WRITE(s,fp)   fputs(s,fp)

#define OPEN_BIN_READ(a,f,l)    open(a, O_RDONLY)
#define OPEN_BIN_WRITE(a,f,l)   open(a, O_WRONLY | O_CREAT, 0700)
#define CLOSE_BIN(fd)           close(fd)

#define BIN_READ(fd,b,sz)   read(fd,b,(int)sz)
#define BIN_WRITE(fd,b,sz)  write(fd,b,(int)sz)
#endif

#endif
