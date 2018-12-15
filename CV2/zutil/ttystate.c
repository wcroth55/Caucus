/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** TTYSTATE.  What the hell state is the terminal in, anyway? */

/*: CR  7/18/92 15:25 Carvmark the &*%$! thing! */

#include <stdio.h>
#include "caucus.h"
#include "systype.h"
#include "sys.h"

#include <termio.h>

FUNCTION  ttystate (str)
   char  *str;
{
   struct termio me;
   int    ok, i;

   ok = (ioctl (0, TCGETA, &me) >= 0);
   printf ("\n%s, OK=%d\n", str, ok);
   printf ("  c_iflag=%06o, c_oflag=%06o, c_cflag=%06o, c_lflag=%06o\n",
              me.c_iflag, me.c_oflag, me.c_cflag, me.c_lflag);
   printf ("  c_line =%06o\n", me.c_line & 0xFF);
   for (i=0;   i<NCC;   ++i)  printf ("%d=%03o  ", i, me.c_cc[i]);
   printf ("\n");
}
