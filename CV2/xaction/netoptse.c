/*** Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
/    This program is published under the terms of the GPL; see
/    the file license.txt in the same directory.  */
/*** NET_OPTSET.  Set Net_flag options.
/
/ */

/*: JV 10/22/91 16:00 Create net_optset.  */
/*: JV 10/30/91 11:52 Changed name of variable audit to audit_flag. */
/*: CR  1/10/92 21:20 Remove global onofftable. */
/*: CR  1/13/92 17:06 Add onoff arg to net_optset(). */
/*: DE  6/04/92 16:24 Chixified */

#include "caucus.h"
#include "handicap.h"
#include "audit.h"
#include "null.h"

extern Audit   audit_flag;
extern union   null_t      null;
extern int     debug;

FUNCTION  net_optset (option, value, onoff)
   int    option;
   Chix   value, onoff;
{
   int    on;
   int4   num;

   on = (tablematch (onoff, value) == 0 ? 1 : 0);
   if (NOT chxnum (value, &num))  num = on;

   switch (option) {
      case ( 0):   audit_flag.host_mail_in   = on;     break;
      case ( 1):   audit_flag.host_mail_out  = on;     break;
      case ( 2):   audit_flag.host_conf_in   = on;     break;
      case ( 3):   audit_flag.host_conf_out  = on;     break;
      case ( 4):   audit_flag.people_in      = on;     break;
      case ( 5):   audit_flag.people_out     = on;     break;
      case ( 6):   audit_flag.user_conf      = on;     break;
      case ( 7):   audit_flag.user_mail_in   = on;     break;
      case ( 8):   audit_flag.user_mail_out  = on;     break;
      case ( 9):   audit_flag.conf_in        = on;     break;
      case (10):   audit_flag.conf_out       = on;     break;
      case (11):   audit_flag.xfp_time       = on;     break;
      case (12):   audit_flag.xfu_time       = on;     break;

      case (-1):
      default:      break;
   }
}

