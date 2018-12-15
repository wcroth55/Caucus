#
#---loginbox.i      Show new (4.4) login box, proceed to log user in,
#                   and go on to 
#   called from startup.i
#
# Copyright (C) 1995-2005 CaucusCare.com.  All rights reserved.
# This program is published under the terms of the CDDL; see
# the file license.txt in the same directory.
#
#: CR 08/23/02 New file.
#: CR 10/13/04 Use percentage font sizes.
#: CR 02/28/06 Add favicon.
#==========================================================================

set clearpix $(img)/clearpix.gif

"Content-type: text/html
"Pragma: no-cache
"Expires: Thu, 01 Dec 1994 16:00:00 GMT
"
"<html>
"<head>
"<title>Caucus Log In</title>
"<link rel="icon"          href="$(http)://$host()/~$caucus_id()/favicon.ico" 
"     type="image/x-icon" >
"<link rel="shortcut icon" href="$(http)://$host()/~$caucus_id()/favicon.ico" 
"     type="image/x-icon" >

"<style type="text/css">
"   td.medium     { font-family: verdana, arial;  font-size: 80%;}
"   td.mediumedge { font-family: verdana, arial;  font-size: 80%;
"                   background-color: #0070FF;}
"   td.small      { font-family: verdana, arial;  font-size: 60%;}
"   td.edge       { background-color: #0070FF; }
"     .tiny       { font-family: verdana, arial;  font-size: 60%;}
"     .nav        { text-decoration: none ;       color: blue }
"     .nav:hover  { text-decoration: underline;   color: blue }
"</style>

"<script type="text/javascript" language="javascript">
"   document.cookie = "caucus_ver=xyz";

"   function cookies_allowed() {
"      if (document.cookie != null  &&  document.cookie.length > 0) return true;

"      var cookie = window.open ("$(http)://$host()/~$caucus_id()/nocookie.html", 
"         "nocookie", 
"         "toolbar,location,status,menubar,scrollbars,resizable," +
"         "width=600,height=400");

"      cookie.focus();
"      return false;
"   }
"</script>

"</head>

"<body bgcolor="#A0A0A0" onLoad="document.clogin.userid.focus();">
"<center>
"
"&nbsp;<br>
"<form method="post" name="clogin"   onSubmit="return cookies_allowed();"
"      action="$(http)://$host()/$(sweb)/swebsock/$pid()/$(main)/loginbox.cml">

#---Target where the user is really trying to get to (cml file & args)
"<input type="hidden" name="target" 
"      value="$word (7 $replace (/ 32 $request_uri()))">
"
"<table border=0 cellspacing=0 cellpadding=0 bgcolor="#E0E0EF">
"
"<tr class="edge" valign="middle">
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"<td class="mediumedge"><img src="$(clearpix)" width=1 height=4 alt=""
"    ><br>&nbsp;&nbsp;&nbsp;&nbsp;<br
"    ><img src="$(clearpix)" width=1 height=4 alt=""></td>
"<td class="mediumedge" colspan=2
"   ><b><font color=white>Caucus Log In</font></b></td>
"<td class="mediumedge">&nbsp;</td>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"
"<tr>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"<td colspan=4><img src="$(clearpix)" width=1 height=7 alt=""></td>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"<tr>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"<td class="small">&nbsp;</td>
"<td class="small">&nbsp;User ID<br>
"    &nbsp;<input type="text"     size="16" name="userid" tabindex="1"
"             onChange="cookies_allowed();"></td>
"<td valign=top align=right colspan=2 rowspan=3 class="medium"
"   ><a href="http://caucuscare.com" target="_blank"
"   ><img src="$(img)/logo_only.gif" border=0 alt="CaucusCare.Com"
"   ></a>&nbsp;&nbsp;</td>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"
"<tr>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"<td colspan=2><img src="$(clearpix)" width=1 height=7 alt=""></td>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"
"<tr>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"<td class="small">&nbsp;</td>
"<td class="small">&nbsp;Password</td>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"<tr>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"<td class="small">&nbsp;</td>
"<td class="small">&nbsp;<input type="password" size="16" name="password"
"    tabindex="2">&nbsp;&nbsp;</td>
"    <td><input type="image" src="$(img)/login.gif" border=0 tabindex="3"></td>
"<td class="small">&nbsp;</td>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"
"<tr>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"<td colspan=4><img src="$(clearpix)" width=1 height=11 alt=""></td>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"
"<tr>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"<td class="medium">&nbsp;</td>
"<td class="small" colspan=2>&nbsp;<a class="nav" tabindex="4"
"     href="$(http)://$host()/$(sweb)/swebsock/$pid()/$(iface)/reg/reset.cml"
"   >Forgot your password?</a></td>
"<td class="medium">&nbsp;</td>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"
"<tr>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"<td colspan=4><img src="$(clearpix)" width=1 height=13 alt=""></td>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"
"<tr>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"<td class="medium">&nbsp;</td>
"<td class="small" colspan=2>&nbsp;<a class="nav" tabindex="5"
"  href="$(http)://$host()/$(sweb)/swebsock/$pid()/$(iface)/reg/register.cml?\
         method=2"
"    onClick="return cookies_allowed();"
"   >New User Registration</a></td>
"<td class="medium">&nbsp;</td>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"
"<tr>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"<td colspan=4>&nbsp;</td>
"<td class="edge"><img src="$(clearpix)" width=2 height=1 alt=""></td>
"
"<tr>
"<td colspan=6 class="edge"><img src="$(clearpix)" width=2 height=2 alt=""></td>
"</table>
"
"<p>
"
"</form>
"
"</center>
"
"</div>
"</body>
"</html>
