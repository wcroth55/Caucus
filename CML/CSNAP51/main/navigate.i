#
#---NAVIGATE.I    Show all/prev/first/last/next navigation buttons,
#   as appropriate.
#
#   inc(1)  4-digit conf num
#   inc(2)  4-digit item num
#   inc(3)  page num
#   inc(4)  pages
#
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#----------------------------------------------------

   if $greater ($inc(4) 1)
      "<A HREF="../C$inc(1)/$inc(2)0000.htm"><IMG $(src_expand) BORDER=0></A>
   end
   else
      "<IMG $(src_expand0)>
   end
   
   if $greater ($inc(3) 1)
      set ppnum $mac_expand (%digit4($minus ($inc(3) 1)))
      "<A HREF="../C$inc(1)/$inc(2)$(ppnum).htm"><IMG $(src_prevpg) BORDER=0></A>
   end
   else
      "<IMG $(src_prevpg0)>
   end
   
   if $greater ($inc(3) 1)
      "<A HREF="../C$inc(1)/$inc(2)0001.htm"><IMG $(src_1stpg) BORDER=0></A>
   end
   else
      "<IMG $(src_1stpg0)>
   end
   
   if $less ($inc(3) $inc(4))
      set npnum $mac_expand (%digit4($inc(4)))
      "<A HREF="../C$inc(1)/$inc(2)$(npnum).htm"><IMG $(src_lastpg) BORDER=0></A>
   end
   else
      "<IMG $(src_lastpg0)>
   end
   
   if $less ($inc(3) $inc(4))
      set npnum $mac_expand (%digit4($plus ($inc(3) 1)))
      "<A HREF="../C$inc(1)/$inc(2)$(npnum).htm"><IMG $(src_nextpg) BORDER=0></A>
   end
   else
      "<IMG $(src_nextpg0)>
   end

#-----------------------------------------------------------------------
