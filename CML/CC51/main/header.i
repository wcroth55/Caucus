#
#---HEADER.I     Caucus Logo, tool bar, and navigation bar.
#
#   $inc(1)      pagename for help
#   $inc(2-7)    Description of current page title.
#
# Copyright (C) 1995-2007 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#--------------------------------------------------------------------


#-------------First row: Caucus logo and tool bar.-------------------

"<a NAME="top"><img $(src_clear) width=1 height=1></a>
 include $(main)/headtool.i $inc(1)


#-------------Second row: location bar.---------------------------
include $(main)/location.i $inc(2) $inc(3) $inc(4) $inc(5) $inc(6) $inc(7) \
                          $inc(8) $inc(9)

#"<P>

#---Clear field that handles AOL protection (against multiple form
#   submissions), since we actually got to another page.
set last_unique 0

#---Check for disk-write error.
if $not_empty ($disk_failure())
   "<P>
   "<B>Disk-write error encountered: $disk_failure()</B>
   "<P>
   "Press <B>Quit</B>.&nbsp;
   "If this problem continues, please contact your system
   "administrator.
   "<P>
   "<HR>
   "</div>
"</body>
   "</HTML>
   return
end

#---Set page saveability (changed by form-result pages)
set can_save_page 1

#------------------------------------------------------------------------
