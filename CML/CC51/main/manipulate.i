#
#---Manipulate.i    "Manipulate check responses" button or link
#
#   $inc(1)  "in form"  1 => include file is inside a form
#                       0 => otherwise
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#---------------------------------------------------------------

if $(use_button)
   if $not ($inc(1))
      "<FORM NAME=net2check>
   end
   "<INPUT TYPE=button VALUE="Act on" NAME="manip" 
   "    onClick="pop_up('indv');">
   " checked responses
   "$if ($not($inc(1)) </FORM>)
end
else
   "<table cellspacing=0 cellpadding=0>
   "<tr valign=top>
   "<td><img $(src_bullet)>&nbsp;</td>
   "<td><A HREF="#" onClick="pop_up('indv', 'A Title'); return false;"
       ">Act&nbsp;on&nbsp;checked&nbsp;responses</A>&nbsp;</td>
   "</table>
end

