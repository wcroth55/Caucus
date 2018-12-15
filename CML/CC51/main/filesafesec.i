#
#---filesafesec.i    Link to a filesafe section.
#
#   Invoked as:
#      $includecml (filesafesec.i islink filesafe section)
#
#   Arguments:
#      inc(1)  1=>show clickable link, 0=>just show url
#      inc(2)  filesafe name
#      inc(3)  section
#      inc(4)  calling script's $arg(3)...$arg(8)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 10/03/05 C5 changes.
#-----------------------------------------------------------------------

#---Is the current user allowed in the group that goes with this filesafe?
if $gt_equal ($group_access ($userid() Filesafe $lower($inc(2))) $priv(minimum))

   #---Find the actual section name that matches.
   for section in $list_site_data (0 - FS_$lower($inc(2))_)
      if $equal ($lower($replace(32 _ $site_data (0 - $(section)))) $lower($inc(3)))

         #---Compute the URL of the filesafe page & target.
         set fsurl  $(mainurl)/filesafe.cml?\
                    $(nch)+$(nxt)+$inc(4)+$lower($inc(2))#$(section)

         #---Show either just <a> part of link (no link text -- that's up
         #   to the caller), or else just URL.
         if $inc(1)
            "<a href="$(fsurl)">
         end
         else
            "$(fsurl)
         end
         quit
      end
   end
end

if $inc(1)
   "<a href="#"><i>(No match!)</i>&nbsp;
end
else
   "#
end
