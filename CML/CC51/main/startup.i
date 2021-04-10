#
#---STARTUP.I   Start-up include file.
#
#   This file MUST be included by the first .cml page to be
#   interpreted in a Caucus session.
#
#   Arguments:
#     $inc(1)    filename for current page (if none, no threading!)
#     $inc(2)    additional args (after standard arg(3)-arg(8) for
#                normal invocation of current page)
#
#   Synopsis:
#   =========
#   When a Caucus session is started, the CML interpreter starts
#   with a particular ".cml" page.  The name of this page
#   is part of the URL.
#
#   Caucus may be started on any particular CML page.  This allows
#   static HTML links into a conference, bookmarks, or timed-out
#   sessions to take a user directly where they want to be "inside"
#   Caucus.
#
#   In order to make sure a Caucus session gets started properly,
#   however, every CML page must include startup.i to handle
#   initializing all of the common variables used throughout the
#   Caucus session, as well as deciding if certain special pages
#   (password confirmation, and personal information registration)
#   had to be visited first.
#
#   Usage:
#   ==========
#   Since the most important common variable is "mainurl", the
#   simplest way to use startup.i is:
#
#       if $empty($(mainurl))
#          include $cml_dir()/startup.i  thisfile.cml  [additional args]
#          quit
#       end
#
#   If you need to change any aspects of the interface,
#   see the switch.i file.
#
#   This use of startup.i creates a stack, or thread, of pages to
#   be visited... thus allowing for password confirmation and other
#   pages to be evaluated, before getting to 'thisfile.cml' (in the
#   above example).
#
#   If no stack is desired, use startup.i w/o any arguments.
#   This does all the initialization, but allows the calling
#   page to proceed with its own execution.  WARNING: in this
#   case, the caller must be careful to explicitly use the new value
#   of $(mainurl) in any links or Location directives, as the old base
#   mainurl is most likely wrong!!
#
#   Use "no stack"s with care, mostly in xxxxf.cml files that ONLY
#   do form processing and do not actually generate an html page.
#   (I.e., that do a Location: or %location() to go to another page.)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#---Revision history:
#: CR 04/13/05 C5 code merge.
#: CR 11/21/05 Replace group_update() with group_update_rule().
#: CR 03/13/06 Add top banner capability.
#: CR 06/05/06 Remove SQL table creation to updateDatabase.cml.
#: CR 11/05/07 Remove javascript dependencies.
#: CR 07/11/08 Add "keep my reading private" option.
#: CR 02/23/09 Record user's IP address as event.s1 for 'login' event.
#: CR 08/24/09 Interface-specific showUserid controls "(userid)" display instead of show_id.
#: CR 09/04/09 Make all href targets explicitly go to mainWindow (in case of frames!)
#: CR 09/22/09 grouprules.grkey is now auto-increment.
#: CR 09/24/09 Add top banner center image option.
#: CR 01/23/2020 use Plaintext, not Textbox
#========================================================================

#---IFACE   Interface directory
#
#   In 4.4, an interface is typically a single directory under
#   ~/CML, such as ~/CML/CC44.  Under an interface directory
#   are the "tool" directories, such as "main" and "help" and "reg".

set sweb    $word (1 $replace(/ 32 $request_uri()))
set iface   $str_sub (0 $str_revdex (/ $cml_dir()) $cml_dir())
set main    $(iface)/main
set help    $(iface)/help
set dir     xxxxxxxxxx
set inc     yyyyyyyyyy
set href    zzzzzzzzzz
set helpdir 0000000000
#
#========================================================================

#---Is the user logged in?  If not, load loginbox.i and log the user
#   in first.  It will proceed on to original URL.
if $empty ($http_remote_user())
   include $(main)/switch.i
   include $(main)/loginbox.i
   quit
end

#---Local automatic interface switching, if needed.
include $(main)/local_iface.i

#---Detect type of browser, and $(browser_name).
include $(main)/browser_detection.i


#==============================================================
#
#---SWITCHES: include the default and custom interface switches.

#---Choose regular Caucus upload, or xupload
set uploadPage reg-uploadpop
if $not_empty($config(xupload))
   set uploadPage xup-uploadpop
end

#---me3options: list of options to appear in section III of personal info edit.
set me3options ebox detail sort_all sort_new dates boxwide boxhigh wordwide \
               wordhigh pageresps context lmargin rmargin cssfont csssize \
               bozofilter trailerbar cmlerr thumbnails whativeread

#---Get the most basic switch values (that rarely change)
include $(main)/switch.i

#---Get the default values for the more changeable switches.
include $(main)/defaults.i
include $(main)/icons.i

#---Get all macro definitions.
include $(main)/tablemac.i
include $(main)/macros.i
include $(main)/announcedef.i 0
include $(main)/tmn.i
#include $(main)/assignments.i

#---Get local overrides (should be placed in main.1 directory)
include $(main)/localswitch.i
include $(main)/localdefaults.i
include $(main)/localmacros.i
include $(main)/mods.i
include $(main)/xupload.i

#---Load the values for the changeable switches.
#include $(main)/switch2.i
for x in $list_site_data (0 - SW)
   set   varname  $str_sub (2 1000 $(x))
   set $(varname) $site_data (0 - $(x))
end

set allowhtml 1

#==============================================================
#---can_use_(editor):  do the work here of determining if we can
#   use the new (2004) richtext editor.

set can_use_richtext 0
if $(use_javascript)
   if $(is_netscape5)
      set can_use_richtext 1
   end
   if $and ($(is_explorer55) $not ($(is_macintosh)))
      set can_use_richtext 1
   end
   if $and ($greater ($(mozilla_version) 0)  $less ($(mozilla_version) 1.4))
      set can_use_richtext 0
   end
   if $(is_safari)
      set can_use_richtext 1
   end
end

#==============================================================
#
#---MAINURL, HELPURL:  Full URLs for tool pages in this interface.
#
set mainurl  $(http)://$host()/$(sweb)/swebsock/$pid()/$(main)
set helpurl  $(http)://$host()/$(sweb)/swebsock/$pid()/$(help)

#==============================================================
#
#---Miscellaneous strings used throughout the interface:

#---PAGERESPS: # of responses per page of item
set pageresps $word (1 $user_data ($userid() pageresps) $(default_pageresps))

#---CONTEXT: # of responses that appear in a page *before* a selected response
set context   $word (1 $user_data ($userid() context)   $(default_context))

#---LMARGIN: size of left margin (pixels or %) in item & response display
set lmargin   $word (1 $user_data ($userid() lmargin)   $(default_lmargin))

#---RMARGIN: size of right margin (pixels or %) in item & response display
set rmargin   $word (1 $user_data ($userid() rmargin)   $(default_rmargin))

#---COLS:  width of textbox, in characters
set cols $word (1 $user_data ($userid() boxwide) $(default_cols))

#---ROWS:  height of textbox, in characters
set rows $word (1 $user_data ($userid() boxhigh) $(default_rows))

#---wordwide: width  of word-processor, in pixels
set wordwide $word (1 $user_data ($userid() wordwide) $(default_wordwide))

#---wordhigh: height of word-processor, in pixels
set wordhigh $word (1 $user_data ($userid() wordhigh) $(default_wordhigh))

#---ebox: choice of editor
set ebox $word (1 $user_data ($userid() ebox) (default))
if $equal($(ebox) Textbox)
   set ebox Plaintext
end

#---trailerbar: show toolbar at bottom?
set trailerbar 0$user_data ($userid() trailerbar)

#---If 1st time w/o CSS properties, set defaults.
if $empty ($site_data  (0 - if_$(iface)_css_set))
   eval $set_site_data (0 - if_$(iface)_css_set 1)
   eval $set_site_data (0 - if_$(iface)_css_font Arial, Helvetica, sans-serif)
   eval $set_site_data (0 - if_$(iface)_css_size 80%)
end

#---If 1st time w/o shown/hidden buttons, set defaults.
set buttons_original goto search people status assign filesafe tasks \
                     notebook help quit
if $empty  ($site_data (0 - if_$(iface)_buttons_shown)\
            $site_data (0 - if_$(iface)_buttons_hidden))
   eval $set_site_data (0 - if_$(iface)_buttons_shown $(buttons_original))
   eval $set_site_data (0 - if_$(iface)_logo_shown 1)
   eval $set_site_data (0 - if_$(iface)_tbstyle cc50)
   eval $set_site_data (0 - if_$(iface)_showbuttontext 1)
   for x in $(buttons_original)
      eval $set_site_data (0 - if_$(iface)_buttontext_$(x) $upper1($(x)))
   end
   eval $set_site_data (0 - if_$(iface)_buttontext_goto Go&nbsp;to)
   eval $set_site_data (0 - if_$(iface)_buttontext_notebook Notes)
end

set buttons_shown  $site_data (0 - if_$(iface)_buttons_shown)
set buttons_hidden $site_data (0 - if_$(iface)_buttons_hidden)
set logo_shown     $word (1 $site_data (0 - if_$(iface)_logo_shown) 1)
set tbstyle        $site_data (0 - if_$(iface)_tbstyle)


#---css user & site (interface) properties
set css_font  $site_data (0 - if_$(iface)_css_font)
set css_size  $site_data (0 - if_$(iface)_css_size)
set css_file  $site_data (0 - if_$(iface)_css_file)
set ucss_font $user_data ($userid() cssfont)
set ucss_size $user_data ($userid() csssize)

set leftBox        0$site_data (0 - if_$(iface)_leftBox)
set leftBoxColor    $site_data (0 - if_$(iface)_leftBoxColor)
set leftBoxContent  $site_data (0 - if_$(iface)_leftBoxContent)
set leftBoxText     $site_data (0 - if_$(iface)_leftBoxText)
set leftBoxWidth $word(1 $site_data (0 - if_$(iface)_leftBoxWidth) 20%)

#---Banner (interface) properties
set banner_bg        $site_data (0 - if_$(iface)_banner_bg)
set banner_left      $site_data (0 - if_$(iface)_banner_left)
set banner_leftwidth $site_data (0 - if_$(iface)_banner_leftwidth)
set banner_right     $site_data (0 - if_$(iface)_banner_right)
set banner_center    $site_data (0 - if_$(iface)_banner_center)
set banner_height    $site_data (0 - if_$(iface)_banner_height)

set bozos     $user_data ($userid() bozos)

#---TARGET_WINDOW  Code inside an <A HREF> to pop up a target window
set target_window onClick="win = window.open ('', 'fromcau', \
    'resizable=1,toolbar=1,scrollbars=1,menubar=1,width=500,height=380');" \
    TARGET="fromcau"

#---CML Interface version number
set i_version  226

include $(main)/helpstrings.i

#---"Caucus Center" name
set center_name Caucus Center
if $not_empty ($site_data (0 - if_$(iface)_ccenter))
   set center_name $site_data (0 - if_$(iface)_ccenter)
end

#---FileSafe name
set filesafe_name Filesafe
if $not_empty ($site_data (0 - if_$(iface)_filesafe))
   set filesafe_name $site_data (0 - if_$(iface)_filesafe)
end

#---Tasks name
set tasks_name task

#---Timezone vars, item title width (breadcrumb trail/compass bar)
set timezonehrs  0
set timezonename
if $not_empty ($site_data (0 - if_$(iface)_timezonehrs))
   set timezonehrs $site_data (0 - if_$(iface)_timezonehrs)
end
if $not_empty ($site_data (0 - if_$(iface)_timezonename))
   set timezonename $site_data (0 - if_$(iface)_timezonename)
end
set item_title_width $word (1 $site_data (0 - if_$(iface)_item_title_width) 100)

#---Integration with other web apps: get encrypted password
include $(main)/intg_encpass.i  $userid()  encpass

#---Math symbol definitions:
include $(main)/mathsymvals.i

#---Gradebook (Course status) tasks
set course_task_name entries

set _supports_class 1

#---Need to get mgrGroups out of the below.
#include $(main)/sql_init_grouprules.i

#---Perform any needed table updates, entirely data-driven.
include $(main)/sql_update_tables.i

#---Update name_cascade if needed.
include $(main)/namecascade_build.i

include $(main)/namecascade.i


#==============================================================

#---Make sure this user is in group: interface_group_$(iface).
set igroup Caucus4 interface_group_$(iface)
if $not ($group_access ($userid() $(igroup)))
   set query INSERT INTO grouprules \
      (grkey, owner, name, userid, access)  VALUES \
      (NULL, 'Caucus4', 'interface_group_$(iface)', '$userid()', $priv(include))
   eval %sql($(query))
   eval $group_update_rule ($userid() $quote ($(igroup)))
end

#---Get the conference list of  "personal" and "popular" conferences.
set l_confs $site_data (0 - SWl_confs)

##---Join this user to any autojoin confs s/he's not already a member of.
#for cn in $replace (, 32 $site_data(0 - autojoin))
#   set  cname $cl_name($(cn))
#   if $greater (%access($(cn)) 0)
#      if $not ($it_member($(cn)))
#         eval $it_join ($(cn))
#         eval $set_user_data ($userid() my_confs $user_data ($userid() my_confs) \
#                                                $(cname))
#      end
#   end
#end
#set  m_confs $user_data ($userid() my_confs)

#---Initialize the itemlist counter.
set ilist       0
set ilistmax  200

#---Default value for target_switch (see ilist.i and macros.i)
set target_switch 0

#---Kick the "unique # counter" a few times.
eval $unique() $unique()

#---Check to make sure that the web server supplied an actual
#   authorized userid.
if $empty ($http_remote_user())
   "Location: $(mainurl)/nouserid.cml
   "
   quit
end


#---Now that we know we have a userid, track each user's # of
#   Caucus session starts by browser type.
set btype $replace   (32 _ $(browser_name))
set bstr  $user_data ($userid() _browser_count)
set bptr  $tablefind ($(btype) $(bstr))
if $(bptr)
   set bstr $words (1 $(bptr) $(bstr)) \
            $plus  ($word ($plus ($(bptr) 1) $(bstr)) 1) \
            $rest  ($plus ($(bptr) 2) $(bstr))
end
else
   set bstr $(bstr) $(btype) 1
end
eval $set_user_data ($userid() _browser_count $(bstr))

#---Track how this user authenticated (currently ldap or internal,
#   unknown => internal).
set auth_source $user_data ($userid() auth_source)
if $empty ($(auth_source))
   set  auth_source $pw_source()
   eval $set_user_data ($userid() auth_source $(auth_source))
end

#---Default URC root symbol name (User Registration, Custom)
set urcroot urc
set urtroot urt

#---If there's no default registration page template, store one.
if $empty ($site_data (0 - $(urcroot)1))
   include $(main)/regdef.i $(urcroot)
end

#---These may be irrelevant in C5 -- investigate.
include $(main)/urcbase.i
include $(main)/add_suspend.i

eval $set_user_data    ($userid() last_iface  $(iface))
if $empty  ($user_data ($userid() ur_faviface))
   eval $set_user_data ($userid() ur_faviface $(iface))
end

#---Handle users created by Jenzabar interface
set registeredOn $sql_query_select (SELECT registeredon FROM user_info WHERE userid='$userid()')
if $empty($(registeredOn))
   include $(main)/group_addnewusers.i $userid()
   set query UPDATE user_info SET registeredon=now() WHERE userid='$userid()'
   set h $sql_query_open ($(query))
   eval  $sql_query_close($(h))
end


#---Prepare to start up the interface.
#
#   Create a "thread" of the following pages, in order:
#      (0) Caucus Center
#      (1) current page, with additional args from $inc(2)
#      (2) personal info registration page (if needed -- new user only)
#      (3) password confirmation page (if needed)
#
#   Then do a "Location:" to the last page of the thread.  As the
#   interface unwinds the thread, we will get back to the desired
#   page -- and its caller will always appear to be the Caucus Center.


set arg(2) $page_save (1 center.cml 0+x+x+x+x+x+x # $(center_name))

#---Don't do any further threading if no arguments.
if $empty ($inc(1))
   return
end

set fragment $if ( $equal ($inc(1) viewitem.cml) #here #)
set page1    $page_save (1 $inc(1) $arg(2)+$arg(3)+$arg(4)+$arg(5)+$arg(6)+\
                           $arg(7)+$arg(8)$inc(2) $(fragment) ? )

if $empty ($user_info ($userid() lname))
   set page1 $page_save (1 perinfo.cml $(page1)+x # Personal Information)
end

if $and ($(confirm_password) $pw_can_verify())
   set page1 $page_save (1 passchk.cml $(page1)+x # Password Confirmation)
end

include $(main)/calculateShowUserid.i
include $(main)/setLatLong.i

include $(main)/local_startup.i

set mainWindow cmain$word(1 $replace (/ 32 $pid()))

#---Log login!
set event_cnum 0
set event_inum 0
set time_login $time()
eval $logevent (login $(iface) 0 0 0 0 0 0 x $ip_addr())

#---Full form of logevent here, just for reference
#eval $logevent (login $(iface) [duration size conf item r0 rn object
#                             s1 s2 s3 n1 n2 n3])



#---Start unwinding!
"Status: 303
"Location: $(mainurl)/$page_return ($(page1) # )
"Content-type: text/html
"
"<html>
"<body bgcolor="#FFFFFF">
if $(use_javascript)
   "<script type="text/javascript" language="javascript">
   "   location.href = '$(mainurl)/$page_return ($(page1) # )';
   "</script>
end
"</body>
"</html>

quit
#------------------------END-OF-STARTUP.I----------------------
