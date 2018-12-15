#
#---googleMaps.i
#
#   Support code for google maps interfaces.
#-------------------------------------------------------------------

"   function initialize() {
"      if (GBrowserIsCompatible()) {
"        map = new GMap2(document.getElementById("map_canvas"));
"        map.setCenter(new GLatLng(usersLat, usersLng), zoom);
"        map.addControl(new GSmallZoomControl());
"        map.addControl(new GMapTypeControl());
"        icon = new GIcon();
"        addrpnt = new GMarker(new GLatLng(usersLat, usersLng), {draggable: true, title: "title"  }) ;
"        addrpnt.enableDragging() ;
"        map.addOverlay(addrpnt) ;
"        GEvent.addListener(addrpnt,'dragend',function() {newpoint()}) ;
"      }
"    }
