#
#---MACROS.I    Define Caucus "macros" for this interface.
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
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#   Notes:
#      1. explain commenting syntax for CaucusMacroDoc
#      2. "FIX" entries need to be fixed to handle individual
#         item permissions.
#
#: CR 11/11/07 Make %person() handle no javascript; add %makeLegal()
#: CR 05/16/08 Add %groupMap()
#: CR 10/13/08 Begin rearranging for translation by CaucusMacroDoc.
#: CR 06/22/09 Add width=,height= to %ifs_popup().
#: CR 08/09/09 Add %swfVideo() macro.
#: CR 08/24/09 Interface-specific showUserid controls "(userid)" display instead of show_id.
#============================================================================

#@ Last revised 08/16/09

#@ <p/>
#@ The Caucus "macro" capability is a simple "macro expansion" language that
#@ is embedded into CML, the "Caucus Markup Language".  See the
#@ $mac_define() function in the Caucus Reference Manual for general information
#@ on defining Macros.

#@ <p/>
#@ There is a large set of pre-defined macros included with Caucus.
#@ They fall into three major categories:
#@ <ol type="I">
#@ <li>Simple macros that ordinary users can enter into their
#@ own items and responses.
#@ <li>More complex macros intended for power users, organizers,
#@ or managers, to define or extend aspects of the user interface.
#@ <li>Internal macros used in the CML pages to implement the user
#@ interface.  These are really being used as extensions to the
#@ CML language itself, and should not be seen or used by
#@ ordinary <b>or</b> power users.
#@ </ol>

#@ <p/>
#@ The macros in each major category are also divided by
#@ overall purpose into sub-categories, although the
#@ division is not always obvious.
#@ <p/>
#@ Any arguments inside []'s are optional and may be left out.&nbsp;
#@ Don't actually type the []'s.

#@ <p/>
#@ Some definitions of macro arguments follow:
#@ <ul>
#@ <li>groupType
#@ <ol>
#@ <li>Caucus4 -- for "system" groups.
#@ <li>Conf -- for conferences.
#@ <li>Filesafe -- for filesafes.
#@ <li>Item -- for individually permissioned item groups.
#@ <li>Mgr -- for manager groups.
#@ <li>Quiz -- for quiz groups.
#@ <li><i>userid</i> -- for groups for a specific user.
#@ </ol>
#@ </ul>
#@ <p/>
#@ <hr>


#@<p/>
#@<ol type="I">

#@<li><b>Ordinary User Macros</b>
#@<ol>

#@ <li><b>Text Manipulation Macros</b><br>
#@ Most of these were meant for the "plain text" editor,
#@ and are not needed for the "rich text" editor (aka RTE).

# %bold (text)
eval $mac_define (bold <B>@@</B>)
# %b(text)
eval $mac_define (b    <B>@@</B>)

# %tt (text)
eval $mac_define (tt <TT>@@</TT>)

# %blockquote (text)
eval $mac_define (blockquote <BLOCKQUOTE>@@</BLOCKQUOTE>)

# %italic(text)
eval $mac_define (italic <I>@@</I>)
# %i(text)
eval $mac_define (i      <I>@@</I>)

# %big(text)
eval $mac_define (big <BIG>@@</BIG>)

# %small(text)
eval $mac_define (small <SMALL>@@</SMALL>)

# %sub(text)
eval $mac_define (sub <SUB>@@</SUB>)

# %sup(text)
eval $mac_define (sup <SUP>@@</SUP>)

# %strike(text)
eval $mac_define (strike <STRIKE>@@</STRIKE>)

# %under(text)
eval $mac_define (under <U>@@</U>)
eval $mac_define (u <U>@@</U>)

# %mono(text)
eval $mac_define (mono <TT>@@</TT>)

# %color (colorname text)
eval $mac_define (color <FONT COLOR="@1">\$rest(2 @@)</FONT>)

# %bullet(text)
#  Indented "bulleted" paragraph.
eval $mac_define (bullet \
        <TABLE CELLSPACING=0 CELLPADDING=0 BORDER=0><TR VALIGN=top> \
        \$ifthen (       \$or(\$(is_explorer4) \$(is_netscape5)) \
             <TD>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</TD><TD><LI></TD>)\
        \$ifthen (\$not (\$or(\$(is_explorer4) \$(is_netscape5))) \
             <TD VALIGN=top><UL><LI></UL></TD>)\
        <TD WIDTH="100%">@@</TD></TABLE>)

# %ol(n text)
#     Indented numbered (N) paragraph.
eval $mac_define (ol \
        <TABLE CELLSPACING=0 CELLPADDING=0 BORDER=0><TR VALIGN=top> \
        <TD ALIGN=right>@1.&nbsp;<BR>\
           <IMG SRC="\$(icon)/clearpix.gif" WIDTH=40 HEIGHT=1></TD>\
        <TD WIDTH="100%">\$rest(2 @@)</TD></TABLE>)

#@<p/>
#@<li><b>"Go to" macros</b>
#@<p/>

# %response (rnum)
# Link to response 'rnum' in the current item.
eval $mac_define (response \
       <a href="viewitem.cml?\
                \$(nch)+\$(nxt)+\$(m_cnum)+\$(m_inum)+@1+x+x+x#here">\
                \$if (@1 Response&nbsp;@1 the item)</a>)

# !response_new (inum linktext...)
#---Not converted for 5.1 yet
#eval $mac_define (response_new \$ifthen (\
#          \$gt_equal (\$it_resps(\$(m_cnum) @1) \$it_newr(\$(m_cnum) @1)) \
#          <A HREF="\%_caucus_url(\$(m_cnum) @1 \$it_newr())">\$rest (2 @@))</A>)
  
# !confname_response_new (cname inum linktext...)
#eval $mac_define (confname_response_new \
#      \$set (_cfrn \$cl_num(@1))\
#      \$ifthen (\
#         \$gt_equal (\$it_resps(\$(_cfrn) @2) \$it_newr(\$(_cfrn) @2)) \
#         <A HREF="\%_caucus_url(\$(_cfrn) @2  \$it_newr())">\$rest (3 @@)</A>))

# %item (inum [rnum])
# Link to item 'inum', optionally to specific response 'rnum'.
eval $mac_define (item \
   \$set (_can \$plus(\$includecml(\$(main)/userCanAccessItem.cml \
                                   \$userid() \$(cnum) \$item_id(\$(cnum) @1))))\
   \$ifthen  (\$(_can) \
      <A HREF="\%_caucus_url(\$(m_cnum) @1 \$word(1 @2 0))">\
                  Item @1\$if (\$not_empty(@2) :@2)</A>)\
   \$ifthen  (\$not(\$(_can)) \
               <u>Item @1\$if (\$not_empty(@2) :@2)</u>))

#--FIX for individual permissions.
# %itemtitle(inum)
# Link to item 'inum', but show the title of the item as the link text.
eval $mac_define (itemtitle  \
         <a href="\%_caucus_url(\$(m_cnum) @1 0)">\
            \%item_title(\$item_id(\$(m_cnum) @1))</a>)

# %conference (conf_name [inum [rnum [hover text]]])
#  Link to conference 'conf_name', optionally specifying
#  item 'inum', response 'rnum'.
#   If 'hover text' is supplied, mousing-over the link will show
#   'hover_text' as a DHTML pop-over.  In that case, inum=0 means
#   just the conference, no item.
eval $mac_define (conference \
         \$set (cthis \$word (1 \$cl_list(@1)))\
         \$set (ctnam  \%displayed_conf_name (\$cl_list(@1)))\
         \$if (\$less (\$group_access ($userid() CONF \$(cthis)) 1) <u>@1</u> \
            \$ifthen (\$less(0@2 1) \
               <A HREF="\%_caucus_url(\$(cthis))" \
                 target="cmain$word (1 $replace (/ 32 $pid()))"\
                 \$ifthen(\$not_empty(@4) \
                    onMouseover="popOver('conf\$(cthis)-@2-@3', true);" \
                     onMouseOut="popOver('conf\$(cthis)-@2-@3', false);")\
                 >\$(ctnam)</A>)\
            \$ifthen (\$greater(0@2 0) \
               \$set (_can \$plus(\$includecml(\$(main)/userCanAccessItem.cml \
                                    \$userid() \$(cthis) \$item_id(\$(cthis) @2))))\
               \$ifthen (\$(_can) \
                  <A HREF="\%_caucus_url(\$(cthis) @2 @3 0)" \
                    target="cmain$word (1 $replace (/ 32 $pid()))"\
                    \$ifthen(\$not_empty(@4) \
                       onMouseover="popOver('conf\$(cthis)-@2-@3', true);" \
                        onMouseOut="popOver('conf\$(cthis)-@2-@3', false);")>\
                  \$(ctnam)&nbsp;Item&nbsp;@2\
                  \$if (\$not_empty(@3) :@3)</A>)\
               \$ifthen (\$not(\$(_can)) \
                  <u>\$(ctnam)&nbsp;Item&nbsp;@2\
                  \$if (\$not_empty(@3) :@3)</u>))\
            \$ifthen(\$not_empty(@4) \
               <div id="conf\$(cthis)-@2-@3" \
                  style="POSITION: absolute; visibility: hidden">\
               <table border="0" cellspacing="1" cellpadding="4" \
                 class="hoverTable">\
               <tr><td class="hoverCell">\$rest(4 @@)</td></table></div>)\
         )\
       )

# %ifaceConference (iface conf_name inum rnum text)
#  Evaluates to clickable link with text 'text'.
#  Clicking the link switches interface to 'iface', and
#  goes directly to conference 'conf_name', item label 'inum' (or 0 for no item),
#  response 'rnum' (or 0 for no response).
eval $mac_define (ifaceConference \
         \$set (cthis   \$word (1 \$cl_list(@2)))\
         \$set (cAccess \$group_access (\$userid() CONF \$(cthis)))\
         \$ifthen (\$less    (\$(cAccess) 1) <u><b>\$rest(5 @@)</b></u>) \
         \$ifthen (\$greater (\$(cAccess) 0) \
            \$ifthen (\$equal (@3 0) \
               <A HREF="ifaceConference.cml?@1+\$(cthis)+@3+@4" \
                  target="cmain$word (1 $replace (/ 32 $pid()))" >\$rest(5 @@)</A>)\
            \$ifthen (\$not_equal (@3 0) \
               \$set (_can \$plus(\$includecml(\$(main)/userCanAccessItem.cml \
                                    \$userid() \$(cthis) \$item_id(\$(cthis) @3))))\
               \$ifthen (\$(_can) \
                  <A HREF="ifaceConference.cml?@1+\$(cthis)+@3+@4" \
                     target="cmain$word (1 $replace (/ 32 $pid()))" >\$rest(5 @@)</A>)\
               \$ifthen (\$not(\$(_can)) \
                  <u>\$rest(5 @@))\
            )\
         )\
       )


# %createItem(linktext)
# If user can create an item in the current conference,
# display link to "create an item" page with 'linktext'.
eval $mac_define (createItem \
         \$includecml(\$(main)/createItem.i \$quote(@@)))


#@ <li><b>People</b>

# %person(userid [opt link text])
# Link pops-up window with info about 'userid'.
if $(use_javascript)
   eval $mac_define (person \
     \$ifthen (\$not_equal($userid() @1) \
                  <A HREF="javascript: pop_up('person', '@1');" \%help(h_name))\
     \$ifthen (\$equal($userid() @1) \
                  <A HREF=\%to_parent(me.cml?\$(nch)+\$(nxt)+\$arg(3)+\
                  \$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8)+@1) \
                  \%help(h_youare))\
     >\$ifthen (\$empty(@2) \%per_name(@1) \$if (\$(showUserid) &nbsp;(@1)))\
      \$ifthen (\$not_empty(@2) \$unquote(@2))</a>)
end
else
   eval $mac_define (person \
     \$ifthen (\$not_equal($userid() @1) \
        <a href="\$(mainurl)/person.cml?$unique()+$arg(2)+$arg(3)+$arg(4)+\
           \$arg(5)+\$arg(6)+\$arg(7)+\$arg(8)+@1" TARGET="_blank") \
     \$ifthen (\$equal($userid() @1) \
        <A HREF=\%to_parent(me.cml?\$(nch)+\$(nxt)+\$arg(3)+\
           \$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8)+@1) \
                  \%help(h_youare))\
     >\$ifthen (\$empty(@2) \%per_name(@1) \$if (\$(showUserid) &nbsp;(@1)))\
      \$ifthen (\$not_empty(@2) \$unquote(@2))</a>))
end

# %personSelf() 
# Link to one's own personal-profile page.
eval $mac_define (personSelf \
        <A HREF=\%to_parent(me.cml?\$(nch)+\$(nxt)+\$arg(3)+\
                \$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8)+@1) \
                \%help(h_youare)>\%per_name(\$userid())</a>)

#@</ol>

#----POWER
#@<p/>
#@<li><b>Power Users &amp; Managers</b><br>

#@<ol>
#@<li><b>"Go to" location - advanced</b><br>
#@ Use these in responses or other text to provide links to
#@ other conferences or locations.


# %url (actual_url, text of link)
# Pop-up link to an external URL 'actual_url'.
eval $mac_define (url  \%url2 (\%striplink(@@)))
eval $mac_define (url2 <A HREF="\%protocolOf(@@)://\%strip_http(@1)" \
           onClick="\%js_open_win(fromcau)" TARGET="fromcau"\
           >\$if (\$empty(@2) @1 \$rest(2 @@))</A>)

# %protocolOf(url)
# Extract just the protocol name (http or https) from 'url', even
# if 'url' has already been expanded into a clickable link by
# the richtext editor.
eval $mac_define (protocolOf \
        \$set(diagnostics at1='@@')\
        \$word (1 \
           \$ifthen(\$gt_equal (\$str_index(http://  @@) 0) http) \
           \$ifthen(\$gt_equal (\$str_index(https:// @@) 0) https) \
           http))

# %urlparams (actual_url, windowparams, text of link)
# Like %url, but you may specify the javascript-style window opening
# parameters in 'windowparams'.
eval $mac_define (urlparams  \%urlparams2 (\%striplink(@@)))
eval $mac_define (urlparams2 <a href="@1" \
          onClick="window.open ('', 'urlparams', '@2');   return true;" \
          target="urlparams">\$if (\$empty(@3) @1 \$rest(3 @@))</a>)

# FIX for individual item permissions.
# %last(conf_name inum optional_text)
#  Link to last response in conference 'conf_num' item 'inum'.
eval $mac_define (last \
   \$set (cthis \$cl_num(@1)) \
   \$if (\$less (\$group_access (\$userid() CONF \$(cthis)) $priv(minimum)) <U>@1</U> \
      \$set (lastr \$mac_expand(\%item_lastresp(\$item_id(\$(cthis) @2)))) \
      \$ifthen (\$empty(@3) \
         \%conference(\$cl_name(\$(cthis)) @2 \$(lastr))) \
      \$ifthen (\$not_empty(@3) <A HREF="\%_caucus_url(\$(cthis) @2 \
         \$(lastr))">\$rest(3 @@)</A>))\
)

# %conf_is_allowed (conf_name text)
# Evaluates to 'text' if current user is allowed in 'conf_name'.
eval $mac_define (conf_is_allowed \
        \$ifthen (\$gt_equal (\$group_access ($userid() CONF \$cl_num(@1)) 1) \$rest(2 @@)))

# %conf_has_new (conf_name text)
# Evaluates to 'text' if there is new material in 'conf_name'.
eval $mac_define (conf_has_new \
     \$ifthen (\$mac_expand(\%countNewInCnum(\$cl_num(@1))) \$rest (2 @@)))

eval $mac_define (conf_nonmember  0)

# %conf_link (conf_name text)
#    Evaluates to a link with 'text', that goes to 'conf_name'.
eval $mac_define (conf_link \
        <A HREF="\$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+x+x+x+x+x">\
        \$rest(2 @@)</A>)

# %conf_url (conf_name)
#    Evaluates to full URL for conference 'conf_name'.  Use
#    "/caucus/conf_name" instead unless contraindicated.
eval $mac_define (conf_url \
        \$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+x+x+x+x+x)

# FIX perms
# %conf_item_url (conf_name item)
#    Evaluates to full URL for conference 'conf_name', 'item'.  Use
#    "/caucus/conf_name/item instead unless contraindicated.
eval $mac_define (conf_item_url \
        \$(mainurl)/viewitem.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+\
        \$item_id(\$cl_num(@1) @2)+0+0+1+x#here)

# %conf_item_link (confname item linktext)
# Evaluates to a link with 'linktext', that goes to
# conference 'confname', item 'item'.
eval $mac_define (conf_item_link \$eval(\$cl_list(@1))\
        <a href="\$(mainurl)/viewitem.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+\
                 \$item_id(\$cl_num(@1) @2)+\
                 0+x+x+x#here">\$rest(3 @@)</a>)

# %conf_name_with_new (conf_name text)
#    "Convenience macro", combines other macros to show 'text' link to
#    conference 'conf_name' (if allowed), with a new icon (if anything is new).
eval $mac_define (conf_name_with_new \
        \%conf_is_allowed (@1 \%conf_link (@1 \$rest(2 @@) \
                              \%conf_has_new (@1 \%icon_new()))))
# %TMN_CONF_with_new (conf_name text)
# Same as %conf_name_with_new().
eval $mac_define (TMN_conf_with_new \
        \%conf_is_allowed (@1 \%conf_link (@1 \$rest(2 @@) \
                              \%conf_has_new (@1 \%icon_new()))))

# %conference_with_new (conf_name [optional text])
#    Link to conference 'conf_name', but with the new icon if anything is new.
#    If 'optional text' is supplied, shows that as link text instead of conference name
eval $mac_define (conference_with_new \
         \$set (cthis   \$word (1 \$cl_list(@1)))\
         \$set (ctnam   \$cl_name(\$(cthis)))\
         \$set (dtnam   \$if (\$empty(@2) \$(ctnam) \$rest(2 @@)))\
         \$set (_noacc  \$less (\$group_access ($userid() CONF \$(cthis)) 1))\
         \$ifthen (      \$(_noacc)    <u>\$(dtnam)</u>)\
         \$ifthen (\$not(\$(_noacc)) \
            <A HREF="\%_caucus_url(\$(cthis))" \
                 target="cmain$word (1 $replace (/ 32 $pid()))">\$(dtnam)</A>\
            \$ifthen (\$mac_expand(\%countNewInCnum(\$(cthis))) \%icon_new())))

# %conf_file_url (conf_name user item resp filename)
#    Full URL for file 'filename' uploaded by 'user' into 'conf_name', 'item', 'resp'.
eval $mac_define (conf_file_url \
        $(http)://$http_lib()/@1/@2/\%digit4(@3)\%digit4(@4)/@5)


# %conf_has_new_br (conf_name text)
#  Evaluates to 'text' (with a line break at the end)
#  if 'conf_name' has new material.
eval $mac_define (conf_has_new_br \
     \$ifthen (\$mac_expand(\%countNewInCnum(\$cl_num(@1))) \$rest (2 @@))<br>)

# %conf_link_br (conf_name_br text)
#  Evaluates to a link (with a line break at the end) 'text' to 'conf_name'
eval $mac_define (conf_link_br \
        <A HREF="\$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+x+x+x+x+x">\
        \$rest(2 @@)</A><BR>)

# %conf_name_with_new_br (conf_name text)
#    "Convenience macro", combines other macros to show link with 'text' to
#    conference 'conf_name' (if allowed), with a new icon (if anything is new) along
#    with a line break at the end of each line.
eval $mac_define (conf_name_with_new_br \
        \%conf_is_allowed (@1 \%conf_link (@1 \$rest(2 @@) \
                              \%conf_has_new_br (@1 \%icon_new()))))

# %anyConfHasNew(conf1 conf2 ...)
#  Evaluate to the new icon
#    if any of the confs 'conf1', 'conf2', etc. have new material.
eval $mac_define (anyConfHasNew \
     \$set(_anyconf @@)\
     \$includecml($(main)/anyconfhasnew.i))

# %countNewInCnum(cnum)
# Evaluates to the number of items with new material in conference 'cnum'.
eval $mac_define (countNewInCnum \$includecml($(main)/countNewInCnum.i @1))


# %center(text)
#   Link (with 'text') to Caucus Center
eval $mac_define (center <a href="\%_caucus_url()">\
          \$ifthen(    \$empty(@1)  \$(center_name))\
          \$ifthen(\$not_empty(@1)  @@)\
     </a>)

# %readrange (startitem enditem link_text)
#    Link to read items 'startitem' thru 'enditem' in current
#    conference.
eval $mac_define (readrange <A HREF="readrange.cml?\$arg(3)+@1+@2">\
                  \$rest(3 @@)</A>)

# %conf_has_no_new (conf_name text)
# Evaluates to 'text' if there is <b>no</b> new material for this user
# in 'conf_name'.
eval $mac_define (conf_has_no_new \
     \$ifthen (\$not(\$mac_expand(\%countNewInCnum(\$cl_num(@1)))) \$rest (2 @@)))

# %purchaseGroupMembership(groupType groupName paypalAccount price link-text)
# Pops open a window that allows the current user to purchase, through PayPal,
# membership in the specified Caucus group on this site.
# For example, %purchaseGroupMembership(Caucus4 myGroup roth@caucuscare.com 1.00 Buy Access)
# will display a link with text "Buy Access".  The price is $1.00, and completing the
# purchase will automatically add the current user to the "System" group "myGroup".
# If the user already has membership in that group, the link will NOT appear.
eval $mac_define (purchaseGroupMembership \
   \$ifthen (\$less (\$group_access($userid() @1 @2) 1) \
      <a href="purchaseGroupMembership.cml?\$unique()+call+@1+@2+@3+@4" \
         onClick="\%js_open_win(paypal)" TARGET="paypal" \
         >\$rest(5 @@)</a>\
   ) \
)

#@<p/>
#@<li><b>Interface Storage Conference</b><br>
#@ Every Caucus "interface" has a special conference that is used
#@ to help define the look and feel of the interface.
#@ (E.g. for interface CC51, the conference is called interface_storage_cc51.)
#@ This conference only appears to Caucus managers, it is otherwise
#@ invisible to regular users.
#@ The macros below reference items in this conference.

# %ifs_page_link (item resp text)
#    ("InterFace Storage PAGE LINK").
#   Evaluates to a link with 'text', that goes to
#   ifspage.cml (shows a response as a single
#    "resource" page), 'item':'resp' from the current "Interface Storage" conference.
eval $mac_define (ifs_page_link \
        <A HREF="\$(mainurl)/ifspage.cml?\$(nch)+\$(nxt)+x+x+x+x+@1+@2"> \
        \$rest(3 @@)</A> )

#---%pullMatchingWord1 (match text)
#   Returns 1st word of 'text' if match is initial substring of text.
eval $mac_define (pullMatchingWord1 \
   \$if (\$equal (0 \$str_index(@1 \$rest(2 @@))) \$word(2 @@) \$rest(2 @@)))

#---%stripMatchingWord1 (match text)
#   Returns everything *but* 1st word of 'text'
#   if match is initial substring of text.
eval $mac_define (stripMatchingWord1 \
   \$ifthen (    \$equal (0 \$str_index(@1 \$word(2 @@))) \$rest(3 @@))\
   \$ifthen (\$not_equal (0 \$str_index(@1 \$word(2 @@))) \$rest(2 @@))\
)

# %ifs_popup (item resp [width=nnn] [height=nnn] text)
#  Evaluates to a link with 'text', that
#    pops-up a new page, with contents of IFS ('item', 'resp') -- and nothing else!
#    If the optional width= and height= parameters appear (in that order!), they
#    control the size of the pop-up window.
eval $mac_define (ifs_popup \
   <a href="\$(mainurl)/ifspopup.cml?\$(nch)+\$(nxt)+x+x+x+x+@1+@2" \
      onClick="win = window.open('', 'ifs_popup', \
         'resizable=1,toolbar=1,scrollbars=1,menubar=1,location=1,status=1,\
         \%pullMatchingWord1(width= \$rest(3 @@)),\
         \%pullMatchingWord1(height= \%stripMatchingWord1(width= \$rest(3 @@)))'); \
         win.focus(); return true;" \
      target="ifs_popup" >\%stripMatchingWord1(height= \%stripMatchingWord1(width= \$rest(3 @@)))</a> )

# %ifs_resp (item [resp])
#   ("InterFace Storage RESPonse")
#    Evaluates to text of current Interface Storage conference 'item':'resp', interpreted
#    as CML.
eval $mac_define (ifs_resp \
        \$includecml($(main)/ifs_resp.cml \$(iface) @1 @2))

#@<li><b>People - Advanced</b><br>

# %personpop(userid)
#   Like %person(), but always the pop-up, never the edit-my-own-page.
#   No (uid) after the name, either.
eval $mac_define (personpop \
        <A HREF="javascript: pop_up('person', '@1');" \%help(h_name)>\
                  \%per_name(@1)</a>)

# %person_noclick (userid [text])
# Evaluates to a "do-nothing" link with either the person's name,
# or else 'text', as text. Clicking on the link does nothing.
eval $mac_define (person_noclick \
     \$ifthen (\$not_equal($userid() @1) \
                  <A HREF="#" onClick="return false;")\
     \$ifthen (\$equal($userid() @1) \
                  <A HREF="#" onClick="return false;")\
     >\$ifthen (\$empty(@2) \%per_name(@1))\
      \$ifthen (\$not_empty(@2) \$unquote(@2))\
      \$if (\$(showUserid) &nbsp;(@1))</A>)


# %mailto (email_address Name)
# Evaluates to a "mailto:" link using 'email_address',
# and 'Name' as the link text. Clicking on this
# link will automatically open most user's email client,
# preparing to send email to 'email_address'.
eval $mac_define (mailto <A HREF="mailto:@1" \
           >\$if (\$empty(@2) @1 \$rest(2 @@))</A>)

# %aim(handle)
#    hot link to AIM instant messenger for 'handle'
eval $mac_define (aim \
       \$ifthen (\$not_empty(@1) <A HREF="aim:goim?ScreenName=@1">@1</A>))
  

#--- %onnow (option_list)
# show table of current users.
#        See onnow.i for a list of valid options.
eval $mac_define (onnow     \$includecml($(main)/onnow.i \$arg(3) \$quote(@@)))

#    %onnow_check()
#       Calculate list of users "on now". Use this immediately
# before calling %onnow_link().
eval $mac_define (onnow_check \$includecml($(main)/onnow_calc.i))

#    %onnow_link()
#      link to page showing table of current users
eval $mac_define (onnow_link <a href="people_on.cml?\$(nch)+\$(nxt)+\
                     \$arg(3)+\$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8)+\
                     @1">@@</a>)

#    %onnow_user(uid)
#     shows "(now)" if uid is on now.
#    (Must call %onnow_check() once beforehand in page)
eval $mac_define (onnow_user  \$ifthen (\$tablefind (@1 \$(onnow_users)) (now)))


# %peoplelist(grouptype groupname $quote(fieldList) title)
# Display a list of all the people in 'grouptype', 'groupname'.
# 'fieldlist' is a space-separated list of the fields to be displayed,
# including: lname, fname, userid, laston, email.
# To turn off display of a field, prefix with "-", i.e. "-email"
# prevents displaying the email column.
# 'title' is the title displayed at the top of the list.
eval $mac_define (peoplelist \
     <a href="peoplelist.cml?\$(nch)+\$(nxt)+\$arg(3)+\$arg(4)+\$arg(5)+\
                             @1+@2+@3">\$rest(4 @@)</a>)

# %peoplelistURL(grouptype groupname $quote(fieldList))
# Same as %peoplelist(), except it evaluates to a URL to a page
# that just lists the people.
eval $mac_define (peoplelistURL \
     peoplelist.cml?\$(nch)+\$(nxt)+\$arg(3)+\$arg(4)+\$arg(5)+@1+@2+@3)

# %peoplegallery(grouptype groupname all width title)
# Display a rectangular "gallery" of the people in 'grouptype', 'groupname'.
# If 'all' is 1, display everyone in the group.  If 0, display only those
# people who have pictures. 'width' is the numeric value of the width
# of the page, e.g. 5 means show 5 people "across".
eval $mac_define (peoplegallery \
       <a href="peoplegallery.cml?\$(nch)+@1+@2+@3+@4+\
          \$str_replace($quote( ) \%01 \$rest(5 @@))+x+x" \
          onClick="\%js_open_win(gallery)" TARGET="gallery"\
                 >\$rest(5 @@)</a>)


# %groupMap (groupType groupName text)
# Evaluates to a link (with 'text') that pops-up a Google Maps page,
# showing the location of everyone in 'groupType', 'groupName'.
eval $mac_define (groupMap <a href="groupMap.cml?\$unique()+@1+@2" \
                   onClick="\%js_open_win(groupMap)" TARGET="groupMap"\
                       >\$rest(3 @@)</a>)

# %itemMap (itemLabels zoom text)
# Evaluates to a link (with 'text') that pops-up a Google Maps page,
# showing the location from which the items in itemLabels were written.
# ItemLabels is a comma-separated list or range of item labels.
# Zoom is the Google-maps zoom factor, typically 3-18.
# 0 defaults to a reasonable value.
eval $mac_define (itemMap <a href="itemMap.cml?\$unique()+\$(cnum)+@1+@2" \
         onClick="win = window.open('','groupMap',\
         'resizable=1,toolbar=1,scrollbars=0,menubar=1,location=1,status=1,width=720,height=470'); \
         win.focus(); return true;"  TARGET="groupMap"\
     >\$rest(3 @@)</a>)

# %itemMapEmbed (itemLabels zoom width height)
# Embeds a Google Maps page (in the current page),
# showing the location from which the items in itemLabels were written.
# ItemLabels is a comma-separated list or range of item labels.
# Zoom is the Google-maps zoom factor, typically 3-18.
# 0 defaults to a reasonable value.
# Width is the width of the map in pixels.
# Height is the height of the map in pixels.
eval $mac_define (itemMapEmbed \$includecml(\$(main)/itemMapEmbed.cml \$(cnum) @@))

# %itemMapFrame (itemLabels zoom width height)
# Embeds a Google Maps page (in the current page),
# showing the location from which the items in itemLabels were written.
# ItemLabels is a comma-separated list or range of item labels.
# Zoom is the Google-maps zoom factor, typically 3-18.
# 0 defaults to a reasonable value.
# Width is the width of the map in pixels.
# Height is the height of the map in pixels.
eval $mac_define (itemMapFrame \
   <iframe src="itemMapFrame.cml?\$unique()+\$(cnum)+@1+@2+@3+@4" \
           frameborder="0" width="\$plus(10 @3)" height="\$plus(10 @4)"></iframe>)


#eval $mac_define (js_open_sub   win = window.open ('', '@1',\
#    'resizable=1,toolbar=1,scrollbars=1,menubar=1,location=1,status=1,' + \



# %addUser2GroupOnClick (groupType groupName ifsItem text)
#   Evaluates to a link with 'text' to IFS page 'ifsItem'. When user clicks
#   through to the page, Caucus adds their userids to group
#   'groupType' 'groupName'.
eval $mac_define (addUser2GroupOnClick  \
     <a href="addUser2GroupOnClick.cml?\$unique()+@1+@2+@3">\$rest(4 @@)</a>)

#@<li><b>Images and Videos</b><br>

# %imagelink (imageURL targetURL [width [height]])
# Evaluates to an image-link: it shows the image at 'imageURL',
# clicking on the image pops-up a window showing the page at
# 'targetURL'.  The new window may be set to open with
# 'width' and 'height' (in pixels).
eval $mac_define (imagelink \
     <A HREF="@2" TARGET="_blank"><IMG SRC="@1" BORDER=0 \
                   \$ifthen (\$not_empty(@3) WIDTH=@3) \
                   \$ifthen (\$not_empty(@4) HEIGHT=@4)></A>)

# %image(URL [width height [alt_text]])
# Simply displays the image at 'URL', scaled to 'width' and 'height',
# and with optional 'alt_text' (what you see if image display is turned
# off, or if you hover over the image).
eval $mac_define (image  \%image2 (\%striplink (@@)))
eval $mac_define (image2 \
        <IMG SRC="@1" BORDER=0 \
             \$ifthen (\$not_empty(@3) WIDTH="@2" HEIGHT="@3") \
             \$ifthen (\$not_empty(@4) ALT="\$rest(4 @@)")>)


# %icon_new()
# Displays the "new" icon.
eval $mac_define (icon_new <IMG $(src_isnew) BORDER=0>)


# %photo (userid [options])
# Evaluates to a clickable image link of user 'userid',
# clicking pops-up the person page for 'userid'.
# 'Options' are HTML image options, such as "height=xx", etc.
eval $mac_define (photo \
   \$ifthen (\$not_empty (\%per_lname(@1)) \
      \$ifthen (\$not_empty (\$user_info (@1 picture)) \
         \%person(@1 \
         \$quote(<img src="\$(http)://\$user_info(@1 picture)" \$rest(2 @@) \
             border=0>)))))

# %picture(userid/filename)
# Displays image in Caucus site's public_html/LIB/PICTURES/'userid'/'filename'.
# Convenience macro for statically-supplied images.
eval $mac_define (picture <img src="/~$caucus_id()/LIB/PICTURES/@1">)

#
# %swfVideo (source width height)
#    Show an SWF ("flash") video embedded in the current window.
#    'source' is the URL of the video
#    width and height are specified in # of pixels.
eval $mac_define (swfVideo \
   <script src="/~$caucus_id()/SWFObject.js" type="text/javascript"></script>\
   <object id="FlashID" classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" width="@2" height="@3">\
     <param name="movie" value="@1" />\
     <param name="quality" value="high" />\
     <param name="wmode" value="opaque" />\
     <param name="allowFullScreen" value="true" />\
     <param name="swfversion" value="9.0.45.0" />\
     <!-- This param tag prompts users with Flash Player 6.0 r65 and higher to download the latest version of Flash Player. Delete it if you don&rsquo;t want users to see the prompt. -->\
     <!-- Next object tag is for non-IE browsers. So hide it from IE using IECC. -->\
     <!--[if !IE]>-->\
     <object type="application/x-shockwave-flash" data="@1" width="@2" height="@3">\
       <!--<![endif]-->\
       <param name="quality" value="high" />\
       <param name="wmode" value="opaque" />\
       <param name="swfversion" value="9.0.45.0" />\
       <param name="allowFullScreen" value="true" />\
       <!-- The browser displays the following alternative content for users with Flash Player 6.0 and older. -->\
       <div>\
         <h4>Content on this page requires a newer version of Adobe Flash Player.</h4>\
         <p><a href="http://www.adobe.com/go/getflashplayer"><img src="http://www.adobe.com/images/shared/download_buttons/get_flash_player.gif" alt="Get Adobe Flash player" width="112" height="33" /></a></p>\
       </div>\
       <!--[if !IE]>-->\
     </object>\
     <!--<![endif]-->\
   </object>\
   <script type="text/javascript">\
   <!--\
   swfobject.registerObject("FlashID");\
   //-->\
   </script>)

#@<li><b>My Conferences</b><br>
#  %myGroupConfs (groupType groupName showHeader text...)
#  Display a "My Conferences"-like list of conferences (with columns
#  for new material) that belong to group ('groupType', 'groupName').
#  If 'showHeader'=1, display the column headers.
#  'Text' is the optional "lead-in text" that appears at the head
#  of the list of conferences.
eval $mac_define (myGroupConfs \$includecml($(main)/myGroupConfs.cml \
        \$(nxt) @1 @2 @3 \$quote(\$rest(4 @@))))

#@<li><b>Integration With External Tools</b><br>
#   %watchitoo(meetingId [width height])
#   Shows a box 'width' by 'height' pixels that connects to the
#   Watchitoo.com video/chat/meeting server.  The Caucus configuration
#   file (swebd.conf) must have defined the 'config' parameters
#   watchitooVendorId and watchitooAccessToken, as supplied by
#   Watchitoo.
eval $mac_define (watchitoo \$includecml(\$(main)/watchitoo.i @@ 900 500))

#@</ol>

#@<p/>
#@<li><b>Internal Macros</b><br>

#@<ol>
#@<li><b>Windows</b><br>
#---Set delta height & width values for NN and IE -- use them to adjust
#   the size of newly opened windows (since the values that both NN and IE
#   return for the current window size is bogus in different ways).
set dh_ie $if ($(is_macintosh)  140    -50)
set dw_ie (-30)
set dh_nn (-140)
set dw_nn (-30)

#===Sub-macros, used by other macros:

# %js_open_win (name [opener])
# Javascript to open a new window.
#       if 'opener' is not empty, use opener of current window to
#       determine proper size of new window.
#@<pre>
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
#@</pre>

eval $mac_define (js_open_win \%js_open_sub (@@); $(winfocus)  return true; )

eval $mac_define (js_open_sub   win = window.open ('', '@1',\
    'resizable=1,toolbar=1,scrollbars=1,menubar=1,location=1,status=1,' + \
    \$ifthen ($(is_explorer5) 'screenX=0,screenY=0,' + )\
    \$set (ws \$word(1 \$user_data(\$userid() my_win_size) Default))\
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

# %js_open_ann()
# Open announcements window.
eval $mac_define (js_open_ann   win = window.open ('', '@1',\
    'resizable=1,scrollbars=1,' + \
    \$ifthen ($(is_explorer5) 'screenX=0,screenY=0,' + )\
    \$set (ws \$word(1 \$user_data(\$userid() my_win_size) Default))\
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

#   %userxs(popup defaultx)
# X Size of pop-up window.  (User vars are xs_@1, ys_@1.)
eval $mac_define (userxs \
     \$word (1 \$user_data ($userid() xs_@1) \$(site_xs_@1) @2))

#   %userys(popup defaulty)
# Y Size of pop-up window.  (User vars are xs_@1, ys_@1.)
eval $mac_define (userys \
     \$word (1 \$user_data ($userid() ys_@1) \$(site_ys_@1) @2))

# %to_parent (url)
# ilist.i  Redirect links to parent window if
#   $(target_switch) is true.
#   Usage: &lt;A HREF=%to_parent(url)&gt;
eval $mac_define (to_parent \
        \$ifthen (\$not(\$(target_switch)) "@1")\
        \$ifthen (\$(target_switch) "#" \
           onClick="window.opener.location.href = '@1';") )

#---%sane(argument)
#   Sanity testing for internal calls.  Make sure argument is > 2 chars long.
eval $mac_define (sane \$greater (\$strlen(@@) 2))

#---------------------------------------------------------------------
# "Built-in" macros.  These are special macros that *must* be defined,
# because calls to these macros are generated by 'C' code in the
# swebs CML interpreter.

# %_new_target_window(newx, newy)
#  (nee "%target" in 4.0).
#  Opens a new window to be used as a target for some URL.
#  If possible, makes new window 40 pixels less in x and y than
#  the current window.  For old browsers, the new window size is
#  newx x newy pixels.
#
#  Typically used in an &gt;A HREF&gt; tag to put the target URL in
#  a new window, as in:
#     &lt;A HREF="some_url" %_new_target_window(x, y)&gt;
 eval $mac_define (_new_target_window \$char(32)onClick="\
    win = window.open ('', 'fromcau', \
      'resizable=1,toolbar=1,scrollbars=1,menubar=1,location=1,status=1,' + \
    \$set (ws \$word(1 \$user_data(\$userid() my_win_size) Default))\
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

# %_target_fromcau(newx, newy)
#  Opens a window 'fromcau' to be used as a target for some URL.
#  If possible, makes new window 40 pixels less in x and y than
#  the current window.  For old browsers, the new window size is
#  newx x newy pixels.  Typically used with richtext editor URLs, as in:
#    &lt;a href="someurl" target="fromcau" onClick="%_target_fromcau(newx,newy);"&gt;

 eval $mac_define (_target_fromcau \
    win = window.open ('', 'fromcau', \
      'resizable=1,toolbar=1,scrollbars=1,menubar=1,location=1,status=1,' + \
    \$set (ws \$word(1 \$user_data(\$userid() my_win_size) Default))\
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
      ); $(winfocus)  return true; )


# %iframe (url pure [optwidth [optheight [options]]])
#    Display URL in an Iframe.  optWidth is optional width,
#    optHeight is optional height (pixels assumed unless other
#    units given).  Pure is always 1 (reserved for expansion).
#    Other options may be specified, see
#    http://www.w3schools.com/TAGS/tag_iframe.asp
eval $mac_define (iframe  \%iframe2 (\%striplink(@@)))
eval $mac_define (iframe2 \
  &nbsp;<IFRAME SRC="\$word(1 @@)" \
            WIDTH="\$word (1 \$word (3 @@) \$(default_iframe_width))"  \
           HEIGHT="\$word (1 \$word (4 @@) \$(default_iframe_height))" \
              \$rest(5 @@)>\
              <TABLE BORDER=2 CELLSPACING=5 CELLPADDING=5>\
              <TR><TD>\%url(\$word(1 @@))</TD></TABLE>\
        </IFRAME>)

#@<p/><li>
#@ <b>Miscellaneous I</b><br>

# %leave(URL)
#   Dummy macro, just expands to argument(s).
#   Provided so that display of new user registration "layout" page
#   appears to work like the real thing but doesn't terminate
#   Caucus process.
eval $mac_define (leave @@)

# %location(url)
# Replaces all uses of "Location:" directive
#       to go to another CML file.  Acts as an effective work-around for the
#       IE 4 bug (see http://gamgee.acad.emich.edu/~roth/ie_location.html).
#       If the URL has no #fragment, or is not IE 4, %location just spits
#       out the appropriate "Location:" directive.  Otherwise, it produces
#       a tiny HTML page that sets location.href to the proper url.

eval $mac_define (location \
    Pragma: no-cache$newline()\
    Expires: Thu, 01 Dec 1994 16:00:00 GMT$newline()\
    Location: @1\$newline()\$newline())

#------------------------------------------
#===Operational macros used inside Caucus.

# %chk(X)
#  Macro for cus_en*.cml.  If X is in table CHK,
#   output a "CHECKED" tag (for &lt;INPUT TYPE=checkbox...&gt; stuff).
eval $mac_define (chk \$if (\$tablefind (@1 \$(chk)) CHECKED))

# %en_var(user conf var)
#  Get value of e-mail notification variable.
eval $mac_define (en_var \
        \$ifthen (      @1  \$user_data (\$userid() @3_@2)) \
        \$ifthen (\$not(@1) \$site_data (@2 -         @3   )) )

# %set_en_var(user conf var values...)
#   Set value of e-mail notification variable.
eval $mac_define (set_en_var \
        \$ifthen (      @1  \$set_user_data (\$userid() @3_@2 \$rest(4 @@))) \
        \$ifthen (\$not(@1) \$set_site_data (@2 -         @3    \$rest(4 @@))) )

# %bad_email_address (x)
#   Bad if not empty and (no @, no ., has any of ";|&amp;,&lt;&gt;()", or &gt; 1 words).
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

# %nooplink(text)
#   Make a link with 'text' that does nothing.
#   Used for the last entry in the location (compass) bar.
eval $mac_define (nooplink \
           <A HREF="#" onClick="return false;" \%help(h_here)>@@</A>))

# %backlink(text)
#   Make a link with 'text' that does a "BACK".
eval $mac_define (backlink \
        <A HREF="#" \
           \$ifthen (\$(use_javascript) onClick="history.back();")\
        >@@</A>)

#@<p/><li>
#@<b>MSET, MPUSH, MPOP.</b><br>
#@  Handle "m" values (m_cnum, m_inum, m_rnum) <br>
#@  that are used in all of the %liburl2() derived macros for uploaded files.

# %mset (c i r)      Set values of m_cnum, m_inum, m_rnum that are
#   used in %liburl2 macros (when they are outside of their proper item)
eval $mac_define (mset \$set (m_cnum @1) \$set (m_inum @2) \$set (m_rnum @3))

# %clear(stackName)
eval $mac_define (clear \$set(@1))

# %push (stackName value)
eval $mac_define (push \$set(@1 @2 \$(@1)))

# %pop (stackName)
eval $mac_define (pop  \$word(1 \$(@1))\$set(@1 \$rest(2 \$(@1))))

# %top (stackName)
eval $mac_define (top  \$word(1 \$(@1)))


# %mpush (c i r)     Save current m values on a stack, then set new ones.
eval $mac_define (mpush \
        \$set (mstack \$(m_cnum) \$(m_inum) \$(m_rnum) \$(mstack))\
        \$set (m_cnum @1)  \$set (m_inum @2)  \$set (m_rnum @3)\
     )

# %mpop()            Pop m values off of stack.
eval $mac_define (mpop \
        \$set (m_cnum \$word (1 \$(mstack)))\
        \$set (m_inum \$word (2 \$(mstack)))\
        \$set (m_rnum \$word (3 \$(mstack)))\
        \$set (mstack \$rest (4 \$(mstack)))\
     )



# %_caucus_url(cnum inum rnum)
#  (nee "%special" in 4.0).
#  Translate 'http:/caucus/...' so-called "special" URLs into
#  URLs that reference the appropriate CML pages.
#  Note! inum is the item <b>label</b>, not the item_id.
# <p/>
#  http:/caucus              -> (goes to) the Caucus Center<br>
#        http:/caucus/conf         -> conference home page of 'conf'<br>
#        http:/caucus/conf/item    -> conference 'conf', item # 'item'<br>
#        http:/caucus/conf/item:r  -> 'conf' item 'item', response 'r'.<br>
eval $mac_define (_caucus_url \
        \$ifthen (\$empty(@1) \$(mainurl)/center.cml?\$(nch)+\$(nxt)+x+x+x+x+x+x)\
        \$ifthen (\$and (\$not_empty(@1) \$empty(@2)) \
           \$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+@1+x+x+x+x+x)\
        \$ifthen (\$and (\$not_empty(@1) \$not_empty(@2)) \
           \$(mainurl)/viewitem.cml?\$(nch)+\$(nxt)+@1+\$item_id(@1 @2)+@3+x+x+x#here)\
     )




#@<p/><li>
#@ <b>File upload macros.</b><br>
#@  These are internal macros that Caucus automatically
#@ generates when files are uploaded. The user is not supposed to use them,
#@ but they will see them briefly in the text of their response as they
#@ are writing the response.
#@  The "2" versions (liburl2, libimg2, etc.) are for 4.1+ and
#@  assume an "rdir" (item/response sub directory) that is keyed to the
#@  particular item and response numbers where the file was uploaded.

# %liburl2 (userid rdir file)   URL of file in library
# Build link to uploaded file in Caucus LIB directory.
# File is referenced by 'userid', filename 'file',
# and "magic" code 'rdir': 1=response, 0=body of item,
# qp=attached to quiz, ap=attached to assignment.
eval $mac_define (liburl2 $(http)://\%strip_http($http_lib()/)\
     \$ifthen (\$equal(@2 1)  ITEMS/\$(m_inum)/\$(m_rnum)/@3)\
     \$ifthen (\$equal(@2 0)  ITEMS/0/0/@3)\
     \$ifthen (\$equal(@2 qp) QUIZ/\$(pkey)/@3)\
     \$ifthen (\$equal(@2 ap) ASSIGN/\$(akey)/@3)\
  )

# %liburl_at (conf item resp file)
#   URL of 'file' in library in another conference 'conf', item 'item'.
eval $mac_define (liburl_at \$ifthen (\$word (1 \$cl_list(@1)) \
          $(http)://$http_lib()/@1/ITEMS/@2/@3/@4))

# %libimg(userid file)
#   IMG file in library (deprecated)
eval $mac_define (libimg <IMG SRC="\%liburl(@1 @2)">)

# %libimg2(userid rdir file [options])
#   IMG file in library
eval $mac_define (libimg2 <IMG SRC="\%liburl2(@1 @2 @3)" BORDER=0 \
                               \$rest(4 @@)>)

# %libimg2r(userid rdir file [options])
#  IMG file in library, right-aligned.
eval $mac_define (libimg2r <IMG SRC="\%liburl2(@1 @2 @3)" BORDER=0 align=right \
                                \$rest(4 @@)>)

# %libimg_at (conf item resp file [options])
eval $mac_define (libimg_at <IMG SRC="\%liburl_at(@1 @2 @3 @4)" BORDER=0 \
                                \$rest(5 @@)>)

# %libgallery(userid rdir file)
eval $mac_define (libgallery \$set(_gallery \$(_gallery) @1 @2 @3 0@4))

# %liblink(userid file)
#   link to file in library (deprecated)
eval $mac_define (liblink <A HREF="\%liburl(@1 @2)" \
                  onClick="\%js_open_win(fromcau)" TARGET="fromcau"\
                  >\%liburl(@1 @2)</A>)

# %liblink2(userid rdir file)
#   Link to file in library
eval $mac_define (liblink2 \
   <a href="\$mac_expand(\%liburl2(@1 @2 @3))" \
            \$mac_expand(\%_new_target_window(400 400))>\
            \$mac_expand(\%liburl2(@1 @2 @3))</a>)

# %libname(userid file name)
#   Named link to file in library (deprecated)
eval $mac_define (libname <A HREF="\%liburl(@1 @2)" \
                  onClick="\%js_open_win(fromcau)" TARGET="fromcau"\
                  >\$rest (3 @@)</A>)

# %libname2(userid rdir file name)
#   Named link to file in library
eval $mac_define (libname2 \
   <a href="\$mac_expand(\%liburl2(@1 @2 @3))" \
            \$mac_expand(\%_new_target_window(400 400))>\$rest(4 @@)</a>)


#@ <p/><li>
#@ <p/><b>MISC</b>

# %telnet (host_address Name)
eval $mac_define (telnet <A HREF="telnet://@1" \
	>\$if (\$empty(@2) @1 \$rest(2 @@))</A>)



# %re_text (cnum id rnum)
# Evaluates to raw text of item 'id' (internal item id), response 'rnum'.
eval $mac_define (re_text \
     \$unquote(\$sql_query_select(\
        SELECT text FROM resps WHERE items_id=@2 AND rnum=@3 AND deleted=0)))

#@<p/><li>
#@<b> Files and Filesafes</b>

# %files (name text)
# Not sure what this is.
set filesurl   $(http)://$host()/~$caucus_id()/FILES
eval $mac_define (files \%url(\$(filesurl)/@1 \
                        \$if (\$empty(@2) @1 \$rest(2 @@))))
eval $mac_define (files_url      $(filesurl)/@1)


# %filesafe (name [text])
#  Evaluates to link to filesafe file 'name'.
#  Use 'text' for link text if supplied.
#  (What the heck should 'name' be?)
eval $mac_define (filesafe \
   \$ifthen (\$and (\$group_access ($userid() Filesafe \$lower(@1)) \
                    \$not_empty (\$site_data(0 - FN_\$lower(@1)))) \
      <a href="\$(mainurl)/filesafe.cml?\$(nch)+\$(nxt)+\$arg(3)+\$arg(4)+\
               \$arg(5)+\$arg(6)+\$arg(7)+\$arg(8)+\$lower(@1)">\
                  \$ifthen (    \$empty(@2) \$upper1 (\$replace (_ 32 @1)))\
                  \$ifthen (\$not_empty(@2) \$rest (2 @@))</a>)\
)

# %filesafes (prefix)
#   List all viewable filesafes beginning with
#   'prefix'; if no 'prefix', list all filesafes.
eval $mac_define (filesafes \
        \$includecml (\$(iface)/main/filesafes.i \
           \$arg(2)+\$arg(3)+\$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8) @1))

# %filesafesec (name section [text])
# Evaluates to a link (with optional 'text') to a file safe section,
# for filesafe 'name', section 'section'.
eval $mac_define (filesafesec \
      \$includecml (\$(iface)/main/filesafesec.i 1 @1 @2 \
                    \$arg(3)+\$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8))\
      \$ifthen (    \$empty(@3) \$upper1(\$replace(_ 32 \$lower(@1) / @2))</a>)\
      \$ifthen (\$not_empty(@3) \$rest(3 @@)</a>)\
    )

# %filesafesecurl (name section)
# Evaluates to full URL to filesafe 'name', 'section'.
eval $mac_define (filesafesecurl \
       \$includecml (\$(iface)/main/filesafesec.i 0 @1 @2 \
                     \$arg(3)+\$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8))\
     )

# %filesafefile (name section file)
# Evaluates to a link to 'file' in filesafe 'name', 'section'.
eval $mac_define (filesafefile \
        \$includecml (\$(iface)/main/filesafefile.i 1 @1 @2 @3))

# %filesafefileurl (name section file)
# Evaluates to full URL to 'file' in filesafe 'name', 'section'.
eval $mac_define (filesafefileurl \
        \$includecml (\$(iface)/main/filesafefile.i 0 @1 @2 @3))

# %filesafethumb (name section filesmall filebig [width height])
# Evaluates to a clickable link that is (presumably) a small image
# file 'filesmall' in filesafe 'name', 'section'.&nbsp;
# Clicking on the link pops up a new window containing 'filebig'
# from the same filesafe section.  'Width' and 'height' are
# optional arguments to specify the new window size.
eval $mac_define (filesafethumb \
        <a href="\%filesafefileurl(@1 @2 @4)" \
           onClick="win = window.open ('', 'thumb', \
             'resizable=1,toolbar=0,scrollbars=1,menubar=0,location=0,\
              status=0\$ifthen (\$not_empty (@6) ,width=@5,height=@6)'); \
              $(winfocus) return true;" target="thumb" \
           ><img src="\%filesafefileurl(@1 @2 @3)" border=0></a>)

#@<p/><li>
#@ <b>MISC II</b>

# %login_gme (url text)
#   Example macro for how to integrate login with another web
#   application (in this case GroupMind Express).  See login_gme.cml
#   for more info.  Caller must supply login form submission
#   page as 'url', 'text' as text of link.
eval $mac_define (login_gme \
              <a href="\$(mainurl)/login_gme.cml?\$(nch)+\$(nxt)+@1" \
                 onClick="\%js_open_win(login_gme)" target="login_gme" \
                 >\$rest(2 @@)</a> )

# %preinc(x)
#   Pre-increment x.  Like C "++x"
eval $mac_define (preinc \$set (@1 \$plus (\$(@1) 1))\$(@1))

# %displayed_conf_name(cnum)
# Evaluates to displayed form of conference name for conference 'cnum'.
eval $mac_define (displayed_conf_name \$replace(_ 32 \$cl_name(@1)))

# %interface (dir [text])    Link to start using interface 'dir'.
# Evaluates to a clickable link (with optional 'text') that goes
# to (??), switching the user to interface 'dir'.
eval $mac_define (interface  \
   \$ifthen (\$exists ($caucus_path()/CML/@1/main) \
      <a href="interface.cml?\$unique()+@1">\
         \$if (\$empty(@2) @1 \$rest (2 @@))</a>)\
   \$ifthen (\$not (\$exists ($caucus_path()/CML/@1/main)) \
      <u>\$if (\$empty(@2) @1 \$rest (2 @@))</u>))

# %mainwindow (url text)
# Used in a pop-up window, displays a link with 'text'
# that redirects the main (opening) window to 'url'.
eval $mac_define (mainwindow \
     <a href="#" onClick="self.opener.location.href = '@1'">\$rest(2 @@)</a>)

# %flashchat(text)
# Pop-up link to a Flashcoms chat room (that is implicitly tied to the
# current conference), with linktext 'text'.
# Automatically signs user in, with current Caucus userid.
eval $mac_define (flashchat \
     <a href="flashchat.cml?\$(cnum)" target="_blank">@@</a>)

#---%flashlogs (room text)
# Link to 'list of flash chat logs' page
eval $mac_define (flashlogs \
   <a href="\$(mainurl)/flashList.cml?\$(nch)+\$(nxt)+\$arg(3)+\$arg(4)+\
            \$arg(5)+\$arg(6)+\$arg(7)+\$arg(8)+@1">\$rest(2 @@)</a>)

#---%countries()
# SELECT tag of countries
eval $mac_define (countries <select name="countries"> \
   <option>(choose one)</option> \
   <option>United States</option>  \
   <option>Canada</option>  \
   <option>Afghanistan</option>  \
   <option>Albania</option>  \
   <option>Algeria</option>  \
   <option>Andorra</option>  \
   <option>Angola</option>  \
   <option>Antigua + Barbuda</option>  \
   <option>Argentina</option>  \
   <option>Armenia</option>  \
   <option>Australia</option>  \
   <option>Austria</option>  \
   <option>Azerbaijan</option>  \
   <option>Bahamas</option>  \
   <option>Bahrain</option>  \
   <option>Bangladesh</option>  \
   <option>Barbados</option>  \
   <option>Belarus</option>  \
   <option>Belgium</option>  \
   <option>Belize</option>  \
   <option>Benin</option>  \
   <option>Bhutan</option>  \
   <option>Bolivia</option>  \
   <option>Bosnia + Herzegovina</option>  \
   <option>Botswana</option>  \
   <option>Brazil</option>  \
   <option>Brunei Darussalam</option>  \
   <option>Bulgaria</option>  \
   <option>Burkina Faso</option>  \
   <option>Burma (Myanmar)</option>  \
   <option>Burundi</option>  \
   <option>Cambodia</option>  \
   <option>Cameroon</option>  \
   <option>Cape Verde</option>  \
   <option>Central African Republic</option>  \
   <option>Chad</option>  \
   <option>Chile</option>  \
   <option>China</option>  \
   <option>Colombia</option>  \
   <option>Comoros</option>  \
   <option>Congo</option>  \
   <option>Costa Rica</option>  \
   <option>Cote d'Ivoire</option>  \
   <option>Croatia</option>  \
   <option>Cuba</option>  \
   <option>Cyprus</option>  \
   <option>Czech Republic</option>  \
   <option>Denmark</option>  \
   <option>Djibouti</option>  \
   <option>Dominica</option>  \
   <option>Dominican Republic</option>  \
   <option>Ecuador</option>  \
   <option>East Timor</option>  \
   <option>Egypt</option>  \
   <option>El Salvador</option>  \
   <option>England</option>  \
   <option>Equatorial Guinea</option>  \
   <option>Eritrea</option>  \
   <option>Estonia</option>  \
   <option>Ethiopia</option>  \
   <option>Fiji</option>  \
   <option>Finland</option>  \
   <option>France</option>  \
   <option>Gabon</option>  \
   <option>Gambia, The</option>  \
   <option>Georgia</option>  \
   <option>Germany</option>  \
   <option>Ghana</option>  \
   <option>Great Britain</option>  \
   <option>Greece</option>  \
   <option>Grenada</option>  \
   <option>Guatemala</option>  \
   <option>Guinea</option>  \
   <option>Guinea-Bissau</option>  \
   <option>Guyana</option>  \
   <option>Haiti</option>  \
   <option>Honduras</option>  \
   <option>Hungary</option>  \
   <option>Iceland</option>  \
   <option>India</option>  \
   <option>Indonesia</option>  \
   <option>Iran</option>  \
   <option>Iraq</option>  \
   <option>Ireland</option>  \
   <option>Israel</option>  \
   <option>Italy</option>  \
   <option>Jamaica</option>  \
   <option>Japan</option>  \
   <option>Jordan</option>  \
   <option>Kazakhstan</option>  \
   <option>Kenya</option>  \
   <option>Kiribati</option>  \
   <option>Korea, North</option>  \
   <option>Korea, South</option>  \
   <option>Kuwait</option>  \
   <option>Kyrgyzstan</option>  \
   <option>Laos</option>  \
   <option>Latvia</option>  \
   <option>Lebanon</option>  \
   <option>Lesotho</option>  \
   <option>Liberia</option>  \
   <option>Libya</option>  \
   <option>Liechtenstein</option>  \
   <option>Lithuania</option>  \
   <option>Luxembourg</option>  \
   <option>Macedonia </option>  \
   <option>Madagascar</option>  \
   <option>Malawi</option>  \
   <option>Malaysia</option>  \
   <option>Maldives</option>  \
   <option>Mali</option>  \
   <option>Malta</option>  \
   <option>Marshall Islands</option>  \
   <option>Mauritania</option>  \
   <option>Mauritius</option>  \
   <option>Mexico</option>  \
   <option>Micronesia</option>  \
   <option>Moldova</option>  \
   <option>Monaco</option>  \
   <option>Mongolia</option>  \
   <option>Montenegro</option>  \
   <option>Morocco</option>  \
   <option>Mozambique</option>  \
   <option>Myanmar</option>  \
   <option>Namibia</option>  \
   <option>Nauru</option>  \
   <option>Nepal</option>  \
   <option>The Netherlands</option>  \
   <option>New Zealand</option>  \
   <option>Nicaragua</option>  \
   <option>Niger</option>  \
   <option>Nigeria</option>  \
   <option>Norway</option>  \
   <option>Northern Ireland</option>  \
   <option>Oman</option>  \
   <option>Pakistan</option>  \
   <option>Palau</option>  \
   <option>Palestinian State</option>  \
   <option>Panama</option>  \
   <option>Papua New Guinea</option>  \
   <option>Paraguay</option>  \
   <option>Peru</option>  \
   <option>The Philippines</option>  \
   <option>Poland</option>  \
   <option>Portugal</option>  \
   <option>Qatar</option>  \
   <option>Romania</option>  \
   <option>Russia</option>  \
   <option>Rwanda</option>  \
   <option>St. Kitts + Nevis</option>  \
   <option>St. Lucia</option>  \
   <option>St. Vincent + The Grenadines</option>  \
   <option>Samoa</option>  \
   <option>San Marino</option>  \
   <option>Sao Tome + Principe</option>  \
   <option>Saudi Arabia</option>  \
   <option>Somalia</option>  \
   <option>Scotland</option>  \
   <option>Senegal</option>  \
   <option>Serbia</option>  \
   <option>Seychelles</option>  \
   <option>Sierra Leone</option>  \
   <option>Singapore</option>  \
   <option>Slovakia</option>  \
   <option>Slovenia</option>  \
   <option>Solomon Islands</option>  \
   <option>Somalia</option>  \
   <option>South Africa</option>  \
   <option>Spain</option>  \
   <option>Sri Lanka</option>  \
   <option>Sudan</option>  \
   <option>Suriname</option>  \
   <option>Swaziland</option>  \
   <option>Sweden</option>  \
   <option>Switzerland</option>  \
   <option>Syria</option>  \
   <option>Taiwan</option>  \
   <option>Tajikistan</option>  \
   <option>Tanzania</option>  \
   <option>Thailand</option>  \
   <option>Togo</option>  \
   <option>Tonga</option>  \
   <option>Trinidad + Tobago</option>  \
   <option>Tunisia</option>  \
   <option>Turkey</option>  \
   <option>Turkmenistan</option>  \
   <option>Tuvalu</option>  \
   <option>Uganda</option>  \
   <option>Ukraine</option>  \
   <option>United Arab Emirates</option>  \
   <option>United Kingdom</option>  \
   <option>Uruguay</option>  \
   <option>Uzbekistan</option>  \
   <option>Vanuatu</option>  \
   <option>Vatican City (Holy See)</option>  \
   <option>Venezuela</option>  \
   <option>Vietnam</option>  \
   <option>Western Sahara</option>  \
   <option>Wales</option>  \
   <option>Yemen</option>  \
   <option>Zaire</option>  \
   <option>Zambia</option>  \
   <option>Zimbabwe</option>  \
   </select>)

#---%states()
# SELECT tag of US states and Canadian provinces
eval $mac_define (states <select name="states"> \
   <option value="00">(none)</option> \
   <option value="AL">Alabama</option> \
   <option value="AK">Alaska</option> \
   <option value="AZ">Arizona</option> \
   <option value="AR">Arkansas</option> \
   <option value="AS">American Samoa</option> \
   <option value="CA">California</option> \
   <option value="CO">Colorado</option> \
   <option value="CT">Connecticut</option> \
   <option value="DE">Delaware</option> \
   <option value="DC">District of Columbia</option> \
   <option value="FM">Fed. States of Micronesia</option> \
   <option value="FL">Florida</option> \
   <option value="GA">Georgia</option> \
   <option value="GU">Guam</option> \
   <option value="HI">Hawaii</option> \
   <option value="ID">Idaho</option> \
   <option value="IL">Illinois</option> \
   <option value="IN">Indiana</option> \
   <option value="IA">Iowa</option> \
   <option value="KS">Kansas</option> \
   <option value="KY">Kentucky</option> \
   <option value="LA">Louisiana</option> \
   <option value="ME">Maine</option> \
   <option value="MD">Maryland</option> \
   <option value="MH">Marshall Islands</option> \
   <option value="MA">Massachusetts</option> \
   <option value="MI">Michigan</option> \
   <option value="MN">Minnesota</option> \
   <option value="MS">Mississippi</option> \
   <option value="MO">Missouri</option> \
   <option value="MT">Montana</option> \
   <option value="NE">Nebraska</option> \
   <option value="NV">Nevada</option> \
   <option value="NH">New Hampshire</option> \
   <option value="NJ">New Jersey</option> \
   <option value="NM">New Mexico</option> \
   <option value="NY">New York</option> \
   <option value="NC">North Carolina</option> \
   <option value="ND">North Dakota</option> \
   <option value="MP">Northern Marianas</option> \
   <option value="OH">Ohio</option> \
   <option value="OK">Oklahoma</option> \
   <option value="OR">Oregon</option> \
   <option value="PA">Pennsylvania</option> \
   <option value="PR">Puerto Rico</option> \
   <option value="RI">Rhode Island</option> \
   <option value="SC">South Carolina</option> \
   <option value="SD">South Dakota</option> \
   <option value="TN">Tennessee</option> \
   <option value="TX">Texas</option> \
   <option value="UT">Utah</option> \
   <option value="VT">Vermont</option> \
   <option value="VA">Virginia</option> \
   <option value="VI">(U.S.) Virgin Islands</option> \
   <option value="WA">Washington</option> \
   <option value="WV">West Virginia</option> \
   <option value="WI">Wisconsin</option> \
   <option value="WY">Wyoming</option> \
   <option value="AB">Alberta</option> \
   <option value="BC">British Columbia</option> \
   <option value="MB">Manitoba</option> \
   <option value="NB">New Brunswick</option> \
   <option value="NL">Newfoundland and Labrador</option> \
   <option value="NT">Northwest Territories</option> \
   <option value="NS">Nova Scotia</option> \
   <option value="NU">Nunavut</option> \
   <option value="ON">Ontario</option> \
   <option value="PE">Prince Edward Island</option> \
   <option value="QC">Quebec</option> \
   <option value="SK">Saskatchewan</option> \
   <option value="YT">Yukon Territory</option> \
</select>)



include $(main)/announcedef.i 0

#@ <p/><li>
#@ <b>Gradebook etc.</b><br>
#@   Macros added to support "gradebook", "assignments", and "symbols button"
#@   and related features.

# %is_class(cnum)
#   Is 'cnum' a "class" conference?
eval $mac_define (is_class  \
        \$and (\$not_equal (@1 x)  \
               \$(_supports_class) \
               \$equal (x\$site_data (@1 - confType) xCourse)))

# %supports_class()
#  Evaluates to 1 if this install support "class" (course) conferences,
#  else 0.
eval $mac_define (supports_class  0\$(_supports_class))

# %tab([x])
#   Evaluates to auto-incremented 'tabindex="n"', for use in
#   defining tabbing order for &lt;input&gt; objects.  %tab(x)
#   starts sequence over again at 'x'.  %tab() increments
#   current value of variable tabindex.
eval $mac_define (tab \
        \$set (tabindex \$word (1 @1 \$plus (\$(tabindex) 1)))\
        tabindex="\$(tabindex)")

# %fName (uid)
#   "Firstname" of userid 'uid'.
eval $mac_define (fName \$user_info (@1 fname))

# %lName (uid)
#   "Lastname" of userid 'uid'.
eval $mac_define (lName     \$user_info (@1 lname))

#   "Lastname" of userid 'uid'.
# %per_lname(uid)
eval $mac_define (per_lname \$user_info (@1 lname))

# %per_name(uid)
# Full name of user 'uid', replaces old $per_name().
eval $mac_define (per_name  \$user_info (@1 fname) \$user_info (@1 lname))

# %my_name()
# Full name of this user, replaces old $my_name().
eval $mac_define (my_name \$user_info(\$userid() fname) \
                          \$user_info(\$userid() lname))

# %my_phone()
# Phone number of current user, replaces old $my_phone().
eval $mac_define (my_phone \$user_info(\$userid() phone))

# %my_intro()
# Introductory text of current user, replaces old $my_intro().
eval $mac_define (my_intro \$user_info(\$userid() intro))

# %per_phone(uid)
# Phone number of user 'uid', replaces old $per_phone().
eval $mac_define (per_phone \$user_info (@1 phone))

# %per_laston(uid)
# Date/time user 'uid' was last on Caucus,
# replaces old $per_laston().
eval $mac_define (per_laston  \
     \$set(_laston \$user_info(@1 laston))\
     \$ifthen(\$not_equal (\$str_index(0000 \$(_laston)) 0) \
        \%epochOfyyyy (\$(_laston))))

# %assign(key text)
# Evaluates to a link with 'text', to assignment 'key'.
eval $mac_define (assign \
       <a href="\$(mainurl)/assignedit.cml?\$(nch)+\$(nxt)+\$arg(3)+x+@1+x+x+x"\
       >\$if (\$empty(@2) Assignment-@1 \$rest(2 @@))</a>)

#@<p/><li>
#@<b>Math symbol utility macros</b><br>
#@ For some reason, class="x" where x has text-decoration: none doesn't
#@  work in IE6.

#  %mathsym (symbolname [expand])
#  ??
eval $mac_define (mathsym  \
       \$set (_sym \$tablematch (@1 \$(symnam)))\
       \$ifthen (\$greater (\$(_sym) 0) \
          \$set(_symexp0 \$unquote (\$word ( 1 \$replace (, 32 \
                                    \$word (\$(_sym) \$(symval))))))\
          \$set(_symexp1 \$unquote (\$word (-1 \$replace (, 32 \
                                    \$word (\$(_sym) \$(symval))))))\
          <a href="#" style="text-decoration: none;" \
           \$ifthen (      0@2  onClick="return ebox_addtext('\$(_symexp1)');")\
           \$ifthen (\$not(0@2) onClick="return ebox_addtext('\\%sym(@1)');")\
                >\$(_symexp0)</a>))

# %sym()
# ??
eval $mac_define (sym \
       \$set (_sym \$tablematch (@1 \$(symnam)))\
       \$ifthen (\$greater (\$(_sym) 0) \
          \$unquote(\$word (-1 \$replace (, 32 \$word (\$(_sym) \$(symval)))))))

# %mathimg (imgblue [imgblack])
# ??
eval $mac_define (mathimg \
           \$quote(<img src=/~\$caucus_id()/MATHSYM/@1 border=0>)\
        \$ifthen (\$not_empty(@2) ,\
           \$quote(<img src=/~\$caucus_id()/MATHSYM/@2 border=0>)))

# %courselist(options)
#   Display list of courses.  See courselist.i for 'options'.
eval $mac_define (courselist   \$includecml(\$(main)/courselist.i @@))

# %no_such_conf (cnum)
# ??
eval $mac_define (no_such_conf \$empty (\$cl_name(@1)))

# %is_instructor()
#  Evaluates to 1
#   if current user is a manager, or an organizer or instructor of any conf?
eval $mac_define (is_instructor \$includecml(\$(main)/isinstructor.i))

# %listpeople (separator userid [userid2...])
# ??
eval $mac_define (listpeople  \
        \$includecml (\$(main)/listpeople.i  @1 \$quote(\$rest(2 @@))))

# %people(center linktext)
# ??
eval $mac_define (people \
   <a href="people.cml?\$(nch)+\$(nxt)+\$if(@1 x \$arg(3))+\$arg(4)+\$arg(5)+\
            \$arg(6)+\$arg(7)+\$arg(8)">\$rest(2 @@)</a>)

# %instructors (cnum)
#   List of userids of "instructors" of course conference 'cnum', in the order they appear
eval $mac_define (instructors \$includecml(\$(main)/instructors.i @1))

# %emImgSep (prop)
# ??
eval $mac_define (emImgSep %b(&nbsp;&nbsp;))

# %emFileSep (prop)
# ??
eval $mac_define (emFileSep %b(<p/>))

# %urcregnums()
#   Sorted list of field numbers of REG urc field variables, e.g.
#   REG_CC44/reg_1 is "1", etc.
eval $mac_define (urcregnums \$includecml(\$(main)/urcregnums.i))
# %onnow_options()
#   Static list of default options allowed for %onnow()
#   (Makes it easy to expand by putting definition in local_macros.)
eval $mac_define (onnow_options \
   summary now header name userid active e-mail indent)
# %intro(text)
# Evaluates to "useful" expansion of 'text', frequently
# used to format the "intro" text of a user.
eval $mac_define (intro \
     \$cleanhtml (tag_list \$protect(\$mac_expand(\$wrap2html(@@)))))
# %taskPriority(n)
#   Translates raw priority number 'n' to text (Low, medium, etc.)
eval $mac_define (taskPriority \$word (\$min(10 \$max(1 @1)) \
           &nbsp; &nbsp; Low &nbsp; Med &nbsp; High &nbsp; Urgent &nbsp;))
#            1      2     3    4     5    6     7     8      9      10
  

# %mgr_in_group (userid mgrgroups...)
#   Same as $mgr_in_group, but automatically includes systemAllPrivileges
#   (just to save us typing!)
eval $mac_define (mgr_in_group \$mgr_in_group (@@ systemAllPrivileges))

# %hasUpper (string)
#  Evaluates to 1 if 'string' contains any upper-case chars, else 0.
eval $mac_define (hasUpper \$not_equal (\$quote(\$lower(@@)) \$quote(@@)))


# %access(cnum [userid])
#   Returns access level granted to 'userid' in conference 'cnum'.  Replaces
#   old calls to $cl_access().  "Optional" access values are returned as 0!
eval $mac_define (access \
         \$set(_acc \$group_access (\$if (\$empty(@2) $userid() @2) CONF @1))\
         \$if (\$gt_equal (\$(_acc) $priv(minimum)) \$(_acc) 0))

# %lowname(cnum)
#  Evaluates to lower-case version of conference name of 'cnum'.
eval $mac_define (lowname \$lower(\$cl_name(@1)))

# %homepage()
# ??
eval $mac_define (homepage  \
     \$ifthen (\$not_empty(@1) \
        <A HREF="http://\%strip_http(@1)" \
           onClick="\%js_open_win(fromcau)" TARGET="fromcau"\
           >\$if (\$empty(@2) \%strip_http(@1) \$rest(2 @@))</A>))

# %briefintro()
# ??
eval $mac_define (briefintro \
            \$cleanhtml (prohibited \$protect(\$mac_expand(@@))))

# %item_lastresp(item_id)
# ??
eval $mac_define (item_lastresp  \
         \$sql_query_select(SELECT lastresp FROM items WHERE id=@1))

# %itemsUnder (cnum itemId)
eval $mac_define (itemsUnder \$sql_query_select (\
      SELECT items_id FROM conf_has_items \
       WHERE cnum=@1 AND items_parent=@2 AND deleted=0 \
       ORDER BY item0 DESC, item1 DESC, item2 DESC, item3 DESC, item4 DESC, item5 DESC))

# %item_title(item_id)
# Evaluates to title of item 'item_id'.
eval $mac_define (item_title \$replace(10 32 \$includecml($(main)/itemTitle.i @1)))
  

# %acceptTerms(item)
# ??
eval $mac_define (acceptTerms \
   Click <a href="\$(http)://$host()/\$(reg)/swebsock/\$pid()/\$(iface)/main/\
          ifspopupNologin.cml?\$unique()+0+x+x+x+x+@1+0" \
          onClick="\%js_open_win(ifs_popup)" target="ifs_popup" \
                 >here</a> to see the terms of registration. )

#@<p/><li>
#@ <b>Quizzes</b><br>

# %quiz_summary(itemlabel)
#   Link to a quiz summary, item number (label) 'itemlabel'.
eval $mac_define (quiz_summary  \$includecml($(main)/quiz_summary.i \
   \$quote(\$(nch)+\$arg(2)+\$arg(3)+\$arg(4)+\$arg(5)+\$arg(6)+$arg(7)+$arg(8)) @1))

# %quiz_problem_summary(itemlabel)
# ??
eval $mac_define (quiz_problem_summary \
   \$includecml($(main)/quiz_problem_summary.i \
   \$quote(\$(nch)+\$arg(2)+\$arg(3)+\$arg(4)+\$arg(5)+\$arg(6)+$arg(7)+$arg(8)) @1))

#@<li><b>Filters and general text manipulation</b><br>

# %digit4(number)
# Pad 'number' with leading zeroes, to make a 4 digit number.
eval $mac_define (digit4 \$str_sub(0 \$minus(4 \$strlen(@1)) 000)@1)

# %digit8(number)
# Pad 'number' with leading zeroes, to make an 8 digit number.
eval $mac_define (digit8 \$str_sub(0 \$minus(8 \$strlen(@1)) 0000000)@1)

#--- %strip_http (url)    strips off leading "http://" if present
#eval $mac_define (strip_http \
#       \$if (\$equal (\$str_index (http:// @1) 0) \
#          \$str_sub (7 2000 @1) \
#          @1)\
#     )

eval $mac_define (strip_http \
        \$word (1 \
           \$ifthen (\$equal(\$str_index(http://  @1) 0) \$str_sub(7 2000 @1)) \
           \$ifthen (\$equal(\$str_index(https:// @1) 0) \$str_sub(8 2000 @1)) \
           @1))

# %word_remove (n list)
# Evaluates to a 'list' of words, but with word number 'n' removed.
eval $mac_define (word_remove  \
       \$words (1            \$minus(@1 1)  \$rest(2 @@)) \
       \$words (\$plus(@1 1) 100000         \$rest(2 @@)))
  
# %word_insert (n word list)
# Evaluates to a 'list' of words, but with 'word' inserted at position 'n'.
eval $mac_define (word_insert \
       \$words (1            \$max(0 \$minus(@1 1))  \$rest(3 @@)) @2 \
       \$words (\$max(1 @1)  100000                  \$rest(3 @@)))


# %reducedChars(text)
# Evaluates to 'text', with
# everything translated to lower case, and blanks and non-printable
# chars removed.
eval $mac_define (reducedChars \$replace(10 32 \$includecml($(main)/reducedChars.i \$quote(@@))))

# %makeLegal (str)
# Evaluates to 'str', translated to a legal CDATA form
# that HTML form name= fields will accept.
eval $mac_define (makeLegal \$replace([ _ \$replace(] _ @@)))

# %safename(text)
# Evaluates to 'text', but with non-printing chars (including blanks) replaced with "_".
eval $mac_define (safename  \$includecml($(main)/safename.i \$quote(@@)))
  

# %striplink(string)   "Strip" the &lt;A HREF=...&gt;url&lt;/a&gt; away from the
#   start of 'string', leaving just the "url" part, and anything
#   following after the &lt;A HREF&gt;...&lt;/A&gt;.  Frequently used in
#   other macros that take a URL as an argument, but may (due to
#   the quirky nature of the richtext editor) find that they are
#   getting a full &lt;A HREF&gt; tag set instead.
eval $mac_define (striplink \
   \$set (_a0 \$str_index (<a   \$lower(@@)))\
   \$set (_a1 \$str_index (</a> \$lower(@@)))\
   \$set (_a  \$and (\$not(\$(_a0)) \$greater (\$(_a1) 0)))\
   \$ifthen (\$(_a) \
      \$unhtml(, \$str_sub (0 \$(_a1) @@)) \
      \$str_sub (\$plus (4 \$(_a1)) 1000000 @@))\
   \$ifthen (\$not(\$(_a))  @@))


#  %stripquotes(text)
# Strips leading and trailing double quotes from 'text'.
eval $mac_define (stripquotes \
     \$set (gotquotes \
        \$and (\$equal (\$str_sub (0                        1 @@) ") \
               \$equal (\$str_sub (\$minus (\$strlen(@@) 1) 1 @@) ")))\
     \$ifthen (\$(gotquotes) \$str_sub (1 \$minus (\$strlen(@@) 2) @@))\
     \$ifthen (\$not(\$(gotquotes)) @@))\
   )

# %nbsp(text)  translate all blanks in 'text' into &nbsp;'s.
eval $mac_define (nbsp \$str_replace($char(4) &nbsp; \$replace(32 04 @@)))


# %trimtext (chars text)
#   Display 1st 'chars' of 'text'.  If not all of 'text' is displayed,
#   add a "..." on the end to indicate text was trimmed.
eval $mac_define (trimtext    \$str_sub (0 @1 \$rest(2 @@))\
                  \$if (\$less (@1 \$strlen(\$rest (2 @@))) ...))

# %rtrim (text)
#   "Right trim" 'text', i.e. remove all trailing blanks.
#   Note: we need to remove trailing newline generated by $includecml()!
eval $mac_define (rtrim \
                  \$set(_rtrim \$includecml(\$(main)/rtrim.i \$quote(@@)))\
                  \$str_sub (0 \$minus(\$strlen(\$(_rtrim)) 1) \$(_rtrim)))

# %tripletOf(cnum word)
# Evaluate to the conf/itemId/response triplet of "word", which
# can be an item:response label, or the keyword "last", or anything
# that $item_parse() can handle.
eval $mac_define (tripletOf \$includecml(\$(main)/tripletOf.i @1 @2))

#@<li><b>SQL</b><br>

# %sql(query)
#   Simple execution of SQL 'query'.  For non-SELECTs only.  We keep _q, _h in case
#   we ever need to debug this.
eval $mac_define (sql \$set (_q @@)\
                      \$set (_h \$sql_query_open(@@))\
                      \$eval(\$sql_query_close(\$(_h))))

# %sql1row(query)
#   Simple execution of SQL 'query'.  For one-rows SELECTs only.  We keep _q, _h
#   in case we ever need to debug this.
eval $mac_define (sql1row \$set (_q @@)\
                          \$set (_h \$sql_query_open(@@))\
                          \$eval(\$sql_query_row(\$(_h)))\
                          \$eval(\$sql_query_close(\$(_h))))

#@<li><b>Date &amp; Time</b><br>

# %epoch_of (x)
#   Returns epoch time.  X can either be an epoch time number, or
#   a date string (such as produced by $dateof()).  Used when forcing
#   a possibly unknown format into epoch time.  Returns 0 if X is empty.
eval $mac_define (epoch_of \$if (\$empty(@1) 0 \$if (\$max(@1) @1 \$epoch(@1) )))

# %datetz ( date | time)
#   Translate a 2-word date (e.g. the output from $dateof()) or a 1-word
#   time (e.g. the output from $time()) into the $dateof() format
#   for the current timezone, as defined by $(timezonehrs).
#   E.g. if the server local time is EST, and $(timezonehrs) is set to -3,
#   then %datetz (23-MAY-2003 11:00) becomes "23-MAY-2003 8:00".
 eval $mac_define (datetz \
         \$ifthen(\$plus(0 \$word(1 \$replace(- 32 @1))) \
           \$set (_dt \$replace (- 32 \
              \$dateof (\$plus ( \$mult(3600 \$(timezonehrs)) \
                 \$if (\$not_equal (\$sizeof(@@) 1)  \$epoch(@1 @2)   @@) \
               )))\
           )\
           \$upper1(\$lower(\$word (2 \$(_dt)))) \
           \$word(1 \$(_dt)), \
           \$rest(3 \$(_dt)) \
        ))


# %yyyymmddOf (date)
#   Convert common 'date' formats to yyyy-mm-dd.
eval $mac_define (yyyymmddOf  \
        \$includecml (\$(main)/yyyymmddof.i @@))

# %epochOfyyyy (date)
#   Converts yyyy-mm-dd 'date' to epoch times.
eval $mac_define (epochOfyyyy \
        \$ifthen (\$empty(@1) 0 )\
        \$ifthen (\$not_empty(@1) \
           \$epoch(\
              \$str_sub(8 2 @1)-\
              \$word (\$str_sub (5 2 @1) \
                 jan feb mar apr may jun jul aug sep oct nov dec)-\
              \$str_sub(0 4 @1) @2)))

# %dateOfyyyy (date)
# Convert yyyy-mm-dd 'date' to... ?
eval $mac_define (dateOfyyyy \
        \$str_sub(8 2 @1)-\
        \$word (\$str_sub (5 2 @1) \
           jan feb mar apr may jun jul aug sep oct nov dec)-\
        \$str_sub(0 4 @1))

# %monthDayYear (date)
#   Convert yyyy-mm-dd 'date' to "Mon dd, yyyy".
eval $mac_define (monthDayYear  \
        \$ifthen (\$and (\$not_empty(@1)  \$not_equal (@1 0000-00-00)) \
           \$set (temp \$replace(- 32 @1))\
           \$word (\$word (2 \$(temp)) \
                  Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec) \
           \$word (3 \$(temp)), \$word (1 \$(temp))))

# %monthDayYearHour(date)
#   Convert yyyy-mm-dd hh:mm:ss 'date' to "Mon dd, yyyy hh:mm".
eval $mac_define (monthDayYearHour \
     \$set(_mdyh \$mac_expand(\%monthDayYear(@1)))\
     \$ifthen (\$not_empty(\$(_mdyh)) \$(_mdyh) \$str_sub(0 5 @2)))

# %monthDayYearHourTZ(date)
#   Adjust a "yyyy-mm-dd hh:mm" 'date' by the number of hours in $(timezonehrs),
#   and then display it as "Mon dd, yyyy hh:mm".
eval $mac_define (monthDayYearHourTZ \
     \$mac_expand (\%monthDayYearHour( \
     \$yyyymmddOf (\$plus (\$epochOfYYYY(@@) \$mult(3600 \$(timezonehrs)))))))

# %monthDay(date)
# Evaluate to day of month(?) of 'date'.
set thisYear $word (3 $replace (- 32 $dateof($time())))
eval $mac_define (monthDay \
        \$set(_mdy \$mac_expand(\%monthDayYear(@1)))\
        \$set(_mde \$equal (\$word(3 \$(_mdy)) $(thisYear)))\
        \$ifthen (      \$(_mde)  \$replace(, 32 \$words (1 2 \$(_mdy))))\
        \$ifthen (\$not(\$(_mde)) \$(_mdy)))

# %monthDayHour(datetime)
# ??
eval $mac_define (monthDayHour \
        \$set(_mdy \$mac_expand(\%monthDayYear(@1)))\
        \$set(_mde \$equal (\$word(3 \$(_mdy)) $(thisYear)))\
        \$ifthen (      \$(_mde)  \$words (1 2 \$(_mdy)))\
        \$ifthen (\$not(\$(_mde)) \$(_mdy)) \$str_sub(0 5 @2))

# %openElgg (elggUrl link-text)
# Experimental!&nbsp;
# Displays a link.  Clicking on the link opens a new
# window, and automatically (SSO) logs in to Elgg, with
# the Caucus userid -- but it only works if the Caucus userid
# is an OpenId(!!).&nbsp;
# 'elggUrl' is the URL of Elgg installation, minus the leading http://
eval $mac_define (openElgg \
   <a href="\$(mainurl)/openElgg.cml?\$unique()+@1" target="Elgg">\$rest(2 @@)</a>)

# %frameElgg (elggUrl topSizeInPixels link-text)
# Experimental!&nbsp;
# Displays a link.&nbsp;
# Clicking on the link goes to a page with the usual Caucus toolbar
# in a top frame, and Elgg in the lower frame.&nbsp;
# 'elggUrl' is the URL of Elgg installation, minus the leading http://.&nbsp;
# 'topSizeInPixels' is the size of the top frame.
eval $mac_define (frameElgg \
   <a href="\$(mainurl)/frameElgg.cml?\$unique()+@1+@2">\$rest(3 @@)</a>)

#@</ol>
