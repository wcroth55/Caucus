#
#---WARNADMIN.I   Is this site running out of user slots?  If so,
#   warn the administrator via e-mail if possible.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#

#---If there is a limit on the max # users on a Unix server...
if $and ($lice_max_users()  $equal ($word (1 $opsys()) unix))

   #---and if the admin hasn't been warned in a day...
   set warned 0$site_data (0 - warn_admin)
   if $greater ($time() $plus ($(warned) 86400))
      eval $set_site_data (0 - warn_admin $time())

      #---and there are less than 20 user slots remaining...
      set remaining $minus ($lice_max_users() $lice_act_users())
      if $empty($(user_limit_trigger))
         set user_limit_trigger 20
      end
      if $less ($(remaining) $(user_limit_trigger))

         #---Run the supplied script that e-mails the Caucus admin
         #   with a warning.
         eval $xshell (0 1 1 $caucus_path()/warnadmin \
                             $(remaining) $admin_mail())
      end
   end
end

#-------------------------------------------------------------------
