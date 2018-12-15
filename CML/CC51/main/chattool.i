#
#---chattool.i    includecml definition of %chattool macro.
#
#   The babylon chat tool can be invoked in a wide variety of
#   ways by the %chattool() macro.  This file is run by 
#   the appearance of the macro, and decides what to do -- i.e.
#   does it embed the chat tool right in the page, or create
#   a link that pops it up, etc.
#
#   inc(1)  current conference number
#   inc(2)  room
#   inc(3)  options
#              type=float    tiny pop-up launches applet in own window (default)
#              type=popup    large popup has applet embedded in it)
#              type=embed    main page embeds applet directly)
#              whiteboard=n  1=show whiteboard (default), 0=no whiteboard
#              pageuser=n    1=show 'Page user(s)' button (default), 0=not
#              im=n          1=show 'Instant Messaging' button (default), 0=not
#              help=n        1=show Help button (default), 0=not
#              ignore=n      1=show Ignore users button (default), 0=not
#              fullnames=n   1=use fullnames, 0=use userids (default)
#              backcolor=x   make background color hex value "x" (no leading #)
#              buttoncolor=x make button color hex value "x" (no leading #)
#   inc(4-9) link text
#
#: CR 08/14/03 New file.
#: CR 08/22/03 Add many new options, see above.
#: CR 06/01/04 Call %rtrim() on <a...>text</a> to remove blue underscores.
#: CR 06/09/04 For embedded chat, warn if organizer has turned off applets!
#: CR 01/03/06 Modify for C5.
#------------------------------------------------------------------------

#---Make sure babylon is configured properly!
if $empty ($config(babylon_port))
   "<b><u>Babylon not configured!</u></b>&nbsp;
   return
end

#---Parse chat options
include $(main)/chatoptions.i $inc(3)

#---Determine room name
if $equal ($inc(2) 0)
   set room $cl_name ($inc(1))
end
elif $numeric ($inc(2))
   set room $cl_name ($inc(2))
end
else
   set room $inc(2)
end


set linktext $inc(4) $inc(5) $inc(6) $inc(7) $inc(8) $inc(9)
if $equal ($(chat_type) float)
   "<a href="$(mainurl)/babylon.cml?$unique()+$inc(2)+$inc(1)+$inc(3)" \
              target="babylon" \
              onClick="win = window.open ('', 'babylon', \
                         'width=101,height=101,screenX=' + \
                          self.screenX + ',screenY=' + self.screenY);  \
                       win.blur();   self.focus();   return true;"    \
           >%rtrim($(linktext))</a>
end

elif $equal ($(chat_type) popup)
   "<a href="$(mainurl)/babylon.cml?$unique()+$inc(2)+$inc(1)+$inc(3)" \
              target="babylon" \
              onClick="win = window.open ('', 'babylon', \
                         'width=$plus($(chat_width) 25),\
                          height=$plus($(chat_height) 25)');  \
                         setTimeout ('win.blur();', 8000);  return true;"   \
           >%rtrim($(linktext))</a>
end

elif $equal ($(chat_type) embed)
   #---Determine "username" as seen by Babylon, both for display 
   #   and authentication.
   set username $userid()
   if $(chat_fullnames)
      set username  $user_info($userid() fname) $user_info($userid() lname)
   end
   
   #---Force babylon authentication 
   include $(main)/chat_auth.i  $quote($(username))

   if $and ($greater ($str_index (viewitem.cml $request_uri()) 0) \
            $equal   ($sizeof ($cleanhtml (prohibited <applet></applet>)) 0))
      "<b>Applets have been prohibited by the organizer.</b>
   end
   "<applet archive="babylon.jar" code="babylonApplet.class"
   "        codebase="$(http)://$host()/~$caucus_id()/BABYLON/"
   "        width="$(chat_width)" height="$(chat_height)">
   "     <param name="username"     value="$replace (32 01 $(username))">
   "     <param name="password"     value="$(babylon_passwd)">
   "     <param name="servername"   value="$word (1 $replace(: 32 $host()))">
   "     <param name="portnumber"   value="$config(babylon_port)">
   "     <param name="autoconnect"  value="yes">
   "     <param name="chatroom"     value="$(room)">
   "     <param name="usepasswords" value="yes">
   "     <param name="locksettings" value="yes">
   "     <param name="hidecanvas"   value="$if ($(chat_whiteboard) no yes)">
   "     <param name="embed"        value="yes">
   "     <param name="pageuser"     value="$if ($(chat_pageuser) yes no)">
   "     <param name="im"           value="$if ($(chat_im)       yes no)">
   "     <param name="help"         value="$if ($(chat_help)     yes no)">
   "     <param name="ignore"       value="$if ($(chat_ignore)   yes no)">
   if $not_empty ($(chat_buttoncolor))
      "  <param name="buttoncolor"  value="$(chat_buttoncolor)">
   end
   if $not_empty ($(chat_backcolor))
      "  <param name="backcolor"    value="$(chat_backcolor)">
   end
   "</applet>
end

else
   "<b><u>Chat macro error: no type '$(chat_type)'.</u></b>
end
