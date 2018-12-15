#
#--- requiredinfo.i   Make sure user has filled-in all required info fields.
#
#   Purpose:
#      The manager may check "insist on required fields filled-in" for
#      personal info (switch in modify registratin interface).
#
#      If this switch is on, and the user has *not* filled in some required
#      fields, we must take them directly to the "edit personal info" page
#      (me.cml) no matter *where* they entered Caucus.#
#
#      This is accomplished by including this file (requiredinfo.i)
#      in every interface page (except me.cml).
#
#      Note that requiredinfo only checks the required fields *once*
#      per session per interface, and caches the "all required fields filled-in"
#      indicator in the variable '_requiredinfo'.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR  3/28/04 New file.
#: CR 12/19/05 Check site var forceRequired_interface.
#--------------------------------------------------------------------------

#---Have we checked required personal info for this interface yet?
if $not_equal (x$(iface) x$(_requiredinfo))

   #---If we insist on all required fields...
   if 0$site_data (0 - forceRequired_$(iface))

      #---Then scan all required fields.
      include $(main)/requiredcheck.i
   
      #---If there's an error message, at least one field is not filled in.
      #   Take user directly to "edit personal info" page.
      if $not_empty ($(regerrs))
         set nch $unique()
         "%location($(mainurl)/me.cml?$(nch)+$(nxt)+$arg(3)+$arg(4)+$arg(5)+\
                                            $arg(6)+$arg(7)+$arg(8)+$userid())
         quit
      end
   end

   set _requiredinfo $(iface)
end
#--------------------------------------------------------------------------
