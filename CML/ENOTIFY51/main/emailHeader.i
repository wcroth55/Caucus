#
#---emailHeader.i 

eval $output ($(tfile) 644)
if $not ($(trusted_user))
   set orgId $co_org($(cnum))
   set name  $user_info($(orgId) fname) $user_info($(orgId) lname)
#  "From: $(name) <tmn@c5.caucus.com>
   "From: $(name) <$caucus_id()@$config(mailhost)>
   "Reply-To: $(org_mail)
end
"Subject: $(cname) conference: new material
"To: $user_info ($(uid) email)
"
"Dear $user_info ($(uid) fname) $user_info ($(uid) lname) ($(uid)):
"
"There is new material in the conference "$(cname)"
set loginurl $site_data ($(cnum) - en_loginurl)
if $not_empty ($(loginurl))
   "at: $(loginurl)?location=/$cl_name($(cnum))
end
elif $equal (x$(atdir) xCC51)
   "at: $(http)://$host()/$(swebentry)/$cl_name($(cnum))
end
else
   "at: $(http)://$host()/$(swebentry)/@$(atdir)/$cl_name($(cnum))
end
"
"You can read all of this material immediately by going to this link:
if $not_empty ($(loginurl))
   "    $(loginurl)?location=$cl_name($(cnum))/new
end
else
   "    $(http)://$host()/$(sweb)/$(swebsock)/0/0/$(dir)/anew.cml?\
        0+0+$(cnum)
end
