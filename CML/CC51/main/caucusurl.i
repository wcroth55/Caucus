#
#---CAUCUSURL.I   Parse "Caucus URLs" of the forms:
#
#   http://hostname/caucus
#   http://hostname/caucus/@XYZ
#
#   http://hostname/caucus/conference_name
#   http://hostname/caucus/@XYZ/conference_name
#
#   http://hostname/caucus/conference_name/item_num
#   http://hostname/caucus/@XYZ/conference_name/item_num
#           (if item_num = 'new', starts reading all new material)
#
#   http://hostname/caucus/conference_name/item_num/resp_num
#   http://hostname/caucus/@XYZ/conference_name/item_num/resp_num
#
#   The optional "@XYZ" part of the URL means, go to the
#   appropriate page, using interface directory "XYZ".
#
#   Input: $inc(1)
#      contains part of URL after "http://.../caucus".
#
#   Output: variable 'location'
#      contains full URL to go to next
#
#   Notes:
#      1. If XYZ does not exist, generates nasty message on user's browser.
#   
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 07/17/02 New file, based on old caucus.cml.
#: CR 12/29/03 Allow "http://hostname/caucus/conference_name/new" syntax.
#-------------------------------------------------------------------------

#---Replace /'s and :'s with spaces, so we can parse pieces of URL.
set nch  $unique()
set path_info $replace (/ 32 $inc(1))
set path_info $replace (: 32 $(path_info))

#---Look for the "@interface" keyword, remove it from the path, and set
#   the interface.  (Otherwise use $cml_dir() to get the default interface.)
set iface $cml_dir()
if $equal (@  $str_sub (0 1 $(path_info)))
   set iface  $str_sub (1 1000 $word (1 $(path_info)))/main
   set path_info $rest (2 $(path_info))
end

#---Build root of URLs we'll use to enter proper page with proper interface
set url_root $(http)://$host()/$(sweb)/swebsock/$pid()

#---No conference name ==> Caucus Center.
set cname $word (1 $(path_info))
#if $or ($empty ($(cname))  $equal (x$arg(1) x0) )
if $empty ($(cname))
   set location $(url_root)/$(iface)/center.cml?$(nch)+0+x+x+x+x+x+x
   return
end

#---Figure out conference number.  If not accessible, ==> Caucus Center.
set cnum $word (1 $cl_list ($(cname)))
if $empty ($(cnum))
   set location $(url_root)/$(iface)/center.cml?$(nch)+0+x+x+x+x+x+x
   return
end

#---Figure out item number.  If none ==> conference home page.
set inum   $word (2 $(path_info))
if $empty ($(inum))
   set location $(url_root)/$(iface)/confhome.cml?$(nch)+0+$(cnum)+x+x+x+x+x
   return
end

#---If item 'number' is "new", run thru anew.cml to see all new material.
if $equal (x$(inum) xnew)
   set location $(url_root)/$(iface)/anew.cml?$(nch)+0+$(cnum)+x+x+x+x+x
   return
end

#---Figure out response number.  If none, treat it as zero (start of item).
set rnum $word (3 $(path_info))
set rnum $if ($empty($(rnum)) 0 $(rnum))

#---Handle special case of "last" response.
set id $item_id ($(cnum) $(inum))
if $equal($(rnum) last)
   set rnum %item_lastresp($(id))
end

#---...and view the item.
set location $(url_root)/$(iface)/viewitem.cml?\
             $(nch)+0+$(cnum)+$(id)+$(rnum)+x+x+x

#-----------------------------------------------------------------------
