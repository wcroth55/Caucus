#
#--- watchitoo.i   Implementation of %watchitoo() macro.
#
#    1 meeting id
#    2 width
#    3 height
#-------------------------------------------------------------

"<script type="text/javascript" 
"   src="http://www.watchitoo.com/swf/watchitooShowEmbed.php?playerid=WatchitooPlayer&tp=1&scale=false&width=$inc(2)&height=$inc(3)&layout=11"></script>

"<script type="text/javascript">
"   function OnWatchitooPlayerEvent (playerID, eventName, eventParams) {
"      if (eventName == "ready") {
"         var initializeParams = 
"            { displayName: "$user_info ($userid() fname) $user_info ($userid() lname)" };
"         var watchitooplayer = document.getElementById("WatchitooPlayer");
"         watchitooplayer.Initialize("$config(watchitooVendorId)", 
"            "$config(watchitooAccessToken)", "$inc(1)", initializeParams);
"      }
"   }
"</script>
