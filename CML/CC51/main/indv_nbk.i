#
#---INDV_NBK.I   Add checked responses to category.
#                Then close current (pop-up) window.
#
#  Input:
#    $form(category)    name of category
#    $form(checklist)   checklist of responses
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/18/02 %displayed_conf_name() calculates displayable conference name.
#-----------------------------------------------------------------------


for r in $form(checklist)
   set ititle %item_title($(iinum))
   set where  %displayed_conf_name ($(icnum)) \
             Item $item_label($(icnum) $(iinum)), $(ititle)
   if $(r)
      set where $(where), Response $(r)
   end

   eval $set_user_data ($userid() mark_$(category) \
            $user_data ($userid() mark_$(category)) \
            viewitem.cml 0+$(icnum)+$(iinum)+$(r)+x+1+x #here \
            $quote($(where)) 0)
end

"Content-type: text/html
"
"<HTML>
"<HEAD>
"<TITLE> ...Working... </TITLE>
"<script type="text/javascript" language="javascript">
    include $(main)/clearchk.js
"   window.opener.pop_gone('indv');
"   window.close();
"</SCRIPT>
"</HEAD>

"<BODY $(body_bg) >
"<CENTER><H2>...Working...</H2></CENTER>
"</div>
"</body>
"</HTML>
#-----------------------------------------------------------
