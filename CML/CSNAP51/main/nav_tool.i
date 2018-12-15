#
#---NAV_TOOL.I     Toolbar images
#
#   inc(1)   4 digit conference number (or 0000 for none)
#
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#: CR 12/04/02 Add -p option, do not build/display personal info.
#: CR  7/11/04 Allow turning off individual toolbar buttons.
#----------------------------------------------------------

"<table cellspacing=0 cellpadding=0 border=0>
"<tr valign=top>

if 0$site_data (0 - if_$(iface)_logo_shown)
   "<td><a href="http://www.caucus.com" target="_caucus"
   "><img $(src_logo)></a></td>
end

set openbutton <td><div style="position: relative;"
for x in $site_data (0 - if_$(iface)_buttons_shown)

   #---Skip some buttons
   if $tablefind ($(x) status assign filesafe)
      continue
   end

   if $equal ($(x) goto)
      "$(openbutton)
      "><a href="#" onClick="pop_up('goto', 'A_Title');  return false;"
   end
   elif $equal ($(x) search)
      "$(openbutton)
      "><A HREF="#" onClick="pop_up('search');  return false;"
   end
   elif $equal ($(x) people)
      "$(openbutton)
      if $(do_people)
         "><A HREF="../C$inc(1)/members.htm"
      end
      else
         "><IMG $(src_people) BORDER=0
      end
   end
   elif $equal ($(x) notebook)
      "$(openbutton)
      "><A HREF="#" onClick="pop_up('notebook');  return false;"
   end
   elif $equal ($(x) help)
      "$(openbutton)
      "><A HREF="#" onClick="pop_up('help');  return false;"
   end
   elif $equal ($(x) quit)
      "$(openbutton)
      "><A HREF="../C0000/index.htm"
   end
   else
      continue
   end

   "><img src="$(icon)/tb_$(x)_$(tbstyle).$(icon_type)" border=0
   if $site_data (0 - if_$(iface)_showbuttontext)
      "><span class="button_label"
      ">$site_data (0 - if_$(iface)_buttontext_$(x))</span
   end
   "></a
   if $(is_explorer5)
      "><br><img $(src_clear) width=1 height=7
   end
   "></div></td>
end

"</table>
#-------------------------------------------------------------------------
