#
#---LOCATION.I     Show "location bar".
#
#   inc(1)   4-digit conference number
#   inc(2)   item num
#   inc(3)   response number
#
#   Assumes conference has been joined, and that the item exists.
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#---------------------------------------------------------------

#---Vertical drop: space before compass bar, and vertical spacing of text
#   within compass bar.
set dropbar   6
set dropline 26

  
"<TABLE CELLSPACING=0 CELLPADDING=0 WIDTH=100% BORDER=0>
   "<TR>
   "<TD VALIGN=CENTER><IMG $(src_compass) ALIGN=LEFT>
   "<BR><FONT SIZE=-1><NOBR>
   "<A HREF="../C0000/index.htm">$(center_name)</A>
   "</NOBR>
   
   
   #---Below a conf home page, make conf home page clickable.
   "$(compass_arrow)
   "<A HREF="../C$(fcnum)/index.htm">$(cname)</A>
   
   #---At an item or deeper, put up item title.
   "$(compass_arrow)
   "<A HREF="../C$(fcnum)/$(finum)0001.htm">Item $inc(2)</A>
   
   #---At a response or deeper, put up response number.
   if $not_equal ($inc(3) 0)
      "<NOBR>$(compass_arrow) <A HREF="#">
      "Resp $inc(3)</A></NOBR>
   end
     
     
   "</FONT></TD>
   "<TD>&nbsp;</TD>
   "<TD ALIGN=right VALIGN=CENTER>
      include $(dir)/youare.i $userid()
   "</TD>
"</TABLE>
#---------------------------------------------------------------------
