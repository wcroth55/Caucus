#
#---confGreeting.i  Show "conference Greeting", i.e. the top of the
#   conference home page, before we get to the item selection or
#   item list display.
#
#   inc(1)  conference number
#
#: CR 12/03/10 Add [caucusscript] filtering hack.
#-------------------------------------------------------------------

#---Display conference greeting, if any, applying appropriate filtering.

if $(variants)
   "<table WIDTH="100%">
   "<TR VALIGN=top>

   "<TD>&nbsp;&nbsp;&nbsp;</TD>
   "<TD>
end

set prohibited $(tag_list)
if $not_equal   (x$site_data($inc(1) - tag_list_org) x)
   set prohibited $site_data($inc(1) - tag_list_org)
end

set m_inum 0
"<table class="itemlist_table" border=0 >
"<tr><td>
   set tp $site_data ($inc(1) - greet_tp)
   
   set itemMapCount 0
   if $equal ($(tp) cml)
      "$str_replace([caucusscript] script $cleanhtml(prohibited $protect(\
              $mac_expand($reval($cleanhtml (_empty $site_data($inc(1) - greet1)))))))
      "<BR CLEAR=ALL>
   end
   
   elif $equal ($(tp) html)
      "$str_replace([caucusscript] script $cleanhtml(prohibited $protect(\
                     $mac_expand($cleanhtml (_empty $site_data($inc(1) - greet1))))))
      "<BR CLEAR=ALL>
   end
   
   elif $equal ($(tp) literal)
      "<PRE>
      "$cleanhtml (prohibited \
          $protect($mac_expand($t2html($site_data($inc(1) - greet1)))))
      "</PRE>
   end
   
   elif $equal ($(tp) format)
      "$str_replace([caucusscript] script $cleanhtml (prohibited \
           $protect($mac_expand($wrap2html($site_data($inc(1) - greet1))))))
   end
   
   else
      "This is the central page for the <B>$(cname)</B> conference.
   end
   set itemMapCount 0
"</td>
"</table>
