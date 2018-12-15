#
#---filesafe_del.i   Delete filesafe (if user has proper privs)
#
#   Input: inc(1)  filesafe name
#
#   Output: file_manager (set to 1 if user allowed to delete)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 10/03/05 New file.
#:CR 10/28/08 Use new $priv() function for privilege levels.
#:CR 07/16/09 Sanity test $inc(1).
#------------------------------------------------------------------

if $less ($strlen($inc(1)) 4)
   return
end

#---Calculate permissions: must be either file_manager for this particular
#   filesafe, or else regular manager with proper perms.
set file_manager 0
if $gt_equal ($group_access ($userid() MGR filesafeManage) $priv(readonly))
   set file_manager 1
end
elif $gt_equal ($group_access ($userid() Filesafe $inc(1)) $priv(organizer))
   set file_manager 1
end

if $(file_manager)
   
   #---Delete all sections.
   set fdir    $caucus_path()/public_html/LIB/FILESAFE
   for vn in $list_site_data (0 - FS_$inc(1)_)
      eval $deldir ($(fdir)/$(vn))
      eval $set_site_data (0 - $(vn))
   end
   
   eval $set_site_data (0 - FN_$inc(1))
   eval $set_site_data (0 - FH_$inc(1))

   eval %sql(DELETE FROM grouprules WHERE owner='Filesafe' AND name='$inc(1)')
   eval %sql(DELETE FROM groups     WHERE owner='Filesafe' AND name='$inc(1)')
end

#------------------------------------------------------------------
