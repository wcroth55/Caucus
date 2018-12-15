#
#---methumb.i   Create the thumbnail image for a user.
#
#   inc(1)  userid
#   inc(2)  original picture
#
#:CR 02/20/06 New file.
#-------------------------------------------------------------------------

if $not_empty ($config(convertDir))
   set  thumbNailSize $word (1 $(default_thumbnail) 60x40)
   set  tmpjpg /tmp/caucus.$caucus_id().$inc(1).jpg
   eval $xshell (1 1 1 $config(convertDir)/convert  \
        $caucus_lib()/PICTURES/$escsingle($inc(1)/$inc(2)) \
        -resize $(thumbNailSize) $(tmpjpg))
   eval $copy2lib ($(tmpjpg) PICTURES/$inc(1)/_thumb.jpg)
   eval $delfile  ($(tmpjpg))
   eval $set_user_data ($inc(1) thumbNailSize $(thumbNailSize))
end
