#
#---NAV_ITEM.   Navigation bar for items
#
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 10/13/01 21:31 Add print-entire-item button (based on Gabor's work)

set args $arg(1)+$arg(2)+$arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)


#---print button
"<A HREF="$(mainurl)/viewitema.cml?$(nch)+$(nxt)+\
            $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)+$arg(9)"
"   onMouseOver="window.status='Display printer-friendly page.'; return true;" 
"   onMouseOut="window.status=''; return true;" 
"   TARGET="new"><img $(src_print) alt="Printer-friendly page"></a>

#---expand button
if $or ($(page_after) $(page_before))
   "<A HREF="viewexpa.cml?$(nch)+$(nxt)+\
         $arg(3)+$arg(4)+$arg(5)+$arg(6)+$arg(7)+$arg(8)"
        "%help(h_expa)><IMG $(src_expand)></A>
end
else
   "<A HREF="viewitem.cml?$(args)#here" 
   "%help(h_expa0 false)><IMG $(src_expand0)></A>
end

#---previous page button

if $(page_before)
   set rp  $max (0 $minus ($(rstart) 1) )
   set rs  $max (0 $minus ($(rstart) $(cur_pageresps)) )

   "<A HREF="viewitem.cml?$(nch)+$(nxt)+\
             $arg(3)+$arg(4)+$(rp)+$(rs)+$arg(7)+$arg(8)#here"
   "%help(h_prev)><IMG $(src_prevpg)></A>
end
else
   "<A HREF="viewitem.cml?$(args)#here"
   "%help(h_prev0 false)><IMG $(src_prevpg0)></A>
end

#---first page button

if $(page_before)
   "<A HREF="viewitem.cml?$(nch)+$(nxt)+\
             $arg(3)+$arg(4)+0+0+$arg(7)+$arg(8)#here"
   "%help(h_1st)><IMG $(src_1stpg)></A>
end
else
   "<A HREF="viewitem.cml?$(args)#here"
   "%help(h_1st0 false)><IMG $(src_1stpg0)></A>
end

#---last page button

if $(page_after)
   set rs $plus (1 $minus (%item_lastresp($(vi_inum)) $(cur_pageresps)))
   set rs $max  ($(rs) $plus ($(rstop) 1) )

   "<A HREF="viewitem.cml?$(nch)+$(nxt)+\
             $arg(3)+$arg(4)+$(rs)+$(rs)+$arg(7)+$arg(8)#here"
      "%help(h_last)><IMG $(src_lastpg)></A>
end
else
   "<A HREF="viewitem.cml?$(args)#here"
   "%help(h_last0 false)><IMG $(src_lastpg0)></A>
end

#---next page button

if $(page_after)
   set rs $plus ($(rstop) 1)
   "<A HREF="viewitem.cml?$(nch)+$(nxt)+\
             $arg(3)+$arg(4)+$(rs)+$(rs)+$arg(7)+$arg(8)#here"
   "%help(h_next)><IMG $(src_nextpg)></A><IMG 
   "    SRC="$(img)/clearpix.gif" WIDTH=1 HEIGHT=1>
end
else
   "<A HREF="viewitem.cml?$(args)#here"
   "%help(h_next0 false)><IMG $(src_nextpg0)></A><IMG 
   "    SRC="$(img)/clearpix.gif" WIDTH=1 HEIGHT=1>
end

#==============================================================
