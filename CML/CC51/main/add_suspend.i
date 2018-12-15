#
#---add_suspend.i    Add "suspend" and "favorite interface" user info fields.
#
#   Called from startup.i, detects if these two new fields have been
#   previously added to the local interface REG definition -- and if
#   not, adds the default versions of them.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 08/23/04 New file.
#:CR 09/02/04 Use %urcregnums() to find first new field.
#:CR 09/07/04 Test for blk == 1, => old site, don't add fields.
#-----------------------------------------------------------------------

#---If we have not added the "suspend" and "favorite interface"
#   options for this interface, do so now.
if $empty ($site_data (0 - REG_$(iface)_suspend))

   #---Find first empty (new) REG slot.  If no such fields, don't
   #   add suspend etc fields, as this is probably a very old Caucus site.
   set blk $plus ($max (%urcregnums()) 1)
   if $equal ($(blk) 1)
      return
   end

   eval $set_site_data (0 - REG_$(iface)/reg_$(blk) \
           0 1 1 _c  20 0 suspend x Suspend email notifications?)
   set blk $plus($(blk) 1)
   eval $set_site_data (0 - REG_$(iface)/reg_$(blk) \
           0 1 1 _t   8 0 faviface x Favorite Interface?)
   eval $set_site_data (0 - REG_$(iface)_suspend 1)
end
