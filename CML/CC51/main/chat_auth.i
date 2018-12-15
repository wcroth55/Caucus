#
#---chat_auth.i   include file for chat CML files to force babylon
#   authentication.
#
#   inc(1)   "user name"
#
#: CR 08/14/03 New file.
#: CR 08/22/03 Use full name instead of userid.
#-----------------------------------------------------------------

if $empty ($(babylon_passwd))
   set babylon_passwd $random (1000000)
   eval $xshell (0 1 1 \
           $caucus_path()/SCRIPTS/babylon.sh $config(java) \
           $caucus_path()/public_html/BABYLON  \
           "$replace(32 01 $inc(1))" $(babylon_passwd))
end
