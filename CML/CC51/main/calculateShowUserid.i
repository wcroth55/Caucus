#
#--- calculateShowUserid.i 
#
#    Sets showUserid to indicate whether the "(userid)" after a name
#    should be displayed.
#
#: CR 08/25/2009
#---------------------------------------------------------------------

set showUserid $site_data (0 - if_$(iface)_showUserid)
if $empty ($(showUserid))
   set showUserid $(show_id)
end
