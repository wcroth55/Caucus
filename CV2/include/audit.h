/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** AUDIT.H   Data structures for auditing xactions.
/   */

/*: 10/18/91 10:18 Create this. */

#define AUDIT_OUT 1
#define AUDIT_IN  2

typedef struct audit_t {
   char host_mail_in;
   char host_mail_out;
   char host_conf_in;
   char host_conf_out;
   char people_in;
   char people_out;
   char user_conf;
   char user_mail_out;
   char user_mail_in;
   char conf_out;
   char conf_in;
   char xfp_time;
   char xfu_time;
} Audit;
