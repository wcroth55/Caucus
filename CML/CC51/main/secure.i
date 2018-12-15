#
#---SECURE.I      Page access check
#
#   Verifies that the current user has access to the current page, 
#   bounces them out if not.
#
#   Input:
#
#     $inc(1)      $inc(2)-$inc(5)  action
#    ----------  -----------        ----------
#      open        [none]           no access check required
#      user       [userid]          current user must be [userid]
#     member       [cnum]           current user must be a member of [cnum]
#      org         [cnum]           current user must be an org of [cnum]
#      ins         [cnum]           current user must be an instructor of [cnum]
#     manage       [groups]         current user must be in listed mgr groups
#     quizedit     qkey             current user must have org priv on quiz
#     quiz         qkey             current user must have inc priv on quiz
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR  4/13/04 Use $co_priv() for checking %access() priv levels.
#              Add 'ins' level.
#: CR  8/24/05 Change to use manager groups.
#: CR 10/28/08 Use new $priv() function for privilege levels.
#--------------------------------------------------------------------------
#

set access 0

if   $equal($inc(1) open)
   set access 1
end

elif $equal($inc(1) user)
   set access $equal($userid() $inc(2))
end

elif $equal($inc(1) member)
   set access $gt_equal ($group_access ($userid() CONF $inc(2)) $priv(minimum))
end

elif $equal($inc(1) org)
   set access $gt_equal ($group_access ($userid() CONF $inc(2)) $priv(organizer))
end

elif $equal($inc(1) ins)
   set access $gt_equal ($group_access ($userid() CONF $inc(2)) $priv(instructor))
end

elif $equal($inc(1) manage)
   set access $mgr_in_group ($userid() $inc(2) $inc(3) $inc(4) $inc(5) \
                                       systemAllPrivileges)
end

elif $equal($inc(1) quizedit)
   set access $gt_equal ($group_access ($userid() Quiz $inc(2)) $priv(organizer))
end

elif $equal($inc(1) quiz)
   set access $gt_equal ($group_access ($userid() Quiz $inc(2)) $priv(include))
end

else
  include $(main)/head.i 0 $quote(Bad Page Access Code)

  include $(main)/body.i x onUnload="pop_close('all');"

  "<H2>Page access failed -- bad security parameter.</H2>
  "The page you requested could not be accessed because the page security
  "has been set incorrectly.  Please contact your system administration
  "with this error and the current URL.
  
  quit
end

if $not($(access))
   "%location($(mainurl)/$page_return ($arg(2) # center.cml?$(nch)+0+x+x+x+x+x+x))
   quit
end
