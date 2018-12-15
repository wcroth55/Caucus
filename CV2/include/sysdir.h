/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */

/*** SYSDIR.H.   Definitions of macros and structure templates used by 
/    the directory-reading routine SYSDIR.   For MSDOS based systems
/    only. */

/*: CR 12/16/88 13:29 New include file, only used by IM and PN. */

/*** Definition of a node in the linked list of names returned by SYSDIR. */
struct node_template {
   char   name[13];             /* Name of the file or directory. */
   struct node_template *next;  /* Pointer to next node, or NULL. */
};
