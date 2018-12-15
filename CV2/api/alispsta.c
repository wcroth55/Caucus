/*** A_LIS_PSTAT.   Get list of pstat's (conferences user belongs to)
/
/    array = a_lis_pstat (userid);
/
/    Parameters:
/       int *array;   (returned, allocated & filled list of cnums)
/       Chix userid;  (user in question)
/
/    Purpose:
/       Return a list of conference numbers (cnums) that userid belongs
/       to (more precisely, has an XUPA "pstat" file for).
/
/    How it works:
/       Scans the directory USERnnn/userid for pNNNNxxx, allocates an array
/       and fills it with the existing NNNN's.  Note that this is heavily
/       dependent on the filename format for XUPA as defined by unitname.c.
/
/       The array of cnums is terminated by a cnum of 0.  The caller should
/       free the array when finished with it.
/
/    Returns: array on success.  If there are no conferences for that
/       person, the array is empty (has a single element of 0).
/            
/    Error Conditions:
/
/    Related functions:
/       unitname.c
/
/    Home:  api/alispsta.c
/ */

/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*: CR 05/02/04 New function. */

#include <stdio.h>
#include "caucus.h"
#include "chixuse.h"
#include "api.h"
#include "unitwipe.h"
#include "done.h"

extern Chix confid;

FUNCTION  int *a_lis_pstat (Chix userid) {
   int    start, error, cnums, udnum, i;
   int   *result;
   char   dname[256], fname[256], cidstr[256];
   short  cnum[10000];

   udnum = sysudnum (userid, 0, XUPA+2000);
   if (udnum <= 0) {
      result = (int *) sysmem (sizeof(int) * 4, "a_lis_pstat");
      result[0] = 0;
      return (result);
   }
   ascofchx (cidstr, confid, L(0), L(200));
   sprintf  (dname, "%s/USER%03d/%s", cidstr, udnum, ascquick(userid));

   cnums = 0;
   for (start=1;   sysgfdir (dname, fname, start, &error) == 1;  start = 0) {
      if (fname[0] != 'p'       ||
          ! isdigit (fname[1])  ||  ! isdigit(fname[2])  ||
          ! isdigit (fname[3])  ||  ! isdigit(fname[4]))     continue;
      fname[5] = '\0';
      cnum[cnums++] = atoi (fname+1);
   }

   result = (int *) sysmem (sizeof(int) * (cnums+5), "result");
   for (i=0;   i<cnums;   ++i)  result[i] = cnum[i];
   result[i] = 0;

   return (result);
}
