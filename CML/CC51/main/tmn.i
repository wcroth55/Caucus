#---TMN.I    Define old "TMN" Caucus macros for this interface.
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/18/02 %displayed_conf_name() calculates displayable conference name.
#: CR  4/13/04 Use $co_priv() for checking %access() priv levels.
#: CR 10/25/05 %goto_new_resp OK if access > 0
#==============================================================
#

eval $mac_define (showicon <IMG SRC="\$(icon)/@1" WIDTH="@2" HEIGHT="@3" \
   BORDER="0" ALT="\$rest(4 @@)">)

eval $mac_define (TMN_icon <IMG SRC="\$(TMN_icon)/@1" WIDTH="@2" HEIGHT="@3" \
   BORDER="0" ALT="\$rest(4 @@)">)

eval $mac_define (TMN_link_aux \
  <A HREF="\$(mainurl)/TMN_\$(main)/aux.cml?\$(nch)+\$(nxt)+x+x+x+x+@1+@2"> \
  \$rest(3 @@)</A> \
)

eval $mac_define( TMN_conf \
  <A HREF="\$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+x+x+x+x+x">\
  \$rest(2 @@)</A> \
)

#eval $mac_define( TMN_conf_with_new \
#  <A HREF="\$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+x+x+x+x+x">\
#  \$rest(2 @@)\
#  \$if( \
#    \$not( \
#      \$or( \
#        \$it_inew(\$cl_num(@1)) \
#        \$it_iunseen(\$cl_num(@1)) \
#        \$it_wnew(\$cl_num(@1)) \
#      ) \
#    ) \
#    </A> \
#    \%showicon(inew.gif 23 12 New)</A> \
#  ) \
#)

eval $mac_define( TMN_conf_with_new_as_listitem \
 <LI><A HREF="\$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+x+x+x+x+x">\
  \$rest(2 @@) \
  \$if( \
    \$not( \
      \$or( \
        \$it_inew(\$cl_num(@1)) \
        \$it_iunseen(\$cl_num(@1)) \
        \$it_wnew(\$cl_num(@1)) \
      ) \
    ) \
    </A> \
    \%showicon(inew.gif 23 12 New)</A> \
  )</LI> \
)

eval $mac_define( TMN_conf_if_allowed_with_new_as_listitem \
  \$if(\$not(\%access(\$cl_num(@1))) <!> \
 <LI><A HREF="\$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+x+x+x+x+x">\
    \$rest(2 @@) \
    \$if( \
      \$not( \
        \$or( \
          \$it_inew(\$cl_num(@1)) \
          \$it_iunseen(\$cl_num(@1)) \
          \$it_wnew(\$cl_num(@1)) \
        ) \
      ) \
      </A> \
      \%showicon(inew.gif 23 12 New)</A> \
    )</LI> \
  ) \
)

eval $mac_define( TMN_if_allowed \
  \$if(\$not(\%access(\$cl_num(@1))) <!> \
    \$rest(2 @@) \
  ) \
)

eval $mac_define( TMN_conf_if_allowed_with_new \
  \$if(\$not(\%access(\$cl_num(@1))) <!> \
    <A HREF="\$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+x+x+x+x+x">\
    \$rest(2 @@) \
    \$if( \
      \$not( \
        \$or( \
          \$it_inew(\$cl_num(@1)) \
          \$it_iunseen(\$cl_num(@1)) \
          \$it_wnew(\$cl_num(@1)) \
        ) \
      ) \
      </A> \
      \%showicon(inew.gif 23 12 New)</A> \
    ) \
  ) \
)

eval $mac_define(TMN_conf_imap_area \
  <AREA SHAPE="@2" COORDS="@3"$newline()\
HREF="\$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+x+x+x+x+x"$newline()\
  ALT="\$rest(4 @@)">\
)

eval $mac_define (TMN_conflink \
   <A \%confhomehref(@1)>\$rest(2 @@)\
   \$if( \
      \$not( \
         \$or( \
            \$it_inew(\$cl_num(@1)) \
            \$it_iunseen(\$cl_num(@1)) \
            \$it_wnew(\$cl_num(@1)) \
         ) \
      ) \
      </A> \
      \%showicon(inew.gif 23 12 $quote(New))</A> \
   ) \
)

#eval $mac_define (TMN_show_text_storage_text \
#      \$cleanhtml(prohibited \$protect(\$mac_expand(\$re_text( 2 @1 @2)))) \
#)

#---%TMN_ts_img(userid file)   IMG file in library
eval $mac_define (TMN_ts_img \
  <IMG SRC="$http_lib()/\%lowname(\$(TMN_text_storage))/@1/@2)"> )


eval $mac_define(slide \
  <CENTER><IMG SRC="http://\$host()/~\$caucus_id()/\$(main)/slides/@1" \
  WIDTH="580" HEIGHT="460" BORDER="0"></CENTER> )

eval $mac_define( MiscImg \
  <CENTER><IMG SRC="http://\$host()/~\$caucus_id()/\$(main)/misc/@1" \
  BORDER="0"></CENTER> )

eval $mac_define( TMN_show_work \
  <A HREF="\$(mainurl)/work.cml?\$(nch)+\$(nxt)+@1+@2+@3+\$arg(6)+\$arg(7)+showpagename">\
  \$rest(4 @@)</A>\
)

eval $mac_define( TMN_conf_href \
  \$(mainurl)/confhome.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+x+x+x+x+x \
)

eval $mac_define( TMN_item_href \
  \$(mainurl)/viewitem.cml?\$(nch)+\$(nxt)+\$cl_num(@1)+@2+0+0+1+x#here \
)

#---%goto_new_resp (conf item text)
#   Looks like "go to new response in conf/item, else goto end of item. 
eval $mac_define(goto_new_resp \
   \$set(macCNum \$word(1 \$cl_list(@1))) \
   \$set(macCName \%displayed_conf_name (@1)) \
   \$if(\$not(\$group_access ($userid() CONF \$(macCNum))) \
      <U></U> \
      \$set(macNewR \$it_newr(\$(macCNum) @2)) \
      \$set(macLastR \$it_lastresp(\$(macCNum) @2))\
   ) \
   \$ifthen(\$gt_equal(\$(macNewR) \$(macLastR)) \
      \$ifthen(\$empty(@3) \
         \%conference(\$(macCName) @2 \$(macLastR))\
      ) \
      \$ifthen(\$not_empty(@3) \
         <a href="\%_caucus_url(\$(macCNum) @2 \$(macLastR))">\$rest(3 @@)</a>\
      ) \
   )\
   \$ifthen(\$less(\$(macNewR) \$(macLastR)) \
      \$ifthen(\$empty(@3) \
         \%conference(\$(macCName) @2 \$(macNewR))\
      ) \
      \$ifthen(\$not_empty(@3) \
         <a href="\%_caucus_url(\$(macCNum) @2 \$(macNewR))">\$rest(3 @@)</a>\
      ) \
   )\
)


#===================================================================
