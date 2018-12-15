#
#---POPUP.JS     General pop-up window handler.
#
#   Provides three functions:
#
#   pop_up (type, arg)    creates a pop-up window of 'type' with arguments arg.
#                         Pre-defined types are:
#                              help  goto  indv  person  upld
#
#   pop_close (type)      Closes window 'type'.  pop_close('all') closes all
#                         windows opened by pop_up().
#
#   pop_gone (type)       Notify parent window that window 'type' has been
#                         explicitly closed, usually by code within the
#                         pop-up window.
#
#   pop_reload()          Force parent window to reload itself (Useful when
#                         pop up detects a timed-out server)
# 
#   Depends on several CML variables (usually set in startup.i)
#      href             base HREF for CML pages
#      is_netscape3     Is this netscape 3 (or higher)?
#      is_netscape2     Is this netscape 2?
#      is_explorer3     Is this Internet Explorer 3 (or higher)?
#      help_buttons     Display browser button bar on 'help' windows?
#
#   Note: the following properties are available for the "opt=" part
#   when opening any of these windows.  Typically we use the minimum
#   necessary list of properties, but you may want some of the others
#   for debugging purposes:
#
#   Property          Values           Means
#     toolbar           1, 0 (on, off)   (back,forward buttons, etc)
#     location          1, 0             (current URL field)
#     directories       1, 0             (directory buttons - ugh)
#     status            1, 0             (status line at bottom)
#     menubar           1, 0             (menu bar - file, edit, view etc.)
#     scrollbars        1, 0             (enables scroll bars when needed)
#     resizable         1, 0             (resize windows? note spelling!)
#     width             pixels           (initial width of window)
#     height            pixels           (initial height of window)
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 02/15/00 16:58 i 33 version, better use of window.opener.
#: CR 03/10/0l 22:00 pop_close() checks win.closed first!
#: CR 03/10/0l Embed swebs process id in popped-up window name, make it unique.
#: CR 04/27/04 Add experimental openNearby()  opener
#: CR 10/13/04 Adjust size of redesigned indv window.
#: CR 01/22/06 Force main window to be "cmain" + pid.
#: CR 02/08/06 Get default popup window sizes from site_data.
#: CR 01/17/09 For person pop-up, handle case where there are no context arguments.
#------------------------------------------------------------------------------

 set dbgopt
#set dbgopt  ,toolbar=1
#set dbgopt  ,menubar=1,toolbar=1,location=1,status=1

#---Load the default site popup-window sizes (if any) once per session.
if $not (0$(popup_sizes_loaded))
   for popup in help goto indiv person
      set site_xs_$(popup) $site_data (0 - site_xs_$(popup))
      set site_ys_$(popup) $site_data (0 - site_ys_$(popup))
   end
   set popup_sizes_loaded 1
end

"var xs_help   = %userxs(help 400);
"var ys_help   = %userys(help 280);
"var xs_goto   = %userxs(goto 400);
"var ys_goto   = %userys(goto 200);
"var xs_indv   = %userxs(indv 350);
"var ys_indv   = %userys(indv 430);
"var xs_item   = %userxs(item 350);
"var ys_item   = %userys(item 430);
"var xs_person = %userxs(person 450);
"var ys_person = %userys(person 350);
"var xs_break  = %userxs(break 670);
"var ys_break  = %userys(break 500);
"var xs_wiki   = %userxs(wiki 670);
"var ys_wiki   = %userys(wiki 500);

"var pop = new Object;

"if (window.name == ""  ||  window.name.indexOf("cmain") == 0) {
"   window.name="cmain$word (1 $replace (/ 32 $pid()))"; 
"}

"function pop_up (type, arg) {
"   var url, opt, name;

#   ---If the window exists... 
"   if (pop[type] != null) {

#      ---Can we tell if it's been closed (manually)?
"      if ($or ($(is_netscape3) $(is_explorer4))  
"          &&  pop[type].win.closed)  pop[type].is_open = 0;

#      ---If it's already open (with the same argument), just restore focus to it.
"      if (pop[type].is_open  &&  pop[type].arg == arg) {
"         if ($(is_netscape3) || $(is_explorer4))  pop[type].win.focus();
"         return;
#         ---At one point I took out the 'return', but can't remember why.
#         ---(and leaving it out causes help windows to reload first screen.)
"      }
"   }

#   ---Doesn't exist, create it.
"   else {
"      pop[type] = new Object;
"      pop[type].is_open = 0;
"   }

"   pop[type].arg = arg;

#   ---Determine type of pop-up window.
"   name = "win_" + type + "$word (1 $replace (/ 32 $pid()))";
"   if (type == "help") {
"      url = arg;
"      opt = "toolbar=$(help_buttons)," +
"         "location=0,resizable=1,scrollbars=1,width=" + xs_help + \
                ",height=" + ys_help + "$(dbgopt)";
"   }
"   else if (type == "goto") {
"      url = "$(mainurl)/goto.cml?" + "$arg(3)+$(nxt)+$unique()#top";
"      opt = "resizable=1,width="  + xs_goto + \
                        ",height=" + ys_goto + "$(dbgopt)";
"   }
#   else if (type == "tutor") {
#      url = "$(helpurl)/helpt1.cml?0+0+x+x+x+x+x";
#      opt = "toolbar=1,resizable=1,scrollbars=1,width=500,height=380$(dbgopt)";
#   }
"   else if (type == "indv") {
"      url = "$(mainurl)/indvresp.cml?$unique()+$(vi_cnum)+$(vi_inum)+0#top"
"      opt = "toolbar=$(help_buttons),resizable=1,scrollbars=1,\
              width=" + xs_indv + ",height=" + ys_indv + "$(dbgopt)";
"   }
"   else if (type == "upld") {
"      url = "$(mainurl)/upbox.cml";
"      opt = "toolbar=0,resizable=0,scrollbars=0,width=250,height=150$(dbgopt)";
"   }
"   else if (type == "person") {
"      url = "$(mainurl)/person.cml?$unique()+$arg(2)+$arg(3)+$arg(4)+\
            $arg(5)+$arg(6)+$arg(7)+$arg(8)+" + arg;
       if $empty($arg(4))
"         url = "$(mainurl)/person.cml?$unique()+1+x+x+x+x+x+x+" + arg;
       end
"      opt = "location=0,toolbar=0,resizable=1,scrollbars=1,\
              width=" + xs_person + ",height=" + ys_person + "$(dbgopt)";
"   }
"   else if (type == "break") {
"      url = "$(mainurl)/breakout.cml?$unique()+$arg(2)+$arg(3)+b+\
            $arg(5)+x+" + arg;
"      opt = "location=0,toolbar=0,resizable=1,scrollbars=1,\
              width=" + xs_break + ",height=" + ys_break + "$(dbgopt)";
"   }
"   else if (type == "item") {
"      pop_close("item");
"      url = "$(mainurl)/popupItem.cml?$unique()+" + arg;
"      opt = "location=0,toolbar=0,resizable=1,scrollbars=1,\
              width=" + xs_break + ",height=" + ys_break + "$(dbgopt)";
"   }
"   else if (type == "wiki") {
"      url = "$(mainurl)/wikitext.cml?$unique()+$arg(2)+$arg(3)+$arg(4)+$arg(5)+" 
"               + arg + "+x+x";
"      opt = "location=0,toolbar=0,resizable=1,scrollbars=1,\
              width=" + xs_wiki + ",height=" + ys_wiki + "$(dbgopt)";
"   }
#   else if (type == "info") {
#      url = arg;
#      opt = "toolbar=$(help_buttons)," +
#         "location=1,resizable=1,scrollbars=1,width=400,height=280$(dbgopt)";
#   }
"   else {
"      url = "$(mainurl)/unknown.cml?" + type;
"      opt = "width=200,height=100$(dbgopt)";
"   }

#   ---If the window was already open *and* had the same URL, we're done.
"   if (pop[type].is_open) {
"      if (url == pop[type].win.location.href)  return;
"   }

#   ---Otherwise, open it!
"   else pop[type].win = window.open (url, name, opt);

#   ---For new or changed URL window, set the new URL.
"   if (pop[type].win.location.href != url)
"       pop[type].win.location.href =  url;
"   pop[type].is_open = 1;

#   ---win.opener should be the window that *opens* this pop-up.
#   if ($(is_netscape2) > 0)    pop[type].win.opener = self;
"   if (!pop[type].win.opener)  pop[type].win.opener = self;

#   ---insist this window also gets focus.
"   if ($(is_netscape3) || $(is_explorer4))  pop[type].win.focus();
"   return;
"}


"function pop_close (type) {
"   if (type == "all") {
"      for (wtype in pop) pop_close (wtype);
"   }
"   else {
"      if (pop[type].is_open != null  &&  pop[type].is_open) {
#         ---Explorer can't tell if the help window was closed ("X" button)
#         ---by the user.
#         if      ($(is_explorer3))              pop[type].win.close();
#         else if (pop[type].win.opener != null) pop[type].win.close();
"         if (! pop[type].win.closed  &&
"               pop[type].win.opener != null)      pop[type].win.close();
"         pop[type].is_open = 0;
"      }
"   }
"   return;
"}

"function pop_gone (type) {
"   if (window.pop[type]         != null  &&
"       window.pop[type].is_open != null)
"       window.pop[type].is_open  = 0;
"   return;
"}

"function pop_reload() {
"   var wls = window.location.search;
"   window.location.search = "?1" + wls.substring(1, wls.length-1);
"   return;
"}
#-----------------------------------------------------------------

#---Experimental service function, for opening a window "near"
#   an onClick link.  Typically used like:
#      <a href="...." 
#         onClick="return openNearby('tname', 500, 300, 'resizable=1');"
#         target="tname">
#
"function openNearby (wname, xsize, ysize, opts) {
"   var x0, y0, pos, str;
"   if (window.event != null) {
"      x0  = window.event.screenX - xsize/2;   x0 = (x0 > 0 ? x0 : 0);
"      y0  = window.event.screenY - ysize/2;   y0 = (y0 > 0 ? y0 : 0);
"      pos = ",left=" + x0 + ",top=" + y0;
"   }
"   else {
"      x0  = window.screenX + 100;
"      y0  = window.screenY + 100;
"      pos = ",screenX=" + x0 + ",screenY=" + y0;
"   }
"   str = "width=" + xsize + ",height=" + ysize + pos + "," + opts;
"   w = window.open ("", wname, str);
"   w.focus();
"   return w;
" }
