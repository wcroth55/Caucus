/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SRVSTAT.H  --  Definition of server status values.
/
/    These beint4 in their own include file, as they are referenced
/    by both the vc...() functions and the Caucus server itself. */

/*: CR  7/01/91 20:41 New file. */

#ifndef SRVSTAT_H
#define SRVSTAT_H

/*** Defs of srv_stat values, that define what 'state' the
     server is in. */
#define   OK           0
#define   NO_MORE    150  /* No more clients allowed to connect. */
#define   TERMINATE  200  /* Terminate server immediately. */
#define   TERMAFTER  300  /* Terminate after this client disconnects. */

#endif
