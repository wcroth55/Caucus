#
#---virus_warning.i    Warn user about virus detected by virus_check.i
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#:CR 10/12/04 New file.
#----------------------------------------------------------------------

if $not_empty ($(virus_file))
   "<table border=1>
   "<tr><td>
   "<span class="virus">Virus Warning</span>
   "<p/>
   "Your file <b>$(virus_file)</b> appears to be infected,
   "and has not been uploaded.&nbsp;
   "Please use a virus checker to examine your computer.
   "</td>
   "</table>
   "<p/>
end
