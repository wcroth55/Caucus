# 
#---HEAD.I      Initial HTML for all pages, including the <HEAD> block 
# 
#   $inc(1)     conference number or 0 or x if none
#   $inc(2)     quoted page title  
#   $inc(3)     if not empty and non-zero, name of additional JavaScript file
#   $inc(4)     if not empty, use <BASE HREF="$inc(4)">
#   $inc(5)     if 1, include CB44 editor javascript.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 03/22/00 16:18 Add pop-up confirm box for bulletins.
#: CR 04/20/00 16:48 Include browserwarn.i to check for Win/2000 IE 5 bug.
#: CR 05/10/00 16:27 Comment out include of browserwarn.i for now.
#: CR 02/12/01 09:30 Dynamic tables.
#: CR 08/23/02 Put "Expires" HTTP header lines back in, to force no caching.
#: MH 10/08/02 Added ONNOW code
#: MH 10/16/02 Modified ONNOW code for efficiency, added last_onnow_check
#: CR  3/18/04 Load browser-dependent stylesheet.
#: CR  8/27/04 Add CB44 richtext editor include.
#: CR 10/16/04 Record confnum in ONNOW file.
#: CR 02/19/06 Add favicon.
#: CR 05/08/07 Add blockBackspaceKey.js, stop Backspace from acting like Back.
#----------------------------------------------------------------------------

"Content-type: text/html
"Pragma: no-cache
"Expires: Thu, 01 Dec 1994 16:00:00 GMT
"
"<!-- quirksmode -->
"<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
"<HTML> 
"<HEAD> 
"<TITLE>$unquote($inc(2))</TITLE> 
"<link rel="icon"          href="$(http)://$host()/~$caucus_id()/favicon.ico" 
"     type="image/x-icon" >
"<link rel="shortcut icon" href="$(http)://$host()/~$caucus_id()/favicon.ico" 
"     type="image/x-icon" >


include $(main)/css.i 0 $inc(1)

#---Update this user's "on now" status file every 5 minutes, or
#   whenever the confnum changes.
if $or ($greater ($time() $plus (0$(last_onnow_check) 300)) \
        $not_equal (x$(last_onnow_cnum) x$arg(3)))
   eval $write($caucus_path()/MISC/ONNOW/$userid() \
               $time()$newline()$arg(3)$newline())
   set last_onnow_check $time()
   set last_onnow_cnum  $arg(3)
end

"<script type="text/javascript" language="javascript">
"<!-- Hide script from non-JavaScript browsers...

include $(main)/popup.js  
include $(main)/tablepop.js

if $and ($not_empty($inc(3)) $inc(3))
   include $(main)/$inc(3)
end

#---Test for bulletins every 5 minutes.  Display in a pop-up 'confirm' box
#   if there is one.  See man_bull.cml.
if $greater ($time() $plus (0$(last_bulletin_check) 300))
   set   bulletin $site_data (0 - bulletin)

   #---If there is a bulletin, and it either should be shown every 5 minutes
   #   OR has been changed since the last time we checked... show it.
   if $not_empty ($rest (3 $(bulletin)))
      if $or ($word (2 $(bulletin))  \
              $greater ($word (1 $(bulletin)) 0$(last_bulletin_check)))
         "confirm ("$rest (3 $(bulletin))");
      end
   end
   set last_bulletin_check $time()
end

"//-->
"</SCRIPT>

if $and ($not_empty  ($inc(4))  $not_equal ($inc(4) 0))
   "<BASE HREF="$inc(4)">
end

if 0$inc(5)
   "<script type="text/javascript" language="JavaScript" 
   "        src="$(http)://$host()/~$caucus_id()/CB51/richtext.js"></script>
   "<script type="text/javascript" language="JavaScript" 
   "        src="$(http)://$host()/~$caucus_id()/CB51/html2xhtml.js"></script>
end

#---Block backspace key from doing the annoying "go back one page".
if $not(0$config(AllowBackspaceKey))
   "<script type="text/javascript" src="/~$caucus_id()/blockBackspaceKey.js"
   "></script>
end

"</HEAD>  
