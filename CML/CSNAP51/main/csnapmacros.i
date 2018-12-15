#
#---CSNAPMACROS.I    Define Caucus "macros" for this interface.
#
#   Contains definitions of Caucus macros that are specific to
#   CSNAP, i.e. override the normal definition in macros.i.
#
#   Do ***NOT*** edit this file.  If you have any additions or
#   changes to make, do the following:
#      1) If a main.1/localmacros.i file exists, put changes in there
#   else
#      1) Create a "main.1" directory.
#      2) Copy main/localmacros.i to main.1
#      3) Edit localmacros.i there.
#
#: CR 10/28/98 12:06 CSNAP 1.0
#: CR  5/28/99 17:30 CSNAP 1.1, Caucus 4.1, 8.3 file names
#: CR 12/16/01 18:38 define mset, use m_cnum, m_inum, m_rnum
#: CR  3/10/02 21:00 4.4
#: CR 10/22/02 Final 4.4 wrap-up.
#: CR 12/04/02 modify %person(), do not build/display personal info.
#: CR 03/17/05 Add %photo() specific to csnap.
#: CR 01/05/07 Replace $per_name().
#==============================================================
#

#===Sub-macros, used by other macros: ================================

#---%js_open_win(name)    Javascript to open a new window
eval $mac_define (js_open_win \
          win = window.open ('', '@1',\
          'resizable=1,toolbar=1,scrollbars=1,menubar=1,location=1,\
          width=\$word(1 \$new_win()),height=\$word(2 \$new_win())'); )

#---MSET, MPUSH, MPOP.  Handle "m" values (m_cnum, m_inum, m_rnum) ==========
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

#=====================================================================
#===Simple text manipulation (HTML) macros:

#---%bold (text), %b(text)
eval $mac_define (bold <B>@@</B>)
eval $mac_define (b    <B>@@</B>)

#---%tt (text), %mono(text)
eval $mac_define (tt   <TT>@@</TT>)
eval $mac_define (mono <TT>@@</TT>)

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

#---%color (colorname text)
eval $mac_define (color <FONT COLOR="@1">\$rest(2 @@)</FONT>)

#---%u (text)
eval $mac_define (u     <U>@@</U>)
eval $mac_define (under <U>@@</U>)

#------------------------------------------
#==="Goto location" macros:

#---%_new_target_window (newx, newy)
 eval $mac_define (_new_target_window \$char(32)onClick="\
       win = window.open ('', 'fromcau', 'width=@1,height=@2,\
          resizable=1,toolbar=1,scrollbars=1,menubar=1,location=1,status=1');" \
               TARGET="fromcau" )

#---%url (actual_url, text of link)
#eval $mac_define (url <A HREF="@1" \
#           onClick="%js_open_win(fromcau)" TARGET="fromcau"\
#           >\$if (\$empty(@2) @1 \$rest(2 @@))</A>)
eval $mac_define (url <A HREF="@1" TARGET="_blank"\
           >\$if (\$empty(@2) @1 \$rest(2 @@))</A>)

#---%digit4(num)
eval $mac_define (digit4 \$str_sub(0 \$minus(4 \$strlen(@1)) 000)@1)

#---%response (rnum)
eval $mac_define (response <A HREF="../C\%digit4(\$(cnum))/\
                  \$(vi_inum)-0000.htm#@1">Response @1</A>)

#---%item (inum [rnum])
eval $mac_define (item <A HREF="../C\%digit4(\$(cnum))/\
      \$ifthen (\$empty(@2) \
                  \$item_id(\$(cnum) @1)0000.htm>Item @1</A>)\
      \$ifthen (\$not_empty(@2) \
                  \$item_id(\$(cnum) @1)0001.htm#@2">Item @1:@2</A>))

#---%conference (conf_name [inum [rnum]])
eval $mac_define (conference \
           \$set(cthis  \$word(1 \$cl_list(@1)))\
           \$set(ctname \%displayed_conf_name (\$(cthis)))\
           \$if (\$not (\$cl_access(\$(cthis))) <U>@1</U> \
              \$ifthen(\$empty(@2) \
                 <A HREF="../C\%digit4(\$(cthis))/index.htm">\$(ctname)</A>)\
              \$ifthen(\$and (\$not_empty(@2) \$empty(@3)) \
                 <A HREF="../C\%digit4(\$(cthis))/@2-0001.htm">\
                 \$(ctname) Item @2</A>)\
              \$ifthen(\$and (\$not_empty(@2) \$not_empty(@3)) \
                 <A HREF="../C\%digit4(\$(cthis))/@2-0000.htm#@3">\
                 \$(ctname) Item @2:@3</A>)\
           ))

#---%_caucus_url ([cnum [inum [rnum]]])
eval $mac_define (_caucus_url \
        \$ifthen (\$empty(@1) index.htm)\
        \$ifthen (\$and (\$not_empty(@1) \$empty(@2)) \
           ../C\%digit4(@1)/index.htm)\
        \$ifthen (\$and (\$not_empty(@1) \$not_empty(@2) \$not(@3)) \
           ../C\%digit4(@1)/@2-0001.htm)\
        \$ifthen (\$and (\$not_empty(@1) \$not_empty(@2) @3) \
           ../C\%digit4(@1)/@2-0000.htm#@3)\
     )

#---%readrange (startitem enditem name)
#   Named link to read range of items in current conference.
#   (CSNAP version does not stop at ENDITEM).
#eval $mac_define (readrange <A HREF="\$(href)/readrange.cml?\$arg(3)+@1+@2">\
#                  \$rest(3 @@)</A>)
eval $mac_define (readrange <A HREF="../C\%digit4(\$(cnum))/@1-0001.htm"\
                  >\$rest(3 @@)</A>)


#------------------------------------------
#===File upload macros
#
#---%liburl(userid file)   URL of file in library
eval $mac_define (liburl ../LIB/C\$(cnum)/@1/@2)

#eval $mac_define (liburl2 LIB/C\$(cnum)/@1/\
#       \$if (@2 \%digit4(\$(vi_inum))\%digit4(\$(response)) 00000000)/@3 )
eval $mac_define (liburl2 ../LIB/ITEMS/\$(m_inum)/\$(m_rnum)/@3)

#---%libimg(userid file)   IMG file in library
eval $mac_define (libimg <IMG SRC="../LIB/C\$(cnum)/@1/@2">)

#---%libimg2 (userid rdir file)   IMG file in library
eval $mac_define (libimg2 <IMG SRC="\%liburl2(@1 @2 @3)" border=0>)

#---%libimg2r(userid rdir file)   IMG file in library, right aligned.
eval $mac_define (libimg2r <IMG SRC="\%liburl2(@1 @2 @3)" align=right border=0>)

#---%liblink(userid file)   link to file in library
eval $mac_define (liblink <A HREF="\%liburl(@1 @2)" \
                      TARGET="_blank">\%liburl(@1 @2)</A>)

#---%liblink2(userid rdir file)   link to file in library
eval $mac_define (liblink2 <A HREF="\%liburl2(@1 @2 @3)" \
                    TARGET="_blank">\%liburl2(@1 @2 @3)</A>)

#---%libname(userid file name)   named link to file in library
eval $mac_define (libname <A HREF="\%liburl(@1 @2)" TARGET="_blank"\
                  >\$rest (3 @@)</A>)

#---%libname2(userid rdir file name)   named link to file in library
eval $mac_define (libname2 <A HREF="\%liburl2(@1 @2 @3)" TARGET="_blank"\
                  >\$rest (4 @@)</A>)

#------------------------------------------
#===Other misc macros.

#---%person(userid)
eval $mac_define (person \
     \$set(_pn \$if (\$empty (\$user_info(@1 lname)) _null @1))\
     \$ifthen (      $(do_people) <A HREF="../PEOPLE/\$(_pn).htm">\
        \$ifthen (\$empty(@2)      \$user_info(@1 fname) \$user_info(@1 lname))\
        \$ifthen (\$not_empty(@2)  \$unquote(@2))</a>)\
     \$ifthen ($not ($(do_people)) \$user_info(@1 fname) \$user_info(@1 fname)))

#---%photo (userid [options])
#   image of person, scaled with [options], links to pop-up of person
eval $mac_define (photo \
   \$ifthen (\$not_empty (\$user_info(@1 lname)) \
      \$ifthen (\$not_empty (\$user_info (@1 picture)) \
         \%person(@1 \
         \$set(_picpos \$str_index (PICTURES/@1/ \$user_info(@1 picture)))\
         \$quote(<img \
            src="../LIB/\$str_sub(\$(_picpos) 1000 \$user_info(@1 picture))" \
            \$rest(2 @@) border=0>)))))

#---Each field of info about them.
set picture $user_info ($inc(1) picture)
set picpos  $str_index (PICTURES/$inc(1)/ $(picture))
if $greater ($(picpos) 0)
   set  picture $str_sub ($(picpos) 1000 $(picture))
   eval $copyfile($caucus_lib()/$(picture) $(target)/LIB/$(picture) 755)
   set  picture LIB/$(picture)
end


#---%mailto (email_address Name)
eval $mac_define (mailto <A HREF="mailto:@1" \
           >\$if (\$empty(@2) @1 \$rest(2 @@))</A>)

#---%telnet (host_address Name)
eval $mac_define (telnet <A HREF="telnet://@1" \
	>\$if (\$empty(@2) @1 \$rest(2 @@))</A>)


#===================================================================
# New macros for "Bonanza 1.2" (add-ons to initial 4.10 CML code).
#
# "Conf" macros -- links and urls to conference entities.
#
#--- %conf_is_allowed (conf_name text)
#    If current user is allowed in CONF_NAME, evaluates TEXT.
eval $mac_define (conf_is_allowed \$rest(2 @@))

#--- %conf_has_new (conf_name text)
#    If new material in CONF_NAME, evaluates TEXT.
eval $mac_define (conf_has_new )

#--- %conf_link (conf_name text)
#    Creates a link to CONF_NAME, evaluates TEXT as text of link.
eval $mac_define (conf_link \
        <A HREF="../C\%digit4(\$cl_num(@1))/index.htm">\$rest(2 @@)</A>)

#--- %conf_link_br (conf_name text)
#    Creates a link to CONF_NAME, evaluates TEXT as text of link.
eval $mac_define (conf_link_br \
        <A HREF="../C\%digit4(\$cl_num(@1))/index.htm">\$rest(2 @@)</A><br>)


#--- %conf_url (conf_name)
#    Evaluates to full URL for conference CONF_NAME.  Use
#    "/caucus/conf_name" instead unless contraindicated.
eval $mac_define (conf_url ../C\%digit4(\$cl_num(@1))/index.htm)

#--- %conf_item_url (conf_name item)
#    Evaluates to full URL for conference CONF_NAME, ITEM.  Use
#    "/caucus/conf_name/item instead unless contraindicated.
eval $mac_define (conf_item_url \
        ../C\%digit4(\$cl_num(@1))/@2-0001.htm)

#--- $conf_name_with_new (conf_name text)
#    "Convenience macro", combines other macros to show link to
#    conference (if allowed), with a new icon (if anything is new).
eval $mac_define (conf_name_with_new \
        \%conf_is_allowed (@1 \%conf_link (@1 \$rest(2 @@) \
                              \%conf_has_new (@1 \%icon_new()))))

#--- %conf_name_with_new_br (conf_name text)
#    "Convenience macro", combines other macros to show link to
#    conference (if allowed), with a new icon (if anything is new) along
#    with a <BR> at the end of each line.
eval $mac_define (conf_name_with_new_br \
        \%conf_is_allowed (@1 \%conf_link (@1 \$rest(2 @@) \
                              \%conf_has_new_br (@1 \%icon_new()))))

#--- %conf_file_url (conf_name user item resp filename)
#    Full URL for file FILENAME uploaded by USER into CONF_NAME/ITEM:RESP.
#    PROBABLY WRONG!
eval $mac_define (conf_file_url \
        $(http)://$host()/~$caucus_id()/LIB/@1/@2/@3-@4/@5)

#--- %ifs_page_link (item resp text)    ("InterFace Storage PAGE LINK")
#    Show TEXT as a link to ifspage.cml (shows a response as a single
#    "resource" page), ITEM:RESP from Interface_Storage_$(dir) conference.
eval $mac_define (ifs_page_link \
        <A HREF="../IFS/@1-@2.htm">\$rest(3 @@)</A> )

#--- %ifs_resp (item resp)             ("InterFace Storage RESPonse")
#    Evaluates to text of Interface_Storage_$(iface)/ITEM:RESP, interpreted
#    as CML.
eval $mac_define (ifs_resp \$eval(\$cl_list(interface_storage_\$(iface)))\
      \$cleanhtml(prohibited \$protect(\$mac_expand(\
         \%mpush(\$cl_num(interface_storage_\$(iface)) @1 0@2)\
         \$reval(\$cleanhtml(_empty \
         \$re_text(\$cl_num(interface_storage_\$(iface)) @1 0@2)))\
         \%mpop()) )\
      ))

#---%response_new (inum linktext...)
eval $mac_define (response_new )
             
eval $mac_define (itemtitle <A HREF="\%_caucus_url(\$(m_cnum) @1 0)">\
                   \$re_title (\$(m_cnum) @1 0)</A>)

#-------------------------------------------------------------
#  New "image" macros:
#
#--- %image(URL [width height [alt text]])
eval $mac_define (image \
        <IMG SRC="@1" \$ifthen (\$not_empty(@3) WIDTH=@2 HEIGHT=@3) \
                      \$ifthen (\$not_empty(@4) ALT="\$rest(4 @@)" border=0>))

#--- %icon_new()   Complete <IMG ...> tag for "new" icon.
eval $mac_define (icon_new <IMG $(src_isnew) BORDER=0>)

#-------------------------------------------------------------
#  old TMN macros

eval $mac_define (TMN_conf \%conf_link(@@))
eval $mac_define (TMN_conf_with_new \%conf_link(@@))
eval $mac_define (TMN_icon <IMG SRC="\$(icon)/@1" WIDTH="@2" HEIGHT="@3" \
                             BORDER="0" ALT="\$rest(4 @@)">)
eval $mac_define (showicon <IMG SRC="\$(icon)/@1" WIDTH="@2" HEIGHT="@3" \
                             BORDER="0" ALT="\$rest(4 @@)">)
eval $mac_define (TMN_item_href \%conf_item_url (@@))
eval $mac_define (TMN_conf_if_allowed_with_new_as_listitem \
        <LI>\%conf_link(@@))
#=========================================================================

#---New stuff

#--- %files (name text)
eval $mac_define (files <a href="../FILES/@1" TARGET="_blank">\
                        \$if (\$empty(@2) @1 \$rest(2 @@)))</a>)
eval $mac_define (files_url      ../FILES/@1)

eval $mac_define (displayed_conf_name \
                  \$upper1 (\$replace (_ 32 \$cl_name(@1))) )

#=========================================================================
# Final 4.4 wrap-up: macros in normal interface that were missing here.

eval $mac_define (confname_response_new )

#---%login_gme (url text)
#   Example macro for how to integrate login with another web
#   application (in this case GroupMind Express).   The CSNAP
#   version doesn't do the login (naturally).
eval $mac_define (login_gme \
              <a href="@1" \
                 onClick="\%js_open_win(login_gme)" target="login_gme" \
                 >\$rest(2 @@)</a> )

#--- %onnow (option_list) show table of current users
#    %onnow_link()        link to page showing table of current users
#  See onnow.i for a list of valid options.
eval $mac_define (onnow )
eval $mac_define (onnow_link )

#---Filesafe macros.  Noops for now.
eval $mac_define (filesafe )
eval $mac_define (filesafes )
eval $mac_define (filesafefile )
eval $mac_define (filesafefileurl )

#---%conf_nonmember (confname text)
#   If current user is allowed in 'confname', but is not a member,
#   evaluate 'text'.
eval $mac_define (conf_nonmember  \$rest(2 @@))

#--- %conf_has_new_br (conf_name text)
eval $mac_define (conf_has_new_br <br>)

#---%ifs_popup (item resp text)
#    Pop-up a new page, with contents of IFS (item, resp) -- and nothing else!
eval $mac_define (ifs_popup \
        <A HREF="../IFS/@1-@2.htm" \
                 target="_blank">\$rest(3 @@)</A> )

#---%backlink(text)    Make a link with TEXT that does a "BACK".
eval $mac_define (backlink \
        <A HREF="#" onClick="history.back();">@@</A>)

#---%chatter()   
eval $mac_define (chatter)

eval $mac_define (per_name \$user_info(@1 fname) \$user_info(@1 lname))
