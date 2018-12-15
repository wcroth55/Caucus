#
#---REGREPORT.I     Report on a new user registration, via e-mail.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR  5/06/00 15:23 New file.
#: CR 01/28/01 15:27 Changes for interface-based new user reg page.
#: CR 03/05/02 15:50 fixes for new overrideable directory structure.
#: CR 11/12/04 Use %urcregnums() instead of "count... 1000".
#: CR 02/08/06 Major rewrite for C5.
#: CR 03/09/06 Define allFields!
#: CR 02/04/08 Make $xshell() run synchronously, to ensure file isn't deleted
#       too soon!
#==========================================================================

set sendto $site_data (0 - reg_sendto_$(iface))
if $not_empty ($(sendto))

   set fname /tmp/caucus.$caucus_id().regreport.$word(1 $replace(/ 32 $pid()))
   eval $output ($(fname) 644)

   "To: $(sendto)
   "From: $caucus_id()@$host()
   "Subject: $host() $(iface) registration: \
        $user_info ($inc(1) fname) $user_info ($inc(1) lname)

   
   "Userid $inc(1) just registered at $host() $(iface):
   "

   set query SELECT uc.name FROM user_columns uc, user_iface ui \
              WHERE uc.name = ui.name AND iface='$(iface)' \
                AND ui.on_reg > 0
   set allFields $sql_query_select ($(query)) 

   for field in $(allFields)
      "$(field): $user_info ($inc(1) $(field))
   end
   
   eval $output()
   eval $xshell (0 1 1 $config(sendmail) -t <$(fname))
   eval $delfile ($(fname))
end
   
#==========================================================================
