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
#: CR 11/05/07 Remove javascript dependencies.
#: CR 09/04/09 Make all href targets explicitly go to mainWindow (in case of frames!)
#-------------------------------------------------------------------------
"<NOBR

#---Caucus Logo
if 0$site_data (0 - if_$(iface)_logo_shown)
   "><A HREF="http://www.caucus.com/" 
   "       TARGET="_caucus"><IMG $(src_logo)></A
end

set nextDivider &nbsp;|&nbsp;
set divider     &nbsp;

for x in $site_data (0 - if_$(iface)_buttons_shown)

   set buttonText $site_data (0 - if_$(iface)_buttontext_$(x))

   #---Skip status & assignment buttons if not in a conference.
   if $and ($tablefind ($(x) status assign) \
            $or ($equal ($arg(3) x) $not (%is_class($arg(3)))))
      continue
   end

   if $equal ($(x) goto)
      ">$(divider)<a 
      if $(use_javascript)
         " href="#" onClick="pop_up('goto', 'A_Title'); return false;"
         "    %help(h_goto)>$(buttonText)</a
      end
      else
         " href="$(mainurl)/goto.cml?$arg(3)+$(nxt)+$unique()#top"
         " target="$(mainWindow)"
         "  >$(buttonText)</a
      end
   end

   if $equal ($(x) search)
      ">$(divider)<A HREF="search.cml?$(nch)+$(nxt)+\
           $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)"
      "    target="$(mainWindow)"
      "    %help(h_search)>$(buttonText)</A
   end

   if $equal ($(x) people)
      ">$(divider)<A HREF="people.cml?$(nch)+$(nxt)+\
           $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)"
      "    target="$(mainWindow)"
      "    %help(h_people)>$(buttonText)</A
   end

   if $equal ($(x) notebook)
      ">$(divider)<A HREF="notebook.cml?$(nch)+$(nxt)+\
            $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)"
      "    target="$(mainWindow)"
      "     %help(h_notebook)>$(buttonText)</A
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
      if $(filesafeconf)
         ">$(divider)<a href="filesafe.cml?$(nch)+$(nxt)+$arg(3)+$arg(4)+\
              $arg(5)+$arg(6)+$arg(7)+$arg(8)+$(fname)" 
         "    target="$(mainWindow)"
         " %help(h_filesafe)>$(buttonText)</a
      end
      else
         ">$(divider)<a href="myfilesafe.cml?$(nch)+$(nxt)+\
              $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)" 
         "    target="$(mainWindow)"
         " %help(h_filesafe)>$(buttonText)</a
      end
   end

   elif $equal ($(x) tasks)
      if $(use_javascript)
         ">$(divider)<a href="tasks.cml?$(nch)+$(nxt)+\
               $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)" 
         "    target="$(mainWindow)"
         " %help(h_tasks)>$(buttonText)</a
      end
      else
         continue
      end
   end

   elif $equal ($(x) quiz)
      continue
   end
#  elif $equal ($(x) quiz)
#     ">$(divider)<a href="quiz_my.cml?$(nch)+$(nxt)+\
#           $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)" 
#     " %help(h_quiz)>$(buttonText)</a
#  end

   elif $equal ($(x) status)
      ">$(divider)<a href="grades.cml?$(nch)+$(nxt)+\
           $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)" 
      "    target="$(mainWindow)"
      " %help(h_grades)>$(buttonText)</a
   end

   elif $equal ($(x) assign)
      ">$(divider)<a href="assign.cml?$(nch)+$(nxt)+$arg(3)+x+x+d+x+x" 
      "    target="$(mainWindow)"
      " %help(h_assign)>$(buttonText)</a
   end

   elif $equal ($(x) help)
      set help_title $replace (32 _ $rest (4 $page_get ($(nxt))))
      set help_title $replace (' ` $(help_title))
      set help_title $replace (" ` $(help_title))
      ">$(divider)<A 
      if $(use_javascript)
         " HREF="#" onClick="pop_up('help', \
             '$(helpurl)/h_root.cml?x+x+h_$inc(1).cml+$t2esc($(help_title))');  \
              return false;"
         "    %help(h_help)>$(buttonText)</A
      end
      else
         " href="$(helpurl)/h_root.cml?x+x+h_$inc(1).cml+$t2esc($(help_title))" 
         "    target="_blank">$(buttonText)</a
      end
   end

   elif $equal ($(x) quit)
      ">$(divider)<A HREF="goodbye.cml" %help(h_quit) target="$(mainWindow)">$(buttonText)</A
   end

   else
      include $(main)/localmenu_text.i $arg(3) $arg(4) $arg(5)
      if $(continue)
         continue
      end
   end

   set divider $(nextDivider)
end

"></NOBR>
#-------------------------------------------------------------------------
