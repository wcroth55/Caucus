#
#---filesafefile.i    Link to a filesafe file.
#
#   Invoked as:
#      $includecml (filesafefile.i islink filesafe section filename)
#
#   Arguments:
#      inc(1)  1=>show clickable link, 0=>just show url
#      inc(2)  filesafe name
#      inc(3)  section
#      inc(4)  filename
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 10/03/05 C5 changes.
#-----------------------------------------------------------------------

if $gt_equal ($group_access ($userid() Filesafe $lower($inc(2))) $priv(minimum))
   for section in $list_site_data (0 - FS_$lower($inc(2))_)
      if $equal ($lower($replace(32 _ $site_data (0 - $(section)))) $lower($inc(3)))
         if $inc(1)
            "<a href="$(http)://$http_lib()/FILESAFE/$(section)/$inc(4)"
            "   target="_blank">\
                   $upper1($replace(_ 32 $lower($inc(2))/$inc(3)))/$inc(4)</a>
         end
         else
            "$(http)://$http_lib()/FILESAFE/$(section)/$inc(4)
         end
         quit
      end
   end
end

if $inc(1)
   "<i>(no match)</i>
end
else
   "#
end
