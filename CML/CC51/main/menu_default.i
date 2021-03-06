#
#---MENU_DEFAULT.I     Toolbar button images and links
#
#   This is the default "button menu" page.  To provide an
#   alternative button menu, make a copy of this file, and
#   name it menu_something.i (you choose 'something', just
#   be sure to make it unique).  Then edit it appropriately.
#   Your new file will appear in the pull-down list in
#   the Caucus Center layout page.
#
#   Alternately, if you just want to add a button to this
#   page (which is easier as of Apr 2005), see the
#   localmenu.i file.
#
#   Note the odd way the </A tags are broken across lines; this
#   ensures that no unintended white space (including line breaks)
#   will appear between the tags.  (White space within a tag is OK.)
#
#   The only argument to this page is $inc(1), which is the "pagename" --
#   which is only used for generating the text of the helpfile link
#   for the help button.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR  5/16/02 New file, based on old nav_tool.i
#: CR  3/02/03 Show buttons if interface "hide" switch is not on.
#: CR  3/27/04 Add gradebook, assignments buttons.
#: CR  7/11/04 Allow turning off Help, Quit buttons.
#: CR 12/22/04 Add tasks button.
#: CR 03/30/05 Complete rewrite for adjustable buttons.
#: CR 04/04/05 Allow adding a button.
#: CR 10/03/05 C5 changes.
#: CR 11/28/05 Fix assign/status buttons for courses/non-courses.
#: CR 01/16/06 Add quiz button.
#: CR 12/18/06 Link to conf filesafe: remove colons.
#: CR 11/05/07 Remove javascript dependencies
#: CR 09/04/09 Make all href targets explicitly go to mainWindow (in case of frames!)
#-------------------------------------------------------------------------
"<table cellspacing=0 cellpadding=0 border=0>
"<tr valign=top>

if 0$site_data (0 - if_$(iface)_logo_shown)
   "<td><a href="http://www.caucus.com" target="_caucus"
   "><img $(src_logo)></a></td>
end

set openbutton <td><div style="position: relative;"
for x in $site_data (0 - if_$(iface)_buttons_shown)

   #---Skip status & assignment buttons if not in a conference.
   if $and ($tablefind ($(x) status assign) \
            $or ($equal ($arg(3) x) $not (%is_class($arg(3)))))
      continue
   end

   if $equal ($(x) goto)
      "$(openbutton)><a
      if $(use_javascript)
         " href="#" onClick="pop_up('goto', 'A_Title');  return false;"
         "    %help(h_goto)
      end
      else
         " href="$(mainurl)/goto.cml?$arg(3)+$(nxt)+$unique()#top"
         " target="$(mainWindow)"
      end
   end

   elif $equal ($(x) search)
      "$(openbutton)
      "><a href="$(mainurl)/search.cml?$(nch)+$(nxt)+\
           $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)"  %help(h_search)
      "    target="$(mainWindow)"
   end

   elif $equal ($(x) people)
      "$(openbutton)
      "><a HREF="people.cml?$(nch)+$(nxt)+\
            $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)"  %help(h_people)
      "    target="$(mainWindow)"
   end

   elif $equal ($(x) filesafe)
      set filesafeconf 0
      if $not_equal ($arg(3) x)
         set fname $replace(: _ filesafe_$lower($cl_name($arg(3))))
         if $not_empty ($site_data (0 - FN_$(fname)))
            set filesafeconf $gt_equal ($group_access ($userid() Filesafe \
                                                       $(fname)) $priv(minimum))
         end
      end
      "$(openbutton)
      if $(filesafeconf)
         "><a href="$(mainurl)/filesafe.cml?$(nch)+$(nxt)+$arg(3)+$arg(4)+$arg(5)+$arg(6)+\
              $arg(7)+$arg(8)+$(fname)" %help(h_filesafe)
         "    target="$(mainWindow)"
      end
      else
         "><a href="$(mainurl)/myfilesafe.cml?$(nch)+$(nxt)+\
              $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)" %help(h_filesafe)
         "    target="$(mainWindow)"
      end
   end

   elif $equal ($(x) tasks)
      if $(use_javascript)
         "$(openbutton)
         "><a href="$(mainurl)/tasks.cml?$(nch)+$(nxt)+\
               $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)" %help(h_tasks)
         "    target="$(mainWindow)"
      end
      else
         continue
      end
   end

   elif $equal ($(x) quiz)
      continue
   end
#  elif $equal ($(x) quiz)
#     "$(openbutton)
#     "><a href="quiz_my.cml?$(nch)+$(nxt)+\
#           $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)" %help(h_tasks)
#  end

   elif $equal ($(x) status)
      "$(openbutton)
      "><a href="$(mainurl)/grades.cml?$(nch)+$(nxt)+\
           $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)" %help(h_grades)
      "    target="$(mainWindow)"
   end

   elif $equal ($(x) assign)
      "$(openbutton)
      "><a href="$(mainurl)/assign.cml?$(nch)+$(nxt)+$arg(3)+x+x+d+x+x" %help(h_assign)
      "    target="$(mainWindow)"
   end

   elif $equal ($(x) notebook)
      "$(openbutton)
      "><a href="$(mainurl)/notebook.cml?$(nch)+$(nxt)+\
            $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)" %help(h_notebook)
      "    target="$(mainWindow)"
   end

   elif $equal ($(x) help)
      set help_title $replace (32 _ $rest (4 $page_get ($(nxt))))
      set help_title $replace (' ` $(help_title))
      set help_title $replace (" ` $(help_title))
      "$(openbutton)
      if $(use_javascript)
         "><a href="#" onClick="pop_up('help', \
             '$(helpurl)/h_root.cml?x+x+h_$inc(1).cml+$t2esc($(help_title))');  \
              return false;"  %help(h_help)
      end
      else
         "><a href="$(helpurl)/h_root.cml?x+x+h_$inc(1).cml+$t2esc($(help_title))"
         "    target="_blank"
      end
   end
   elif $equal ($(x) quit)
      "$(openbutton)
      "><a href="$(mainurl)/goodbye.cml" %help(h_quit)
      "    target="$(mainWindow)"
   end
   else
      set continue 0
      include $(main)/localmenu.i $arg(3) $arg(4) $arg(5)
      if $(continue)
         continue
      end
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
