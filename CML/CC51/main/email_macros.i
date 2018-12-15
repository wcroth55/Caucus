#
#---EMAIL_MACROS.I    Define Caucus "macros" for this interface.
#
#   This file contains the default definitions for the Caucus "macros"
#   used throughout the interface.
#
#           * * *   DO NOT CHANGE THIS FILE!!   * * *
#
#   If you wish to change any macro definitions, or add new ones,
#   copy just the macros you need to the file localmacros.i in the
#   override directory main.1 (create the directory and the file if necessary), 
#   and edit or create them there.
#
#: CR  3/20/00 20:31 Add %implemented() macro.
#: CR 03/27/00 20:56 Add bad_email_address() macro.
#: CR  4/04/00 16:16 Add %iframe() macro.
#: CR  4/05/00 15:47 Make %implemented() handle valid empty function returns.
#: CR  4/12/00 17:11 Make %ifs_resp() 2nd arg optional.
#: CR  4/16/00 11:27 Add %backlink() macro.
#: CR  4/20/00 13:16 Change %iframe() def to use default_iframe_width, height
#: CR  5/05/00 16:19 Add %bullet(), %ol().
#: CR  5/07/00 17:41 Add %aim(), dummy %leave(),
#: CR  5/30/00 13:08 Redefine %js_open_win(), %new_target_win().
#: CR  7/15/01 21:35 Add %conf_has_new_br %conf_link_br %conf_name_with_new_br
#: CR  7/17/01 22:25 Add %confname_response_new()
#: CR  8/02/01 10:45 Insert $(http) in libname2, liblink2 defs
#: CR 12/18/01 15:15 Use $(http)://$http_lib() consistently.
#: CR 12/18/01 15:40 Adjust pop-up IE window sizes; add %files()
#: CR 12/20/01 14:25 Add %person_noclick()
#: CR 02/01/02 16:30 Add %strip_http()
#: CR 03/01/02 11:15 Rearrange for use with override directories.
#: CR 03/22/02 16:52 Add %center().
#: CR 09/11/02 Add %login_gme() for Group Mind Express, %ifs_popup().
#: MH 10/10/02 Add %onnow(), %onnow_link()
#: CR 10/18/02 Add %preinc(), %conf_nonmember()
#: CR 11/26/02 Add %liburl_at(), %libimg_at(), make filesafe use FN_$lower().
#: CR 01/27/03 Use conf alias in %displayed_conf_name()
#: CR 04/30/03 Add %filesafesec(), %filesafesecurl().
#: CR 05/14/03 Add optional 'opener' arg to %js_open_win(), add %js_open_sub().
#: CR 05/16/03 include announce.i
#: CR 12/20/03 Add %datetz() to use timezonehrs() for displaying times;
#: CR  4/13/04 Use $co_priv() for checking $cl_access() priv levels.
#              %trimtext().
#: CR 12/26/03 Add %onnow_check(), %onnow_user() to show if a userid is on now.
#: CR 12/30/03 Expand bad_email_address to include "<>()".
#: CR 04/12/04 Add gradebook, assignments, math symbols macros.
#: CR 04/27/04 Add %urlparams(), %listpeople().
#: CR 05/11/04 Add %supports_class(), redefine %is_class() to use it.
#: CR 06/01/04 Add %rtrim().
#: CR 06/11/04 Add %plainOfHTML().
#: CR  8/08/04 Add %epochOfyyyy()
#: CR  8/26/04 Add [options] to %libimg2(), %libimg2r(), libimg_at().
#: CR 08/24/09 Interface-specific showUserid controls "(userid)" display instead of show_id.
#============================================================================

#---Set delta height & width values for NN and IE -- use them to adjust
#   the size of newly opened windows (since the values that both NN and IE
#   return for the current window size is bogus in different ways).
set dh_ie $if ($(is_macintosh)  140    -50)
set dw_ie (-30)
set dh_nn (-140)
set dw_nn (-30)

#===Sub-macros, used by other macros:

#---%js_open_win (name [opener])    Javascript to open a new window
#       if 'opener' is not empty, use opener of current window to
#       determine proper size of new window.
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

eval $mac_define (js_open_sub   win = window.open ('', '@1',\
    'resizable=1,toolbar=1,scrollbars=1,menubar=1,location=1,status=1,' + \
    \$ifthen ($(is_explorer5) 'screenX=0,screenY=0,' + )\
    \$set (ws \$word(1 \$user_var(\$userid() my_win_size) Default))\
    \$ifthen (\$equal (\$(ws) Default) \
    $ifthen ($(use_resize) \
       \$set(which \$if (\$empty(@2) window window.opener))\
          'width='  + $if ($(is_netscape4)  (\$(which).outerWidth+$(dw_nn))  \
                             (\$(which).document.body.clientWidth+$(dw_ie))) + \
         ',height=' + $if ($(is_netscape4) (\$(which).outerHeight+$(dh_nn)) \
                            (\$(which).document.body.clientHeight+$(dh_ie))))\
    $ifthen ($not ($(use_resize)) \
       'width=\$word(1 \$new_win()),height=\$word(2 \$new_win())')\
    )\
    \$ifthen (\$not_equal (\$(ws) Default) \
       'width=\$word (1 \$replace(x 32 \$(ws))),\
        height=\$word(2 \$replace(x 32 \$(ws)))') ); )


#   'resizable=1,toolbar=1,scrollbars=1,menubar=1,location=1,status=1,' + \
#   'resizable=1,scrollbars=1,' + \

eval $mac_define (js_open_ann   win = window.open ('', '@1',\
    'resizable=1,scrollbars=1,' + \
    \$ifthen ($(is_explorer5) 'screenX=0,screenY=0,' + )\
    \$set (ws \$word(1 \$user_var(\$userid() my_win_size) Default))\
    \$ifthen (\$equal (\$(ws) Default) \
    $ifthen ($(use_resize) \
          'width='  + @2 * $if ($(is_netscape4)  (outerWidth+$(dw_nn))  \
                             (document.body.clientWidth+$(dw_ie))) + \
         ',height=' + @3 * $if ($(is_netscape4) (outerHeight+$(dh_nn)) \
                            (document.body.clientHeight+$(dh_ie))))\
    $ifthen ($not ($(use_resize)) \
       'width=\$word(1 \$new_win()),height=\$word(2 \$new_win())')\
    )\
    \$ifthen (\$not_equal (\$(ws) Default) \
       'width=\$word (1 \$replace(x 32 \$(ws))),\
        height=\$word(2 \$replace(x 32 \$(ws)))') ); )

eval $mac_define (js_open_win \%js_open_sub (@@); $(winfocus)  return true; )

#------------------------------------------
#===Simple text manipulation (HTML) macros:

#---%bold (text), %b(text)
eval $mac_define (bold <B>@@</B>)
eval $mac_define (b    <B>@@</B>)

#---%tt (text)
eval $mac_define (tt <TT>@@</TT>)

#---%blockquote (text)
eval $mac_define (blockquote <BLOCKQUOTE>@@</BLOCKQUOTE>)

#---%italic(text), %i(text)
eval $mac_define (italic <I>@@</I>)
eval $mac_define (i      <I>@@</I>)

#---%big(text)
eval $mac_define (big <BIG>@@</BIG>)

#---%small(text)
eval $mac_define (small <SMALL>@@</SMALL>)

#---%sub(text)
eval $mac_define (sub <SUB>@@</SUB>)

#---%sup(text)
eval $mac_define (sup <SUP>@@</SUP>)

#---%strike(text)
eval $mac_define (strike <STRIKE>@@</STRIKE>)

#---%under(text)
eval $mac_define (under <U>@@</U>)
eval $mac_define (u <U>@@</U>)

#---%mono(text)
eval $mac_define (mono <TT>@@</TT>)

#---%color (colorname text)
eval $mac_define (color <FONT COLOR="@1">\$rest(2 @@)</FONT>)

#---%bullet(text)   Indented "bulleted" paragraph.
eval $mac_define (bullet \
        <TABLE CELLSPACING=0 CELLPADDING=0 BORDER=0><TR VALIGN=top> \
        \$ifthen (       \$or(\$(is_explorer4) \$(is_netscape5)) \
             <TD>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD><TD><LI></TD>)\
        \$ifthen (\$not (\$or(\$(is_explorer4) \$(is_netscape5))) \
             <TD VALIGN=top><UL><LI></UL></TD>)\
        <TD WIDTH="100%">@@</TD></TABLE>)

#---%ol(n text)     Indented numbered (N) paragraph.
eval $mac_define (ol \
        <TABLE CELLSPACING=0 CELLPADDING=0 BORDER=0><TR VALIGN=top> \
        <TD ALIGN=right>@1.&nbsp;<BR>\
           <IMG SRC="\$(icon)/clearpix.gif" WIDTH=40 HEIGHT=1></TD>\
        <TD WIDTH="100%">\$rest(2 @@)</TD></TABLE>)

#------------------------------------------
#==="Goto location" macros:

#---%url (actual_url, text of link)
eval $mac_define (url <A HREF="@1" \
           onClick="\%js_open_win(fromcau)" TARGET="fromcau"\
           >\$if (\$empty(@2) @1 \$rest(2 @@))</A>)

#---%urlparams (actual_url, windowparams, text of link)
eval $mac_define (urlparams <a href="@1" \
          onClick="window.open ('', 'urlparams', '@2');   return true;" \
          target="urlparams">\$if (\$empty(@3) @1 \$rest(3 @@))</a>)

#---%response (rnum)
eval $mac_define (response <A HREF="\%_caucus_url(\$(m_cnum) \$(m_inum) @1)">\
                  \$if (@1 Response&nbsp;@1 the item)</A>)

#---%response_new (inum linktext...)
eval $mac_define (response_new \$ifthen (\
          \$gt_equal (\$it_resps(\$(m_cnum) @1) \$it_newr(\$(m_cnum) @1)) \
          <A HREF="\%_caucus_url(\$(m_cnum) @1 \$it_newr())">\$rest (2 @@))</A>)
  
#---%confname_response_new (cname inum linktext...)
eval $mac_define (confname_response_new \
      \$set (_cfrn \$cl_num(@1))\
      \$ifthen (\
         \$gt_equal (\$it_resps(\$(_cfrn) @2) \$it_newr(\$(_cfrn) @2)) \
         <A HREF="\%_caucus_url(\$(_cfrn) @2  \$it_newr())">\$rest (3 @@)</A>))

#---%item (inum [rnum])
eval $mac_define (item <A HREF="\%_caucus_url(\$(m_cnum) @1 @2 0)">\
                   Item @1\$if (\$not_empty(@2) :@2)</A>)

eval $mac_define (itemtitle <A HREF="\%_caucus_url(\$(m_cnum) @1 0)">\
                   \$re_title (\$(m_cnum) @1 0)</A>)

#---%conference (conf_name [inum [rnum]])
eval $mac_define (conference \
                   \$set (cthis \$word (1 \$cl_list(@1)))\
                   \$set (ctnam  \%displayed_conf_name (\$cl_list(@1)))\
                   \$if (\$not (\$cl_access(\$(cthis))) <U>@1</U> \
                      \$ifthen (\$empty(@2) \
                         <A HREF="\%_caucus_url(\$(cthis))">\$(ctnam)</A>)\
                      \$ifthen (\$not_empty(@2) \
                         <A HREF="\%_caucus_url(\$(cthis) @2 @3 0)">\
                         \$(ctnam)&nbsp;Item&nbsp;@2\
                         \$if (\$not_empty(@3) :@3)</A>)\
                   )\
                 )

#---%center(text)    Go to Caucus Center
eval $mac_define (center <a href="\%_caucus_url()">\
          \$ifthen(    \$empty(@1)  \$(center_name))\
          \$ifthen(\$not_empty(@1)  @@)\
     </a>)

#---%readrange (startitem enditem name)
#   Named link to read range of items in current conference.
eval $mac_define (readrange <A HREF="readrange.cml?\$arg(3)+@1+@2">\
                  \$rest(3 @@)</A>)

#---%leave(URL)    Dummy macro, just expands to argument(s).
#   Provided so that display of new user registration "layout" page
#   appears to work like the real thing but doesn't terminate
#   Caucus process.
eval $mac_define (leave @@)


#------------------------------------------
#===File upload macros.  The base versions (liburl, libimg, etc.) are
#   from 4.0 and are retained for backward compatibility for existing
#   responses.  The "2" versions (liburl2, libimg2, etc.) are for 4.1 and
#   assume an "rdir" (item/response sub directory) that is keyed to the
#   particular item and response numbers where the file was uploaded.
#
#---%liburl(userid file)   URL of file in library
#   Build URL out of Caucus LIB directory, conf name, userid (@1) and
#   filename (@2).  Use the following logic to determine conf name
#   (patch to handle responses copied from another conference)
#      if (page is viewitem.cml) {
#         if (response was copied)  use 1st word of $re_copied() as conf name
#         else                      use current conf name
#      }
#      else                         use current conf name
eval $mac_define (liburl $(http)://$http_lib()/\
 \$if (\$greater (\$str_index (viewitem.cml \$page_return (\$(nxt) #)) -1) \
     \$set (ftemp \$word (1 \$re_copied(\$(vi_cnum) \$(vi_inum) \$(response))))\
     \$if (\$empty (\$(ftemp)) \$cl_name(\$(cnum)) \$(ftemp)) \
                               \$cl_name(\$(cnum)))/@1/@2)

eval $mac_define (digit4 \$str_sub(0 \$minus(4 \$strlen(@1)) 000)@1)

#--- %strip_http (url)    strips off leading "http://" if present
eval $mac_define (strip_http \
       \$if (\$equal (\$str_index (http:// @1) 0) \
          \$str_sub (7 2000 @1) \
          @1)\
     )

#---%liburl2 (userid rdir file)   URL of file in library
eval $mac_define (liburl2 $(http)://%strip_http($http_lib()/)\$cl_name(\$(m_cnum))/@1/\
       \$if (@2 \%digit4(\$(m_inum))\%digit4(\$(m_rnum)) 00000000)/@3)


#---%liburl_at (conf item resp file)   URL of file in library in other conf
eval $mac_define (liburl_at \$ifthen (\$word (1 \$cl_list(@1)) \
          $(http)://$http_lib()/@1/\
          \$re_owner(\$cl_num(@1) @2 @3)/\
          \%digit4(@2)\%digit4(@3)/@4))

#---%libimg(userid file)   IMG file in library
eval $mac_define (libimg <IMG SRC="\%liburl(@1 @2)">)

#---%libimg2(userid rdir file [options])   IMG file in library
eval $mac_define (libimg2 <IMG SRC="\%liburl2(@1 @2 @3)" BORDER=0 \
                               \$rest(4 @@)>)

#---%libimg2r(userid rdir file [options])   IMG file in library, right-aligned.
eval $mac_define (libimg2r <IMG SRC="\%liburl2(@1 @2 @3)" BORDER=0 align=right \
                                \$rest(4 @@)>)

#---%libimg_at (conf item resp file [options])
eval $mac_define (libimg_at <IMG SRC="\%liburl_at(@1 @2 @3 @4)" BORDER=0 \
                                \$rest(5 @@)>)

#---%liblink(userid file)   link to file in library
eval $mac_define (liblink <A HREF="\%liburl(@1 @2)" \
                  onClick="\%js_open_win(fromcau)" TARGET="fromcau"\
                  >\%liburl(@1 @2)</A>)

#---%liblink2(userid rdir file)   link to file in library
eval $mac_define (liblink2 \$t2url(\$mac_expand(\%liburl2(@1 @2 @3))))

#---%libname(userid file name)   named link to file in library
eval $mac_define (libname <A HREF="\%liburl(@1 @2)" \
                  onClick="\%js_open_win(fromcau)" TARGET="fromcau"\
                  >\$rest (3 @@)</A>)

#---%libname2(userid rdir file name)   named link to file in library
eval $mac_define (libname2 \
                  \$set (ln \$t2url(\$mac_expand(\%liburl2(@1 @2 @3))))\
                  \$str_sub (0 \$str_index (> \$(ln)) \$(ln))>\$rest(4 @@)</A>)

#------------------------------------------
#===Other misc macros.

#---%person(userid [opt link text])
eval $mac_define (person \
     \$ifthen (\$not_equal($userid() @1) \
                  <A HREF="javascript: pop_up('person', '@1');" \%help(h_name))\
     \$ifthen (\$equal($userid() @1) \
                  <A HREF=\%to_parent(me.cml?\$(nch)+\$(nxt)+\$arg(3)+\
                  \$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8)+@1) \
                  \%help(h_youare))\
     >\$ifthen (\$empty(@2) \$per_name(@1))\
      \$ifthen (\$not_empty(@2) \$unquote(@2))\
      \$if (\$(showUserid) &nbsp;(@1))</A>)

#---%person_noclick (userid [opt link text])
eval $mac_define (person_noclick \
     \$ifthen (\$not_equal($userid() @1) \
                  <A HREF="#" onClick="return false;")\
     \$ifthen (\$equal($userid() @1) \
                  <A HREF="#" onClick="return false;")\
     >\$ifthen (\$empty(@2) \$per_name(@1))\
      \$ifthen (\$not_empty(@2) \$unquote(@2))\
      \$if (\$(showUserid) &nbsp;(@1))</A>)

#---Old version, before pop-up person window.
#eval $mac_define (person <A HREF="person.cml?\$(nch)+\$(nxt)+\
#                  \$arg(3)+\$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8)+\
#                  @1">\$per_name(@1)</A>)
  
#---%imagelink (imageURL targetURL [width [height]])
eval $mac_define (imagelink \
     <A HREF="@2" TARGET="_blank"><IMG SRC="@1" BORDER=0 \
                   \$ifthen (\$not_empty(@3) WIDTH=@3) \
                   \$ifthen (\$not_empty(@4) HEIGHT=@4)></A>)

#---%mailto (email_address Name)
eval $mac_define (mailto <A HREF="mailto:@1" \
           >\$if (\$empty(@2) @1 \$rest(2 @@))</A>)

#---%aim(handle)    hot link to AIM instant messenger for 'handle'
eval $mac_define (aim \
       \$ifthen (\$not_empty(@1) <A HREF="aim:goim?ScreenName=@1">@1</A>))

#---%telnet (host_address Name)
eval $mac_define (telnet <A HREF="telnet://@1" \
	>\$if (\$empty(@2) @1 \$rest(2 @@))</A>)

#--- %implemented (func_call)
#  Is this function call implemented?  Must be a semi-valid form of the
#  function call, test works by looking for "=== " or "NO FUNC" in return
#  value.  A semi-reasonable way to write CML code that depends on very
#  functions, but can act reasonably if run on old versions of swebs.
eval $mac_define (implemented \$not (\$or (\
         \$equal (x<I>NO_FUNC</I>   \$replace (32 _ x@@)) \
         \$equal (\$str_index (===_ \$replace (32 _  @@)) 0))))

#--- %onnow (option_list) show table of current users.
#        See onnow.i for a list of valid options.
#    %onnow_link()        link to page showing table of current users
#    %onnow_check()       calculate list of users "on now".
#    %onnow_user(uid)     shows "(now)" if uid is on now.
#                         (Must call %onnow_check() once beforehand in page)
eval $mac_define (onnow       \$includecml($(main)/onnow.i \$quote(@@)))
eval $mac_define (onnow_link <a href="people_on.cml?\$(nch)+\$(nxt)+\
                     \$arg(3)+\$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8)+\
                     @1">@@</a>)
eval $mac_define (onnow_check \$includecml($(main)/onnow_calc.i))
eval $mac_define (onnow_user  \$ifthen (\$tablefind (@1 \$(onnow_users)) (now)))

#------------------------------------------
#===Operational macros used inside Caucus.
#
#---%location(url)   Replaces all uses of "Location:" directive
#       to go to another CML file.  Acts as an effective work-around for the
#       IE 4 bug (see http://gamgee.acad.emich.edu/~roth/ie_location.html).
#       If the URL has no #fragment, or is not IE 4, %location just spits
#       out the appropriate "Location:" directive.  Otherwise, it produces
#       a tiny HTML page that sets location.href to the proper url.

eval $mac_define (location \
    Pragma: no-cache$newline()\
    Expires: Thu, 01 Dec 1994 16:00:00 GMT$newline()\
    Location: @1\$newline()\$newline())

#---%chk(X) macro, used in cus_en*.cml.  If X is in table CHK,
#   output a "CHECKED" tag (for <INPUT TYPE=checkbox...> stuff).
eval $mac_define (chk \$if (\$tablefind (@1 \$(chk)) CHECKED))

#---%en_var(user conf var)  Get value of e-mail notification variable.
eval $mac_define (en_var \
        \$ifthen (      @1  \$user_var (\$userid() @3_@2)) \
        \$ifthen (\$not(@1) \$conf_var (@2         @3   )) )

#---%set_en_var(user conf var values...)  Set value of e-mail
#   notification variable.
eval $mac_define (set_en_var \
        \$ifthen (      @1  \$set_user_var (\$userid() @3_@2 \$rest(4 @@))) \
        \$ifthen (\$not(@1) \$set_conf_var (@2         @3    \$rest(4 @@))) )

#---%bad_email_address (x)
#   Bad if not empty and (no @, no ., has any of ";|&,<>()", or > 1 words).
#   Could be made smarter...
eval $mac_define (bad_email_address \
        \$and (\$not_empty(@@) \
            \$or (\$less    (\$str_index (@ @@)  0) \
                  \$less    (\$str_index (. @@)  0) \
                  \$greater (\$str_index (; @@) -1) \
                  \$greater (\$str_index (| @@) -1) \
                  \$greater (\$str_index (& @@) -1) \
                  \$greater (\$str_index (, @@) -1) \
                  \$greater (\$str_index (< @@) -1) \
                  \$greater (\$str_index (> @@) -1) \
                  \$greater (\$str_index ($char(40) @@) -1) \
                  \$greater (\$str_index ($char(41) @@) -1) \
                  \$greater (\$sizeof(@@) 1))))

#---%hdhash (link_title)   header.i/trailer.i self-click
#  Probably not needed...
#eval $mac_define (hdhash <A HREF="" onClick="location.hash='top';return false;">@@)

#---%nooplink(text)    Make a link with TEXT that does nothing.
#   Used for the last entry in the location (compass) bar.
#eval $mac_define (nooplink \
#       \$ifthen (\$(use_javascript) \
#           <A HREF="#" onClick="return false;" \%help(h_here)>@@</A>)\
#       \$ifthen (\$not(\$(use_javascript)) \
#           <U><FONT COLOR="\$word(1 \$conf_var(\$(cnum) conflink) BLUE)"\
#              >@@</FONT></U>))
eval $mac_define (nooplink \
           <A HREF="#" onClick="return false;" \%help(h_here)>@@</A>))

#---%backlink(text)    Make a link with TEXT that does a "BACK".
eval $mac_define (backlink \
        <A HREF="#" \
           \$ifthen (\$(use_javascript) onClick="history.back();")\
        >@@</A>)

#===MSET, MPUSH, MPOP.  Handle "m" values (m_cnum, m_inum, m_rnum) ==========
#   that are used in all of the %liburl2() derived macros for uploaded files.

#---%mset (c i r)      Set values of m_cnum, m_inum, m_rnum that are
#   used in %liburl2 macros (when they are outside of their proper item)
eval $mac_define (mset \$set (m_cnum @1) \$set (m_inum @2) \$set (m_rnum @3))

#---%mpush (c i r)     Save current m values on a stack, then set new ones.
eval $mac_define (mpush \
        \$set (mstack \$(m_cnum) \$(m_inum) \$(m_rnum) \$(mstack))\
        \$set (m_cnum @1)  \$set (m_inum @2)  \$set (m_rnum @3)\
     )

#---%mpop()            Pop m values off of stack.
eval $mac_define (mpop \
        \$set (m_cnum \$word (1 \$(mstack)))\
        \$set (m_inum \$word (2 \$(mstack)))\
        \$set (m_rnum \$word (3 \$(mstack)))\
        \$set (mstack \$rest (4 \$(mstack)))\
     )

#===POP-UP window macros============================================

#---Size of pop-up window.  (User vars are xs_@1, ys_@1.)
#   %userxs(popup defaultx), %userys(popup defaulty)
eval $mac_define (userxs \
        \$if (\$empty (\$user_var ($userid() xs_@1))  @2 \
                       \$user_var ($userid() xs_@1) ) )
eval $mac_define (userys \
        \$if (\$empty (\$user_var ($userid() ys_@1))  @2 \
                       \$user_var ($userid() ys_@1) ) )

#---%to_parent (url) ilist.i  Redirect links to parent window if
#   $(target_switch) is true.
#   Usage: <A HREF=%to_parent(url)>
eval $mac_define (to_parent \
        \$ifthen (\$not(\$(target_switch)) "@1")\
        \$ifthen (\$(target_switch) "#" \
           onClick="window.opener.location.href = '@1';") )


#---------------------------------------------------------------------
# "Built-in" macros.  These are special macros that *must* be defined,
# because calls to these macros are generated by 'C' code in the
# swebs CML interpreter.

#---%_new_target_window(newx, newy)       (nee "%target" in 4.0)
#  Opens a new window to be used as a target for some URL.
#  If possible, makes new window 40 pixels less in x and y than
#  the current window.  For old browsers, the new window size is
#  newx x newy pixels.
#
#  Typically used in an <A HREF> tag to put the target URL in
#  a new window, as in:
#     <A HREF="some_url" %_new_target_window(x, y)>

#eval $mac_define (_new_target_window TARGET="_blank")

 eval $mac_define (_new_target_window \$char(32)onClick="\
    win = window.open ('', 'fromcau', \
      'resizable=1,toolbar=1,scrollbars=1,menubar=1,location=1,status=1,' + \
    \$set (ws \$word(1 \$user_var(\$userid() my_win_size) Default))\
    \$ifthen (\$equal (\$(ws) Default) \
      $ifthen ($(use_resize) \
         'width='  + $if ($(is_netscape4)  (window.outerWidth+$(dw_nn))  \
                                   (document.body.clientWidth+$(dw_ie))) + \
        ',height=' + $if ($(is_netscape4) (window.outerHeight+$(dh_nn)) \
                                  (document.body.clientHeight+$(dh_ie))))\
      $ifthen ($not ($(use_resize)) 'width=@1,height=@2')\
    )\
    \$ifthen (\$not_equal (\$(ws) Default) \
       'width=\$word (1 \$replace(x 32 \$(ws))),\
        height=\$word(2 \$replace(x 32 \$(ws)))')\
      ); $(winfocus)  return true;" TARGET="fromcau" )

#---%_caucus_url(cnum inum rnum)          (nee "%special" in 4.0)
#  Translate 'http:/caucus/...' so-called "special" URLs into
#  URLs that reference the appropriate CML pages.
# 
#  Thus, http:/caucus              -> (goes to) the Caucus Center
#        http:/caucus/conf         -> conference home page of 'conf'
#        http:/caucus/conf/item    -> conference 'conf', item # 'item'
#        http:/caucus/conf/item:r  -> 'conf' item 'item', response 'r'.

eval $mac_define (_caucus_url \
        \$ifthen (\$empty(@1) \$(mainurl)/center.cml?\$(nch)+\$(nxt)+x+x+x+x+x+x)\
        \$ifthen (\$and (\$not_empty(@1) \$empty(@2)) \
           \$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+@1+x+x+x+x+x)\
        \$ifthen (\$and (\$not_empty(@1) \$not_empty(@2)) \
           \$(mainurl)/viewitem.cml?\$(nch)+\$(nxt)+@1+@2+@3+x+x+x#here)\
     )

#===================================================================
# New macros for "Bonanza 1.2" (add-ons to initial 4.10 CML code).
#
# "Conf" macros -- links and urls to conference entities.
#
#--- %conf_is_allowed (conf_name text)
#    If current user is allowed in CONF_NAME, evaluates TEXT.
eval $mac_define (conf_is_allowed \$set(ignore \$cl_list(@1))\
        \$ifthen (\$cl_access(\$cl_num(@1)) \$rest(2 @@)))

#--- %conf_has_new (conf_name text)
#    If new material in CONF_NAME, evaluates TEXT.
eval $mac_define (conf_has_new \$set(ignore \$cl_list(@1))\
        \$ifthen (\$or (\$it_inew(\$cl_num(@1)) \
                        \$it_iunseen(\$cl_num(@1)) \
                        \$it_wnew(\$cl_num(@1))) \
                  \$rest (2 @@)))

#---%conf_nonmember (confname text)
#   If current user is allowed in 'confname', but is not a member,
#   evaluate 'text'.
eval $mac_define (conf_nonmember \
        \$set (tnum 0\$word (1 \$cl_list (@1)))\
        \$ifthen ( \$and (\$greater (\$cl_access (\$(tnum)) 0) \
                          \$not (\$it_member (\$(tnum)))) \
                   \$rest (2 @@) ) )

#--- %conf_link (conf_name text)
#    Creates a link to CONF_NAME, evaluates TEXT as text of link.
eval $mac_define (conf_link \$set(ignore \$cl_list(@1))\
        <A HREF="\$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+x+x+x+x+x">\
        \$rest(2 @@)</A>)

#--- %conf_url (conf_name)
#    Evaluates to full URL for conference CONF_NAME.  Use
#    "/caucus/conf_name" instead unless contraindicated.
eval $mac_define (conf_url \$set(ignore \$cl_list(@1))\
        \$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+x+x+x+x+x)

#--- %conf_item_url (conf_name item)
#    Evaluates to full URL for conference CONF_NAME, ITEM.  Use
#    "/caucus/conf_name/item instead unless contraindicated.
eval $mac_define (conf_item_url \$set(ignore \$cl_list(@1))\
        \$(mainurl)/viewitem.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+@2+0+0+1+x#here)

#--- $conf_name_with_new (conf_name text)
#    "Convenience macro", combines other macros to show link to
#    conference (if allowed), with a new icon (if anything is new).
eval $mac_define (conf_name_with_new \
        \%conf_is_allowed (@1 \%conf_link (@1 \$rest(2 @@) \
                              \%conf_has_new (@1 \%icon_new()))))

#--- %conf_file_url (conf_name user item resp filename)
#    Full URL for file FILENAME uploaded by USER into CONF_NAME/ITEM:RESP.
eval $mac_define (conf_file_url \
        $(http)://$http_lib()/@1/@2/\%digit4(@3)\%digit4(@4)/@5)

#--- %ifs_page_link (item resp text)    ("InterFace Storage PAGE LINK")
#    Show TEXT as a link to ifspage.cml (shows a response as a single
#    "resource" page), ITEM:RESP from Interface_Storage_$(iface) conference.
eval $mac_define (ifs_page_link \
        <A HREF="\$(mainurl)/ifspage.cml?\$(nch)+\$(nxt)+x+x+x+x+@1+@2"> \
        \$rest(3 @@)</A> )

#---%ifs_popup (item resp text)
#    Pop-up a new page, with contents of IFS (item, resp) -- and nothing else!
eval $mac_define (ifs_popup \
              <a href="\$(mainurl)/ifspopup.cml?\$(nch)+\$(nxt)+x+x+x+x+@1+@2" \
                 onClick="\%js_open_win(ifs_popup)" target="ifs_popup" \
                 >\$rest(3 @@)</a> )

#--- %conf_has_new_br (conf_name text)
#    If new material in CONF_NAME, evaluates TEXT.
eval $mac_define (conf_has_new_br \$set(ignore \$cl_list(@1))\
        \$ifthen (\$or (\$it_inew(\$cl_num(@1)) \
                        \$it_iunseen(\$cl_num(@1)) \
                        \$it_wnew(\$cl_num(@1))) \
                  \$rest (2 @@))<BR>)

#--- %conf_link_br (conf_name_br text)
#    Creates a link to CONF_NAME, evaluates TEXT as text of link
#    and puts a break at the end
eval $mac_define (conf_link_br \$set(ignore \$cl_list(@1))\
        <A HREF="\$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+x+x+x+x+x">\
        \$rest(2 @@)</A><BR>)

#--- %conf_name_with_new_br (conf_name text)
#    "Convenience macro", combines other macros to show link to
#    conference (if allowed), with a new icon (if anything is new) along
#    with a <BR> at the end of each line.
eval $mac_define (conf_name_with_new_br \
        \%conf_is_allowed (@1 \%conf_link (@1 \$rest(2 @@) \
                              \%conf_has_new_br (@1 \%icon_new()))))


#--- %ifs_resp (item [resp])            ("InterFace Storage RESPonse")
#    Evaluates to text of Interface_Storage_$(iface)/ITEM:RESP, interpreted
#    as CML.
eval $mac_define (ifs_resp \$set(ignore \$cl_list(interface_storage_$(iface)))\
      \$cleanhtml(prohibited \$protect(\$mac_expand(\
         \%mpush(\$cl_num(interface_storage_$(iface)) @1 0@2)\
         \$reval(\$cleanhtml(_empty \
         \$re_text(\$cl_num(interface_storage_$(iface)) @1 0@2)))\
         \%mpop()) )\
      ))

#--- %iframe (url pure optwidth optheight options)
#    Display IFRAME with URL and optwidth and optheight and any other
#    options.  Reserved=0 for now.
#    For netscape, just makes a pop-up window link.
#    (The &nbsp; is a work-around for an IE bug that occurs when an
#    IFRAME appears in an otherwise empty table cell with % widths.)
eval $mac_define (iframe \
  &nbsp;<IFRAME SRC="\$word(1 @@)" \
            WIDTH="\$word (1 \$word (3 @@) \$(default_iframe_width))"  \
           HEIGHT="\$word (1 \$word (4 @@) \$(default_iframe_height))" \
              \$rest(5 @@)>\
              <TABLE BORDER=2 CELLSPACING=5 CELLPADDING=5>\
              <TR><TD>\%url(\$word(1 @@))</TD></TABLE>\
        </IFRAME>)

#-------------------------------------------------------------
#  New "image" macros:
#
#--- %image(URL [width height [alt text]])
eval $mac_define (image \
        <IMG SRC="@1" BORDER=0 \
             \$ifthen (\$not_empty(@3) WIDTH="@2" HEIGHT="@3") \
             \$ifthen (\$not_empty(@4) ALT="\$rest(4 @@)")>)

#--- %icon_new()   Complete <IMG ...> tag for "new" icon.
eval $mac_define (icon_new <IMG $(src_isnew) BORDER=0>)

#===============================================================

#--- %files (name text)
set filesurl   $(http)://$host()/~$caucus_id()/FILES
eval $mac_define (files \%url(\$(filesurl)/@1 \
                        \$if (\$empty(@2) @1 \$rest(2 @@))))
eval $mac_define (files_url      $(filesurl)/@1)



#---Filesafe macro: %filesafe (name [optional link text])
#   If user is allowed in filesafe, displays link to filesafe.
eval $mac_define (filesafe \
   \$ifthen (\$and (\$in_group ($userid() \$lower(@1)) \
                    \$not_empty (\$site_var(FN_\$lower(@1)))) \
      <a href="\$(mainurl)/filesafe.cml?\$(nch)+\$(nxt)+\$arg(3)+\$arg(4)+\
               \$arg(5)+\$arg(6)+\$arg(7)+\$arg(8)+\$lower(@1)">\
                  \$ifthen (    \$empty(@2) \$upper1 (\$replace (_ 32 @1)))\
                  \$ifthen (\$not_empty(@2) \$rest (2 @@))</a>)\
)

#---filesafes macro: %filesafes (prefix)
#   List all viewable filesafes beginning with "prefix"; if none list all
eval $mac_define (filesafes \
        \$includecml (\$(iface)/main/filesafes.i \
           \$arg(2)+\$arg(3)+\$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8) @1))

#---filesafesec (name section [link text])
eval $mac_define (filesafesec \
      \$includecml (\$(iface)/main/filesafesec.i 1 @1 @2 \
                    \$arg(3)+\$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8))\
      \$ifthen (    \$empty(@3) \$upper1(\$replace(_ 32 \$lower(@1) / @2))</a>)\
      \$ifthen (\$not_empty(@3) \$rest(3 @@)</a>)\
    )

#---filesafesecurl (name section)
eval $mac_define (filesafesecurl \
       \$includecml (\$(iface)/main/filesafesec.i 0 @1 @2 \
                     \$arg(3)+\$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8))\
     )

#---filesafefile (name section file)
#   Link to filesafe file.
eval $mac_define (filesafefile \
        \$includecml (\$(iface)/main/filesafefile.i 1 @1 @2 @3))

#---filesafefileurl (name section file)
#   Link to filesafe file.
eval $mac_define (filesafefileurl \
        \$includecml (\$(iface)/main/filesafefile.i 0 @1 @2 @3))

#---filesafethumb (name section filesmall filebig [width height])
eval $mac_define (filesafethumb \
        <a href="\%filesafefileurl(@1 @2 @4)" \
           onClick="win = window.open ('', 'thumb', \
             'resizable=1,toolbar=0,scrollbars=1,menubar=0,location=0,\
              status=0\$ifthen (\$not_empty (@6) ,width=@5,height=@6)'); \
              $(winfocus) return true;" target="thumb" \
           ><img src="\%filesafefileurl(@1 @2 @3)" border=0></a>)

#---%login_gme (url text)
#   Example macro for how to integrate login with another web
#   application (in this case GroupMind Express).  See login_gme.cml
#   for more info.  Caller must supply login form submission
#   page as 'url', 'text' as text of link.
eval $mac_define (login_gme \
              <a href="\$(mainurl)/login_gme.cml?\$(nch)+\$(nxt)+@1" \
                 onClick="\%js_open_win(login_gme)" target="login_gme" \
                 >\$rest(2 @@)</a> )

#---%epoch_of (x)
#   Returns epoch time.  X can either be an epoch time number, or
#   a date string (such as produced by $dateof()).  Used when forcing
#   a possibly unknown format into epoch time.  Returns 0 if X is empty.
eval $mac_define (epoch_of \$if (\$empty(@1) 0 \$if (\$max(@1) @1 \$epoch(@1) )))

#---preinc(x)
#   Pre-increment x.  Like C "++x"
eval $mac_define (preinc \$set (@1 \$plus (\$(@1) 1))\$(@1))

#---%displayed_conf_name(cnum)
#   Calculate display form of conference name for conf 'cnum'.
#eval $mac_define (displayed_conf_name \
#                  \$upper1 (\$replace (_ 32 \$cl_name(@1))))
eval $mac_define (displayed_conf_name \
   \$set (dcntemp \$site_var (ConfAlias_@1))\
   \$ifthen (\$not_empty (\$(dcntemp)) \$(dcntemp))\
   \$ifthen (    \$empty (\$(dcntemp)) \
                     \$upper1 (\$replace (_ 32 \$cl_name(@1))))\
)

#---%interface (dir [text])    Link to start using interface 'dir'.
eval $mac_define (interface  \
   \$ifthen (\$exists ($caucus_path()/CML/@1/main) \
      <a href="interface.cml?\$unique()+@1">\
         \$if (\$empty(@2) @1 \$rest (2 @@))</a>)\
   \$ifthen (\$not (\$exists ($caucus_path()/CML/@1/main)) \
      <u>\$if (\$empty(@2) @1 \$rest (2 @@))</u>))

#---stripquotes: strip leading and trailing double quote off of string,
#                if present.
eval $mac_define (stripquotes \
     \$set (gotquotes \
        \$and (\$equal (\$str_sub (0                        1 @@) ") \
               \$equal (\$str_sub (\$minus (\$strlen(@@) 1) 1 @@) ")))\
     \$ifthen (\$(gotquotes) \$str_sub (1 \$minus (\$strlen(@@) 2) @@))\
     \$ifthen (\$not(\$(gotquotes)) @@))\
   )

#---%mainwindow (url text)
eval $mac_define (mainwindow \
     <a href="#" onClick="self.opener.location.href = '@1'">\$rest(2 @@)</a>)

include $(main)/announcedef.i 0

#---%datetz ( date | time)
#   Translate a 2-word date (e.g. the output from $dateof()) or a 1-word
#   time (e.g. the output from $time()) into the $dateof() format 
#   for the current timezone, as defined by $(timezonehrs).
#   E.g. if the server local time is EST, and $(timezonehrs) is set to -3,
#   then %datetz (23-MAY-2003 11:00) becomes "23-MAY-2003 8:00".
#eval $mac_define (datetz \
#          \$dateof (\$plus ( \$mult(3600 \$(timezonehrs)) \
#             \$if (\$not_equal (\$sizeof(@@) 1)  \$epoch(@1 @2)   @@) \
#          )) \
#       )

 eval $mac_define (datetz \
           \$set (_dt \$replace (- 32 \
              \$dateof (\$plus ( \$mult(3600 \$(timezonehrs)) \
                 \$if (\$not_equal (\$sizeof(@@) 1)  \$epoch(@1 @2)   @@) \
               )))\
           )\
           \$upper1(\$lower(\$word (2 \$(_dt)))) \
           \$word(1 \$(_dt)), \
           \$rest(3 \$(_dt)) \
        )

#---%trimtext (chars text)
#   Display 1st 'chars' of 'text'.  If not all of 'text' is displayed,
#   add a "..." on the end to indicate text was trimmed.
eval $mac_define (trimtext    \$str_sub (0 @1 \$rest(2 @@))\
                  \$if (\$less (@1 \$strlen(\$rest (2 @@))) ...))

#---%rtrim (text)
#   "Right trim" text, i.e. remove all trailing blanks.
#   Note we need to remove trailing newline generated by $includecml()!
eval $mac_define (rtrim \
                  \$set(_rtrim \$includecml(\$(main)/rtrim.i \$quote(@@)))\
                  \$str_sub (0 \$minus(\$strlen(\$(_rtrim)) 1) \$(_rtrim)))

#--------------------------------------------------------------------------
#   Macros added to support "gradebook", "assignments", and "symbols button" 
#   and related features.

#---%is_class(cnum)    Is this a "class" conference?
eval $mac_define (is_class  \
        \$and (\$not_equal (@1 x)  \
               \$(_supports_class) \
               \$equal (x\$conf_var (@1 confType) xCourse)))

#---%supports_class()   Does this install support "class" (course) conferences?
eval $mac_define (supports_class  0\$(_supports_class))

#---%supports_sql()     Does this install have a working SQL connection?
eval $mac_define (check_sql \
        \$ifthen (\$not (\$(_supports_sql)) \
           <script type="text/javascript" language="javascript">\
           confirm ('Error: SQL connection is not working!');\
           </script>\
        ))

#---%yyyymmddOf (date)
#   Convert common date formats to yyyy-mm-dd.
eval $mac_define (yyyymmddOf  \
        \$includecml (\$(main)/yyyymmddof.i @@))

#---%epochOfyyyy (date)
#   Converts yyyy-mm-dd dates to epoch times.
eval $mac_define (epochOfyyyy \$epoch(\
        \$str_sub(8 2 @1)-\
        \$word (\$str_sub (5 2 @1) \
           jan feb mar apr may jun jul aug sep oct nov dec)-\
        \$str_sub(0 4 @1)))

#---%dateOfyyyy (date)
eval $mac_define (dateOfyyyy \
        \$str_sub(8 2 @1)-\
        \$word (\$str_sub (5 2 @1) \
           jan feb mar apr may jun jul aug sep oct nov dec)-\
        \$str_sub(0 4 @1))

#---%monthDayYear (date)
#   Convert yyyy-mm-dd format to "Mon dd, yyyy".
eval $mac_define (monthDayYear  \
        \$ifthen (\$and (\$not_empty(@1)  \$not_equal (@1 0000-00-00)) \
           \$set (temp \$replace(- 32 @1))\
           \$word (\$word (2 \$(temp)) \
                  Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec) \
           \$word (3 \$(temp)), \$word (1 \$(temp))))

#---%tab([x])
#   Evaluates to auto-incremented 'tabindex="n"', for use in 
#   defining tabbing order for <input> objects.
#   %tab(x) starts sequence over again at 'x'.
#   %tab()  increments current value of variable tabindex.
eval $mac_define (tab \
        \$set (tabindex \$word (1 @1 \$plus (\$(tabindex) 1)))\
        tabindex="\$(tabindex)")

#---%fName (uid)
#   "Firstname" of userid uid.
eval $mac_define (fName \
         \$set (_name    \$per_name (@1))\
         \$set (_namelen \$sizeof (\$(_name)))\
         \$words (1 \$minus (\$(_namelen) 1) \$(_name)))

#---%lName (uid)
#   "Lastname" of userid uid.
eval $mac_define (lName \
         \$set (_name    \$per_name (@1))\
         \$set (_namelen \$sizeof (\$(_name)))\
         \$word  (\$(_namelen) \$(_name)))

#---%assign(key linktext)
#   Make "linktext" a link to a specific assignment.
eval $mac_define (assign \
       <a href="\$(mainurl)/assignedit.cml?\$(nch)+\$(nxt)+\$arg(3)+x+@1+x+x+x"\
       >\$if (\$empty(@2) Assignment-@1 \$rest(2 @@))</a>)

#---Math symbol utility macros...
#
#---For some reason, class="x" where x has text-decoration: none doesn't
#   work in IE6.
eval $mac_define (mathsym  \
       \$set (_sym \$tablematch (@1 \$(symnam)))\
       \$ifthen (\$greater (\$(_sym) 0) \
          <a href="#" style="text-decoration: none;" \
           onClick="document.ebox.tx_resp.value+='\\%sym(@1)'; return false;"\
            >\$unquote (\$word (1 \$replace (, 32 \
                        \$word (\$(_sym) \$(symval)))))</a>))

eval $mac_define (sym \
       \$set (_sym \$tablematch (@1 \$(symnam)))\
       \$ifthen (\$greater (\$(_sym) 0) \
          \$unquote(\$word (-1 \$replace (, 32 \$word (\$(_sym) \$(symval)))))))

#---%mathimg (imgblue [imgblack])
eval $mac_define (mathimg \
           \$quote(<img src="/~\$caucus_id()/MATHSYM/@1" border=0>)\
        \$ifthen (\$not_empty(@2) ,\
           \$quote(<img src="/~\$caucus_id()/MATHSYM/@2" border=0>)))

#----------------------------------------------------------------------

#---%courselist(options)
#   Display list of courses.  See courselist.i for option list.
eval $mac_define (courselist   \$includecml(\$(main)/courselist.i @@))

#---%no_such_conf (cnum)
eval $mac_define (no_such_conf \$gt_equal (\$str_index (NO \$cl_name(@1)) 0))

#---%is_instructor()
#   Is current user is a manager, or an organizer or instructor of any conf?
eval $mac_define (is_instructor \$includecml(\$(main)/isinstructor.i))

#---%listpeople (separator userid [userid2...])
eval $mac_define (listpeople  \
        \$includecml (\$(main)/listpeople.i  @1 \$quote(\$rest(2 @@))))

#---%instructors (cnum)
#   List of userids of "instructors" of this course, in the order they appear
eval $mac_define (instructors \$includecml(\$(main)/instructors.i @1))

#---%emImgSep (prop)
eval $mac_define (emImgSep %b(&nbsp;&nbsp;))

#---%emFileSep (prop)
eval $mac_define (emFileSep %b(<p/>))

#---%urcregnums()
#   Sorted list of field numbers of REG urc field variables, e.g.
#   REG_CC44/reg_1 is "1", etc.
eval $mac_define (urcregnums \$includecml(\$(main)/urcregnums.i))

#-----------------------------------------------------------------------
