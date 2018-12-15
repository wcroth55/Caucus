
/*** READ_FLINE.   Read a "line" at a time of forms data from the client.
/
/    ok = read_fline (cd, first, con_len, line, linemax, &begin);
/ 
/    Parameters:
/       int   ok;      (success?  0=>end of file)
/       Hose  cd;      (connection descriptor)
/       int   first;   (1 on first call, 0 the rest)
/       int   con_len; (content length of data to be read)
/       char  line[];  (place to put data read)
/       int   linemax; (max size of LINE)
/       int  *begin;   (set to 1 if this text starts a new line, else 0)
/
/    Purpose:
/       Read a "line" of forms data from the client, up to LINEMAX bytes.
/
/    How it works:
/       Sets BEGIN to 1 if this was the start of a new line (0 means this
/       is a continuation of a previous line).
/
/       If a line ends in a "\r\n", that sequence does NOT appear
/       in the data.  Use BEGIN to determine if a "\r\n" should 
/       appear *before* a line of text.
/       
/
/    Returns:  # data bytes on success (# >= 0)
/             -1 if reached the end of data (no more lines)
/
/    Error Conditions:
/ 
/    Known bugs:
/
/    Home:  readflin.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  8/06/96 17:26 New function. */

#include <stdio.h>
#include "chixuse.h"
#include "sweb.h"
#include "diction.h"
#include "systype.h"

#define  BUFMAX     1024
#define  DONE(x)    success = x;  goto done

#if SUN41
typedef int size_t;
#endif

extern FILE *sublog;
extern int   cml_debug;

FUNCTION  read_fline (cd, first, con_len, line, linemax, begin)
   Hose   cd;
   int    first;
   int4   con_len;
   char  *line;
   int    linemax;
   int   *begin;
{
   static char buf[BUFMAX+2];
   static int4 dataread;
   static int  pos, bufsize, is_begin, saw_cr;
   int4        amount;
   int         lp, success, eof;
   char        cval;
   ENTRY ("read_fline", "");


   /*** On first call, initialize the internal state of read_fline(). */
   if (first) {
      dataread = 0;    /* Amount of CON_LEN bytes read so far. */
      pos      = 0;    /* Byte position about to be read in BUF. */
      bufsize  = 0;    /* Number of bytes of data in BUF. */
      is_begin = 0;    /* Does data about-to-be-read start a new line? */
      saw_cr   = 0;    /* 0=norm, 1=saw '\r'. */
   }

  *begin    = is_begin;
   is_begin = 0;

   lp  = 0;
   eof = 1;
   while (1) {

      if (pos >= bufsize) {
         if (dataread >= con_len)                                { DONE(lp); }
         amount = con_len - dataread;
         if (amount > BUFMAX)  amount = BUFMAX;
         bufsize = hose_read (cd, buf, amount, 0);
         if (bufsize <= 0)                                       { DONE(lp);}
         dataread += bufsize;
         pos = 0;
      }

      eof  = 0;
      cval = buf[pos++];
      if (cval == '\n'  &&  saw_cr)   { saw_cr = 0;  is_begin = 1;  DONE(lp); }

      if (saw_cr)  line[lp++] = '\r';
      saw_cr = (cval == '\r');

      if (NOT saw_cr)   line[lp++] = cval;

      if (NOT saw_cr  &&  lp >= linemax - 10)        { DONE(lp); }
      if (                lp >= linemax -  2)        { DONE(lp); }
   }

done:
   line[success] = 0;
   RETURN (eof ? -1 : success);
}



