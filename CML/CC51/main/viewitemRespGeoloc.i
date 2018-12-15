      "&nbsp;
      if $not_equal (none $word (1 $(respLatLong) none))
         "<span style="position: relative;">
         "   <span style="position: absolute; top: -16px">
         "      <a href="editRespLatLongOnMap.cml?$unique()+$(vi_inum)+$(rnum)+$(safeToEdit)+$(respLatLong)" 
         "         onClick="return openEditMap();"   TARGET="editMap"
         "          ><img $(src_globe) border="0"></a>&nbsp;&nbsp;
         "   </span>
         "</span>
      end
