#
#---browser_detection.i    Separate out browser/platform detection.
#
# Copyright (C) 1995-2006 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 05/02/2004  New file.  Included from startup.i
#: CR 08/02/2004  Set mozilla_version.
#: CR 02/13/2006  Add IE7 detection.
#==============================================================
#
#---BROWSER: Which browser is the user running?
#   Sets BROWSER_NAME to name and version (e.g. "Netscape 3.03")
#   Sets IS_MACINTOSH to 1 if user runs on a mac
#   Sets the following to 1 or 0 as appropriate.
#      IS_NETSCAPE5  Mozilla  5 or higher (aka Netscape 6)
#      IS_NETSCAPE4  Netscape 4 or higher
#      IS_NETSCAPE3  Netscape 3 or higher
#      IS_NETSCAPE2  Netscape 2 or unknown browser
#      IS_EXPLORER7  Internet Explorer 7.0 or higher
#      IS_EXPLORER55 Internet Explorer 5.5 or higher
#      IS_EXPLORER5  Internet Explorer 5.0 or higher
#      IS_EXPLORER4  Internet Explorer 4.0 or higher
#      IS_EXPLORER3  Internet Explorer 3.0 or higher
#
#   Sets USE_BUTTON to 1 if only buttons (and not <A HREF="#">'s) may have
#   onClick events (netscape 2 and ie 3).
#
#   Sets WINFOCUS to "win.focus();" for browsers that properly support it
#   (Netscape 3 under Linux does not for some reason.)
#
set browser_name (Unknown)
set is_safari     0
set is_netscape5  0
set is_netscape4  0
set is_netscape3  0
set is_netscape2  0
set is_explorer7  0
set is_explorer55 0
set is_explorer5  0
set is_explorer4  0
set is_explorer3  0
set icon_type    gif
set mozilla_version 0

set is_macintosh $greater ($str_index (Mac $http_user_agent()) 0)

set browser      $str_index (MSIE $http_user_agent())
if $greater ($(browser) -1)
   set version       $str_sub   ($plus ($(browser) 5) 1 $http_user_agent())
   set versionx      $str_sub   ($plus ($(browser) 5) 3 $http_user_agent())
   set is_explorer3  $greater   ($(version) 2)
   set is_explorer4  $greater   ($(version) 3)
   set is_explorer5  $greater   ($(version) 4)
   set is_explorer55 $or ($greater ($(version) 5) $equal ($(versionx) 5.5))
   set is_explorer7  $greater   ($(version) 6)
   set browser_name  $str_sub   ($(browser) 15 $http_user_agent()) ;
   set semicolon     $str_index (; $(browser_name))
   set browser_name  $str_sub   (0 $(semicolon) $(browser_name))
   if $(is_explorer7)
      set icon_type png
   end
end

else
   set browser  $str_index (Mozilla/ $http_user_agent())
   if $greater ($(browser) -1)
      set version $str_sub ($plus ($(browser) 8) 1 $http_user_agent())
      set is_netscape3 $greater ($(version) 2)
      set is_netscape4 $greater ($(version) 3)
      set is_netscape5 $greater ($(version) 4)
      set browser_name Mozilla \
                       $str_sub ($plus($(browser) 8) 4 $http_user_agent())
#     set browser_name Netscape \
#                      $str_sub ($plus($(browser) 8) 4 $http_user_agent())

      #---Check for Mozilla rv number:
      set temp $str_index (rv: $http_user_agent())
      if $greater ($(temp) -1)
         set temp $word (1 $str_sub ($plus ($(temp) 3) 100 \
                           $replace (41 32 $http_user_agent())))
         set browser_name Mozilla $(temp)
         set temp $replace (. 32 $(temp))
         set mozilla_version $word(1 $(temp)).$word(2 $(temp))
      end

      if $(is_netscape5)
         set icon_type png
      end
   end

   #---Netscape 6 breaks the http user agent version # rules... again.
   set net6     $str_index (Netscape6/ $http_user_agent())
   if $greater ($(net6) -1)
      set browser_name Netscape \
                       $str_sub  ($plus($(net6) 10) 20 $http_user_agent())
      set icon_type png
   end

   #---Netscape 7 breaks the http user agent version # rules... again again!
   set net7     $str_index (Netscape/7 $http_user_agent())
   if $greater ($(net7) -1)
      set browser_name Netscape \
                       $str_sub  ($plus($(net7)  9) 20 $http_user_agent())
      set icon_type png
   end
end

#---Detect Opera and report it.
set opera $str_index (Opera $http_user_agent())
if $greater ($(opera) -1)
   set browser_name Opera $word (1 \
                          $str_sub ($plus($(opera) 6) 40 $http_user_agent()))
   set icon_type png
end

#---Detect Safari and report it.
set safari $str_index (Safari $http_user_agent())
if $greater ($(safari) -1)
   set is_safari 1
   set browser_name Safari $word (1 \
                         $str_sub ($plus ($(safari) 7) 3 $http_user_agent()))
end

#---Detect Firefox.
set firefoxPos $str_index(Firefox/ $http_user_agent())
if $greater ($(firefoxPos) 0)
   set browser_name $word(1 $str_sub($(firefoxPos) 1000 $http_user_agent()))
   set browser_name $replace(/ 32 $(browser_name))
end

set chromePos $str_index(Chrome/ $http_user_agent())
if $greater ($(chromePos) 0)
   set browser_name $word(1 $str_sub($(chromePos) 1000 $http_user_agent()))
   set browser_name $replace(/ 32 $(browser_name))
end

#---Ancient browser support.  Netscape 2 & IE 3 use buttons instead of
#   links for many things (e.g. confhome.cml column headers)
set is_netscape2 $not ($or ($(is_explorer3) $(is_explorer4) $(is_netscape3)))
set use_button $or ($(is_netscape2) \
                    $and ($(is_explorer3) $not ($(is_explorer4))) )

#---Unclear if this should be used or not -- see macros.i
#   Suggests that netscape 3 on unix doesn't have win.focus, but it does?!
set winfocus  win.focus();
if $and ($(is_netscape3)  $not($(is_netscape4))  \
         $greater ($str_index (x11 $lower($http_user_agent())) -1) )
   set winfocus
end

set use_resize $or ($(is_netscape4) $(is_explorer4))

#---Default new window size (Netscape 3 only)
eval $new_win (600 450)

#---Determine PLATFORM while we're at it.  Macintoshes get special coding.
set platform (?)
set mac_pos    $str_index (Mac     $http_user_agent())
if   $greater ($str_index (PPC     $http_user_agent()) 0)
   set platform Mac PPC
end
elif $greater ($str_index (PowerPC $http_user_agent()) 0)
   set platform Mac PPC
end
elif $and ($greater ($(mac_pos) 0)  \
           $equal   ($charval ($plus($(mac_pos) 3) $http_user_agent()) 32))
   set platform Macintosh
end
elif $greater ($str_index (Windows_NT_6.1 $replace(32 _ $http_user_agent())) 0)
   set platform Windows 7
end

#---For everything else, the platform is (usually) the 3rd semi-colon
#   separated token in the "(...)" part of the HTTP user agent string.
#   If empty, or equal to "Nav", use the 1st token.
else
   set lparen $str_index ($char(40) $http_user_agent())
   set rparen $str_index ($char(41) $http_user_agent())
   if $greater ($(lparen) 0)
      set lparen    $plus ($(lparen) 1)
      set useragent $str_sub ($(lparen)  $minus ($(rparen) $(lparen)) \
                             $http_user_agent())
      set useragent $replace (32 01 $(useragent))
      set useragent $replace (;  32 $(useragent))
      set platform  $word (1 $word (3 $(useragent)) $word (1 $(useragent)))
      if $equal ($(platform) Nav)
         set platform $word (1 $(useragent))
      end
      set platform  $replace (01 32 $(platform))

      if $equal ($word (2 $(platform)) NT)
         set platform $(platform) 4.0
         set add_on $tablefind ($word(3 $(platform)) 5.1 5.0 4.0)
         set platform $word ($plus($(add_on) 1) \
            (?) Windows&nbsp;XP Windows&nbsp;2000 Windows&nbsp;NT&nbsp;4.0)
      end
      elif $greater ($str_index (Win$char(1)9x $(useragent)) 0)
         set platform Windows ME
      end
      else
         set platform $word (1 $(platform)) $word (2 $(platform))$(add_on)
      end
      if $equal ($(platform) WinNT)
         set platform NT 4.0
      end
   end
end
