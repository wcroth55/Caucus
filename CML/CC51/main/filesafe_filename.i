#
#---filesafe_filename.i
#
#   Included by filesafe.cml, to show file or resource name/link.
#
# CR 10/04/07.  Separated into include file.
# CR 05/07/17.  Just open new tab, don't mess with opening completely new window.
#-------------------------------------------------------------------------

         #---File name
         if $(is_link)
            set y $readfile ($(fdir)/$(x))
            set slash2 $str_index (// $(y))
            set z      $str_sub   ($plus ($(slash2) 2) 1000 $(y))
            if $greater ($strlen($(y)) 20)
               "<td colspan=5><font face="arial" size="2"
               "   ><a href="$(y)" target="_blank"
               "  onClick="fill_in (document.$(vname), '$(y)', '$escsingle($(desc))', '$escsingle($(keyw))'); "
               "   >$(z)</a>&nbsp;</font></td>
               if $(descFirst)
                  "<tr bgcolor="$(color)"><td colspan=4>&nbsp;</td>
               end
               else
                  "<td>&nbsp;</td></tr>
                  "<tr bgcolor="$(color)"><td>&nbsp;</td>
               end
            end
            else
               "<td><font face="arial" size="2"
               "   ><a href="$(y)" target="_blank"
               "  onClick="fill_in (document.$(vname), '$(y)', '$escsingle($(desc))', '$escsingle($(keyw))'); "
               "   >$(z)</a>&nbsp;</font></td>
            end
         end
         else
            "<td><font face="arial" size="2"
            "><a href="$(http)://$http_lib()/FILESAFE/$(vname)/$(x)"
            "  target="_blank"
            "  onClick="fill_in (document.$(vname), '$(x)', '$escsingle($(desc))', '$escsingle($(keyw))'); "
            ">$(x)</a>&nbsp;</font></td>
         end
