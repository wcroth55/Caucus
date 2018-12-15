#
#---mod_chatmac.i   Definition of babylon 'chat' macro.
#
#: CR 07/04/03 New file -- put chat in 'mod' option format.
#: CR 07/16/03 Add %chatlogs()
#: CR 08/18/03 Add %chatlog()
#------------------------------------------------------------------------

set mod_chat 1

#---%chatlogs (room text)   Link to 'list of chatlogs' page
eval $mac_define (chatlogs \
   <a href="\$(mainurl)/chatlist.cml?\$(nch)+\$(nxt)+\$arg(3)+\$arg(4)+\
            \$arg(5)+\$arg(6)+\$arg(7)+\$arg(8)+@1">\$rest(2 @@)</a>)

#---%chatlog (room date time text)
eval $mac_define (chatlog   \$includecml (\$(main)/chatlog.i \
        \$arg(3)+\$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8)  @@))

#---%chattool (room options text)
eval $mac_define (chattool  \$includecml (\$(main)/chattool.i \$arg(3) @@))

#---Make obsolete %babylon() build on %chattool().
eval $mac_define (babylon   \%chattool (@1 x \$rest (2 @@)))

#---Experimental %chatter() -- link to chat and chat log, and
#   make room name based on site, conf, or item.
eval $mac_define (chatter   \$set (_rm \
     \$ifthen (         \$numeric (\$arg(3)) \
       \$ifthen (       \$numeric (\$arg(4))  \$cl_name(\$arg(3))-item\$arg(4))\
       \$ifthen (\$not (\$numeric (\$arg(4)))  0))\
     \$ifthen (  \$not (\$numeric (\$arg(3)))  site-\$(iface)))\
     \$includecml (\$(main)/chattool.i \$arg(3) \$(_rm) x Chat)\
      &nbsp;&nbsp;&nbsp;\
     \$includecml (\$(main)/chatlog.i \
        \$arg(3)+\$arg(4)+\$arg(5)+\$arg(6)+\$arg(7)+\$arg(8) \
        \$(_rm) 0 0 Chat-log))
     
#------------------------------------------------------------------------
