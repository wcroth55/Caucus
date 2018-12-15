
/*** TESTRESP.  Test Caucus response partfiles for correct format.
/
/    Invoked as:
/       testresp  in_file   out_file
/
/    Arguments:
/       in_file    name of file containing list of item/resp partfiles
/       out_file   error report file
/
/    Purpose:
/       Testresp scans item/response partfiles for errors in the
/       partfile format.  It produces a log of errors in an output file.
/
/    How it works:
/       The input file *must* contain a list of item/resp partfiles
/       in the current directory, sorted alphabetically.
/
/       It produces the following error reports:
/           1. Can't open partfile.
/           2. Response numbers are not strictly increasing by
/              one every time.
/           3. Response number does not match beginning
/              or ending number implied by partfile name.
/
/    Exit status:
/
/    Error Conditions:
/  
/    Side effects:
/
/    Related functions:
/
/    Operating system dependencies:
/
/    Known bugs:
/       VAX and PRIME code not finished.
/
/    Home:  zutil/testresp.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR  7/14/89 15:35 Write TESTRESP program. */
/*: CR  2/02/93 14:22 Add header, handle 10,000 items. */
/*: CR  2/08/93 15:33 Declare str36toi(). */
/*: CR  2/18/93  1:16 Remove unused 'digit4'. */
/*: CR 10/18/93 13:44 Initialize LAST to shut Lint up. */
/*: CR  4/11/03 Use <errno.h> */

#include <stdio.h>
#include <ctype.h>
#include "int4.h"
#define  NOT    !

/*** Select operating system type. */
#undef   UNIX
#define  UNIX   1
#define  VAX    0
#define  PRIME  0

/*** Define ERRORCODE on bad file open. */
#if UNIX
#include <errno.h>
#define  ERRORCODE   errno
#else
#define  ERRORCODE   0
#endif

main (argc, argv)
   int   argc;
   char *argv[];
{
   FILE *inlist, *rsp, *out, *fopen();
   char  name[200], line[200], prevname[200];
   char  sitem[200], sresp[200];
   char *namptr;
   int   last, this, errs, lineno, item, item0, resp, lnum, i, slash;
   int   len;
   int4  str36toi();

   /*** Check # of arguments. */
   if (argc < 3) {
      printf ("Usage: TESTRESP in_file out_file\n");
      exit   (1);
   }

   /*** Open in_file and out_file. */
   nt_mkfile (argv[2], 0700);
   if ( (inlist = fopen (argv[1], "r")) == NULL  ||
        (out    = fopen (argv[2], "w")) == NULL) {
      printf ("Cannot open files '%s' and '%s'.\n", argv[1], argv[2]);
      exit   (1);
   }

   /*** For each file NAME mentioned in in_file... */
   item = -1;
   last = -1;
   while (fgets (name, 200, inlist) != NULL) {

      /*** Pluck off the '\n' and open the file. */
      len = strlen (name) - 1;
      if (name[len] == '\n')  name[len] = '\0';
      if ( (rsp = fopen (name, "r")) == NULL) {
         fprintf (out, "%s: error: cannot open, errno=%d\n", name, ERRORCODE);
         continue;
      }

      /*** Parse the file name for the implied starting ITEM, RESP,
      /    and LNUM numbers.   Skip any files with LNUM > 0, as they
      /    are the continuation partfiles for a very int4 response. */
      item0 = item;
#if UNIX
      slash = -1;
      for (i=0;   name[i];   ++i)   if (name[i]=='/')  slash=i;
      namptr = name + slash + 1;
#endif
      strcpy (sitem, namptr);      sitem[3] = '\0';
      strcpy (sresp, namptr+3);    sresp[3] = '\0';
      if (isdigit(sitem[0]))  sscanf (sitem, "%d", &item);
      else                    item = str36toi (sitem) - 11960;
      if (isdigit(sresp[0]))  sscanf (sresp, "%d", &resp);
      else                    resp = str36toi (sresp) - 11960;
      sscanf  (namptr+6, "%4d", &lnum);

      if (lnum > 0)  continue;

      /*** Check # of last response in previous file against the
      /    implied number of the first response in this file.  If
      /    they don't match correctly, complain. */
      if (item == item0  &&  last != resp-1) {
         fprintf (out, "%s: line ????, %s: %d\n", prevname,
                       "error: bad number for last resp", last);
      }

      /*** Scan through the lines in the partfile.  Concentrate on the
      /    response numbers; ignore anything that doesn't begin with
      /    a '#'. */
      last = -1;
      errs =  0;
      for (lineno=0;   fgets (line, 200, rsp) != NULL;   ++lineno) {
         if (line[0] != '#')  continue;

         /*** Parse the response number.  Look for possible errors:
         /    (a) 1st response number not same as implied by filename;
         /    (b) response numbers not incrementing by 1 each time.
         /    After 5 errors, complain and skip rest of file.  */
         sscanf (line, "#%d", &this);
         line[15] = '\0';
         if (last < 0  &&  this != resp) {
            fprintf (out, "%s: line %4d, %s: '%s'...\n", name, lineno,
                          "error: bad number for 1st resp", line);
            ++errs;
         }

         if (last >= 0  &&  this != last+1) {
            line[15] = '\0';
            fprintf (out, "%s: line %4d, %s: '%s'...\n", name, lineno,
                          "error: resp # out of sequence", line);
            if (++errs >= 5)  {
               fprintf (out, "%s: too many errors, skipping this file.\n",
                             name);
               break;
            }
         }
         last = this;
      }
      fclose (rsp);
      strcpy (prevname, name);
   }

   fclose (inlist);
   fclose (out);
}
