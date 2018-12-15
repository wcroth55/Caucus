#
#---CEN_NOTE.I     List notebook entries on enter page
#
#  $inc(1)   bullet
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#-----------------------------------------------------------------------

set show_cats 0
for category center in $user_data($userid() mark_categories)
   set show_cats $plus($(show_cats) $(center))
end

if $(show_cats)
   "<TR><TD>&nbsp;</TD>

   set $inc(1) $plus ($($inc(1)) 1)
   "<TR VALIGN=top><TD ALIGN=right>$unquote($inc(1))</TD>
   "<TD ><B>Choose a location from your 
   "<A HREF=notebook.cml?$(nch)+$(nxt)+x+x+x+x+x+x %help(h_notebook)>
   "notebook</A>:</B><P></TD>

   "<TR><TD></TD>
   "<TD>
   set category_num 0
   for category center in $user_data($userid() mark_categories)
      set category_num $plus($(category_num) 1)
      if $(center)
         include $(main)/notebook.i $(category) 1 $(category_num)
      end
      "<P/>
   end

"</TD>
end

#-----------------------------------------------------------------------
