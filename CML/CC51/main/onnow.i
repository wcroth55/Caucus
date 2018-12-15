#
#--- ONNOW.I - Display a list of users currently logged on
#
# Arguments:
#   inc(1)     current conf number
#
#   inc(2)     quoted list of options:
#                 summary   display number of users line
#                 now       display current date/time
#                 header    display table headers
#                 name      show users' names
#                 userid    show users' userids
#                 active    show date/time "active since"
#                 e-mail    show users' email addresses (as links)
#                 aim       show users' AIM addresses (as links)
#                 center    center table
#                 indent    indent table
#
#              If none of the above options, all options are turned on (except 'center').
#
#                 conf      only show users in current conf.  Implies all in the 1st list.
#                 allIfaces show users across all interfaces, otherwise
#                           only show users in my interface.  Implies all in the 1st list.
#
# Output:
#   users      unsorted list of users (one per word)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: MH 10/16/02 New file.
#: CR 10/16/02 some rewriting
#: CR 12/20/03 Add timezonehrs, timezonename interface variables.
#: CR 12/26/03 Split calculation part off into onnow_calc.i
#: CR 10/25/04 Add 'conf' option.
#: CR 11/22/04 Use %onnow_options(), add handling of aim option.
#: CR 02/28/06 Rewrite to look more like members.cml display.
#: CR 10/22/06 Fixed display (onnow_whois) where # words in last name > 1.
#: CR 08/10/09 Do not display (userid) if global show_id is false.
#: CR 08/24/09 Interface-specific showUserid controls "(userid)" display instead of show_id.
#------------------------------------------------------------------------

if $empty($inc(2))
   set options %onnow_options()
end
elif $equal ($inc(2) conf)
   set options %onnow_options() conf
end
elif $equal ($inc(2) allIfaces)
   set options %onnow_options() allIfaces
end
else
   set options $inc(2)
end

#---onnow_timeout controls when a person is no longer considered "on now".
set onnow_timeout $word (1 $config(onnow_timeout) $timeout())

#---Calculate the list of people on right now!
include $(main)/onnow_calc.i 1

#---Figure their full names.
set users
set inconf    $tablefind (conf $(options))
set allIfaces $tablefind (allIfaces $(options))
set myIface   $user_data ($userid() last_iface)
for uid ucnum in $(onnow_users)
   if $or ($not ($(inconf))  $equal ($(ucnum) $inc(1)))
      if $or ($(allIfaces) $equal ($(myIface) $user_data($(uid) last_iface)))
         set users $(users) $quote ($lower($user_info($(uid) lname)) $(uid))
      end
   end
end

if $tablefind (summary $(options)) 
   "<B>Number of users currently logged
   if $(allIfaces)
      "in:
   end
   else
      "in to $(myIface): 
   end
   "$sizeof($(users))</B>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
end
if $tablefind (now     $(options))
   "<b>It is now: %datetz ($dateof ($time())) $(timezonename)</b>
end

set index $gen_sort (1 $(users))

"<P>
"$ifthen ($tablefind (center $(options)) <center>)
"$ifthen ($tablefind (indent $(options)) <ul>)

"<table cellspacing=0 cellpadding=0 border=0>
set spacer <td>&nbsp;&nbsp;&nbsp;</td>
if $tablefind (header $(options))
   "<TR><td></td>
   "$ifthen($tablefind(name      $(options)) <TD><b>Member</b></TD>$(spacer))
   if $(showUserid)
      "$ifthen($tablefind(userid    $(options)) <TD><b>Userid</b></TD>$(spacer))
   end
   "$ifthen($tablefind(active    $(options)) <TD><b>Active Since</b></TD>)
   "$ifthen($tablefind(e-mail    $(options)) <TD><b>E-mail</b></TD>)
   "$ifthen($tablefind(aim       $(options)) <TD><b>&nbsp;&nbsp;AIM</b></TD>)
   "$ifthen($tablefind(allIfaces $(options)) <TD align="right"><b>&nbsp;&nbsp;Interface</b></TD>)
   "</TR>
end

set count  0
set parity 1
for which_user in $(index)
   set count  $plus($(count) 1)
   set parity $minus(1 $(parity))

   set userstring  $word ($(which_user) $(users))
   set userstring $unquote($(userstring))
   set onnow_whois $word ($sizeof($(userstring)) $(userstring))

   "<TR valign=bottom class="peoplelist$(parity)">
   "<td align=right>&nbsp;$(count).&nbsp;</td>
   "$ifthen ($tablefind (name   $(options)) \
      <TD class="onnow">%personpop($(onnow_whois))</TD><TD></TD> )
   if $(showUserid)
      "$ifthen ($tablefind (userid $(options)) <TD>($(onnow_whois))</TD><TD></TD> )
   end
   "$ifthen ($tablefind (active $(options)) \
      <TD><small>%datetz ($words (1 2 %per_laston($(onnow_whois))))\
          $(timezonename)</small>&nbsp;</TD>)
   if $tablefind (e-mail $(options))
      set email $user_info ($(onnow_whois) email)
      "<td>$ifthen ($not_empty ($(email)) \
                    <a href="mailto:$(email)">$(email)</a>)</td>
   end
   if $tablefind (aim $(options))
      set aim   $user_info ($(onnow_whois) aim)
      "<td>$ifthen ($not_empty ($(aim)) &nbsp;&nbsp;%aim($(aim)))</td>
   end
   if $tablefind (allIfaces $(options))
      "<td align="right">&nbsp;$user_data ($(onnow_whois) last_iface)</td>
   end
   "</TR>
end
"</table>
"$ifthen ($tablefind (center $(options)) </center>)
"$ifthen ($tablefind (indent $(options)) </ul>)

#------------------------------------------------------------------------
