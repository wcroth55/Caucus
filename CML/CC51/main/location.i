#
#---LOCATION.I      Location Bar.
#
#   Assumes we are "inside" a table.
#
#   Input:
#
#     $inc(1-9)      Current page title
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 04/16/00 11:28 Display "New Item" link if arg(4) is -1.
#: CR 10/18/02 %displayed_conf_name() calculates displayable conference name.
#: CR  3/14/03 add show_item_title
#: CR 12/22/03 Use item_title_width instead of show_item_title.
#: CR 04/11/04 Allow for long page titles encoded with ^B.
#---------------------------------------------------------------------

#---Vertical drop: space before compass bar, and vertical spacing of text
#   within compass bar.
set dropbar   6
set dropline 26


#---Simple case: at the Caucus Center.
if $and ($equal ($arg(3) x) $empty($inc(1)) )
   "<TABLE width="100%" CELLSPACING=0 CELLPADDING=0 border=0>
   "<TR><TD ALIGN=left>
   "   <TABLE CELLSPACING=0 CELLPADDING=0>
   "   <TR><TD><IMG $(src_compass)>&nbsp;</TD>
   "<TD><B>%nooplink($(center_name))</B></TD>
   "</TABLE>
   "<TD ALIGN=right VALIGN="MIDDLE">
   include $(main)/youare.i
   "</TD>
   "</TABLE>
end

#---Anywhere outside "Caucus Center", the caucus center must be clickable.
else
  
   "<TABLE CELLSPACING=0 CELLPADDING=0 width="100%" border=0>
   "<TR><TD ALIGN=left>
   "<TABLE CELLSPACING=0 CELLPADDING=0>
   "<TR><TD><IMG $(src_compass)>&nbsp;</TD>
   "<TD>
   "<NOBR>
   "<A HREF="center.cml?$(nch)+$(nxt)+x+x+x+x+x+x" %help(h_location)>
   "$(center_name)</A>
  
   #---At a conference (or deeper)...
   if $equal ($arg(3) x)
      "</NOBR>
   end
   else
      "$(compass_arrow)
  
      #---Below a conf home page, make conf home page clickable.
      set cname %displayed_conf_name ($arg(3))
      if $and ($equal ($arg(4) x) $empty ($inc(1)) )
         "%nooplink($(cname))</NOBR>
      end
      else
         "<A HREF="confhome.cml?\
             $(nch)+$(nxt)+$arg(3)+x+x+x+x+x" %help(h_location)>\
             $(cname)</A></NOBR>
      end
  
      #---At an item or deeper, put up item title.
      if $not_equal ($arg(4) x)

         #---For each level...
         set itemLabel $item_label($arg(3) $arg(4))
         set itemParts $replace (. 32 $(itemLabel))
         set priorParts $words (1 $minus($sizeof($(itemParts)) 1) $(itemParts))
         set subLabel
         for part in $(priorParts)
            set subLabel $(subLabel)$(part)
            "$(compass_arrow)
            "<a href="$(mainurl)/viewitem.cml?$(nch)+$(nxt)+$arg(3)+\
               $item_id($arg(3) $(subLabel))+x+x+x+x">Item $(subLabel)</a>
            set subLabel $(subLabel).
         end

         #---Figure last response number
         set lrnum $arg(5)
         if $equal ($(lrnum) -1)
            set lrnum %item_lastresp($arg(4))
         end

         #---Are we at a response > 0 ?
         set at_resp $and ($not_equal ($(lrnum) x)  $not_equal ($(lrnum) 0) )

         #---If we're at a response or a function page, make the
         #   item number an active link.
         "$(compass_arrow)
         if $equal ($arg(4) -1)
            "%backlink(New Item)
         end
         elif $or ($(at_resp) $not_empty ($inc(1)))

            if $not_equal (x$(location_itemid) x$arg(4))
               set location_qkey   $sql_query_select (SELECT qkey FROM items WHERE id=$arg(4))
               set location_itemid $arg(4)
            end

            if $empty ($(location_qkey))
               "<a href="viewitem.cml?$(nch)+$(nxt)+$arg(3)+$arg(4)+x+x+x+x#here" 
            end
            else
               "<a href="quiz_take.cml?$(nch)+$(nxt)+$arg(3)+$arg(4)+x+x+x+x" 
            end

            if 0$(item_title_width)
            "  %help(h_location)>Item $(itemLabel): 
            "  %trimtext($(item_title_width) %item_title($arg(4)))</a>
            end
            else
            "  %help(h_location)>Item $(itemLabel)</A>
            end
         end
         else
            if 0$(item_title_width)
               "%nooplink(Item $(itemLabel): \
                 %trimtext($(item_title_width) %item_title($arg(4))))
            end
            else
               "%nooplink(Item $(itemLabel))
            end
         end

         #---At a response or deeper, put up response number.
         if $(at_resp)
            "<NOBR>$(compass_arrow)
            if $not_empty ($inc(1))
               "<A HREF="viewitem.cml?$(nch)+\
                 $(nxt)+$arg(3)+$arg(4)+$(lrnum)+$arg(6)+$arg(7)+$arg(8)#here"
               " %help(h_location)>Resp $(lrnum)</A>
            end
            else
               "%nooplink (Resp $(lrnum))
            end
            "</NOBR>
         end
      end
   end
  
   #---Finally, if we're at a function page, display it.
   if $not_empty ($inc(1))
      "$(compass_arrow)
      "%nooplink($replace (02 32 $inc(1) $inc(2) $inc(3) $inc(4) $inc(5) \
                                $inc(6) $inc(7) $inc(8) $inc(9)))
   end
  
   "</TD>
   "</TABLE></TD>
   "<TD>&nbsp;</TD>
   "<TD width="30%" ALIGN=right VALIGN="middle">
   include $(main)/youare.i
   "</TD>
   "</TABLE>
end
#---------------------------------------------------------------------
