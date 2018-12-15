This is an old (ancient) install guide, for Caucus 4.x.  If you're
really stuck with an odd installation (e.g. under a non-Apache web
server, there might be some clues in here.  But otherwise you should
ignore this file.  As the Caucus 5 distribution gets cleaned up,
this will eventually be deleted.


                  Caucus 4.4 Brief Installation Guide
                  Last revised: 13 November 2002

I. INTRODUCTION

This is the BRIEF, plain-text version of the Caucus 4.4 Installation
Guide.  It is intended only for knowledgeable system managers and
webmasters who want to get Caucus up and running quickly.

Far more information is available in the official "Caucus 
Installation Guide", which is available from http://caucuscare.com/TLIB.

Questions and comments are also welcome on our technical support
conferences.  Go to http://caucuscare.com to register a userid 
and join our conferences.


II. INSTALLATION

II.1 Create the Caucus Userid

Create a new userid, called "caucus", with its own home 
directory.  (You may use a different name if you prefer.  The 
installation procedure will adapt to whichever name you 
choose.)  The home directory for Caucus must have enough free 
disk space to contain all of the Caucus programs and data 
files, and all of the anticipated conference data.  A minimum 
of 100 megabytes is recommended.  (The software itself is 
about 25 megabytes, maximum.)

II.2 Unpackage the Software

The software installation is the same whether you are:

  * installing Caucus for the first time
  * upgrading your existing Caucus software

The installation procedure automatically determines if this is 
a new installation of Caucus or an upgrade to an existing 
Caucus site.  If you are upgrading Caucus, your existing 
conferences will not be affected by the upgrade.

Login to the Caucus userid.  You must actually log in to this 
userid; do not use "root" or "su" access.

These instructions in this guide assume that the Caucus home 
directory is in /home/caucus.  In this guide, whenever you see 
the path /home/caucus, replace it with the actual pathname of 
the Caucus home directory on your system.

The Caucus software is delivered in a file called caucus41.t.Z.  
In the Caucus home directory, type the command below to unpackage 
this file.

    zcat caucus41.t.Z | tar xvf -

(You have probably already done this in order to be reading this file.)


II.3 Run the installation script

The software includes an installation script that will 
automatically create the proper script files, set the proper 
file permissions, and so on.  To run the script, type:

    ./cinstall

The script will ask for your activation key, and the hostname 
(and port number, if needed) of your web server.  Be prepared 
to provide these.  If you have not received an activation key, 
contact CaucusCare at support@caucuscare.com.

Caucus places certain files in the Caucus userid's public HTML 
directory.  The standard name for this directory is 
public_html.  The Caucus distribution includes a public_html 
directory with the necessary files already in it.  If your 
httpd server uses a different name, rename public_html to that 
directory name now.


II.4   Starting and Using Caucus

To start the Caucus server, run the following commands
from "root":

      rm -f /home/caucus/SOCKET/sweb
      rm -f /home/caucus/SOCKET/sweb0*
      /home/caucus/SWEB/swebd  /home/caucus/SWEB/swebd.conf

You should also add these lines to your system start-up file 
(such as /etc/rc.d/rc.local, or whatever it is called on your host) 
so that the Caucus server will start when your system reboots.

The Caucus installation script creates a default HTML page for 
accessing the Caucus conferences on your host.  It is located 
in /home/caucus/public_html/caucus.html.  The URL for this 
file is typically:

   http://yourhost/~caucus/caucus.html

This file is just a template for how to access Caucus from the 
Web.  If your organization already has a set of web pages, you 
will probably want to integrate this file with your existing 
pages.  You might choose to copy the links in this file to the 
appropriate places on your existing web pages; or you might 
decide to edit the caucus.html file and simply make it look 
more like your other web pages.


III.  CONFIGURE YOUR WEB SERVER

Sections III.1 through III.4 describe, in the abstract, the changes 
that must be made to your web server configuration to make it work 
properly with Caucus.  Subsequent sections describe the 
precise details of these changes for the servers listed above.

Once you have made these changes, your Caucus site will be up 
and running!  But you should continue on and read chapter 4, 
"Userids and Passwords", to understand the implications of 
userids and passwords on the Web.


III.1 Define CGI directories

Caucus uses several different CGI programs in the directories 
SWEB and REG to communicate with the web server.  The best way 
to identify these programs to the web server is to declare 
SWEB and REG as CGI directories.

Specifically, declare the following mappings of URLs to CGI 
directories:

      http://yourhost.com/sweb/   to   /home/caucus/SWEB/
and   http://yourhost.com/reg/    to   /home/caucus/REG/

If for some reason you cannot declare a CGI directory, enable 
your server in some other way to treat the files:

     /home/caucus/SWEB/start.cgi
     /home/caucus/SWEB/swebsock
     /home/caucus/REG/swebsock

as CGI programs.


III.2 Define special "/caucus" URL

Caucus users who have already registered a userid may go 
directly to specific conferences, items, or responses through 
the special URLs shown below.

  http://yourhost.com/caucus                       ("Caucus Center" page)

  http://yourhost.com/caucus/conference_name       (conference home page)

  http://yourhost.com/caucus/conference_name/item  (go to that item)

  http://yourhost.com/caucus/conference_name/item:response 
                                                   (go to that response)

In order to make these special URLs work, the web server must 
be configured  to map URLs that begin "http://yourhost.com/caucus" 
to the CGI file /home/caucus/REG/start.cgi.  (This may not be possible 
for all sweb servers.  Users of such servers can still access 
Caucus through the regular caucus.html page.)


III.3 Restrict Access with userids and passwords

(Ignore this subsection unless you intentionally are using
the "old" method of authenticating users.)

Caucus' security requires that each user be identified by a 
unique userid and password.  Caucus uses the standard web 
"access authorization" protocol to implement userid and 
password checking.

To enable access authorization for Caucus, you must declare 
that the directory /home/caucus/SWEB is protected by a userid 
and password database file.  For some web servers (notably
Apache), this is done automatically by the Caucus installation 
script.


III.4 Restart your server

Some web servers must be shutdown and restarted before any of 
the previous changes will take effect.  See your web server 
documentation for details.


III.5 Apache Web Server Instructions

This section describes the precise details of configuring the 
Apache web server to work with Caucus.  It assumes 
that you have already installed your web server and are 
generally familiar with the server configuration files.

Find the httpd configuration file srm.conf.  Edit it, and add 
the lines:

   ScriptAlias  /sweb/   /home/caucus/SWEB/

   ScriptAlias  /reg/    /home/caucus/REG/

Define special "/caucus" URLs.  Also in srm.conf, add the lines:

   ScriptAlias  /caucus/ /home/caucus/REG/start.cgi/
   ScriptAlias  /caucus  /home/caucus/REG/start.cgi

Restrict Access with userids and passwords.  Access authorization 
for NCSA and Apache servers is set up automatically by the Caucus 
installation script.  It creates the file /home/caucus/SWEB/.htaccess, 
which declares that the directory is password-protected.  That file 
points in turn to the userid and password database file 
/home/caucus/caucus_passwd, which is also set up by the Caucus 
installation script.

You must restart the httpd server in order for these changes 
to take effect.  See your server documentation for details.


III.6 Netscape Enterprise Server 2.x Instructions 

This section describes the precise details of configuring the 
Netscape Enterprise Server, version 2.x, to work with Caucus.  
It assumes that you have already installed your web server and 
are generally familiar with server configuration.

Define CGI directories.  From the server configuration page, 
choose "Programs", sub-selection "CGI directory".  Add entries for:

   URL prefix sweb/,    CGI directory /home/caucus/SWEB/

   URL prefix reg/,     CGI directory /home/caucus/REG/

And choose "save and apply" these changes (replacing 
/home/caucus with the home directory of the Caucus userid on 
your system).

Define special "/caucus" URLs.  From the server configuration page, 
choose "Programs", sub-selection "CGI directory".  Add an entry for:

    URL prefix caucus/,    CGI directory /home/caucus/REG/start.cgi

And choose "save and apply" these changes.

Restrict Access with userids and passwords.  In the directory 
/home/caucus/SWEB, create a world-readable file called .nsconfig, 
containing the lines:

  <Files *>
  RequireAuth userfile=/home/caucus/caucus_passwd realm=Caucus userpat=*
  </Files>

From the server configuration page, chose "System Settings", 
sub-selection "Dynamic Configuration Files".  In the "file 
name" field, type ".nsconfig".  Do not change any of the other 
settings or checkboxes in this form.  "Save and apply" these 
changes.

To add userids to the database file, use the self-registration
page from the link in caucus.html.  (Do not use the Netscape server 
user database functions.)

It is not necessary to restart the Netscape Enterprise server.  
All of the changes will take effect immediately.

You may, however, have to explicitly enable "user document 
directories" (i.e., the public HTML directories) in order for 
Caucus to work.  To do this, from the server configuration 
page, choose "Content Management", sub-selection "User 
Document Directories".  You don't need to make any changes, 
just click on "OK" and "Save and Apply" to enable the user  
document directories.

-------------------------------------------------------------------

IV. OTHER INFORMATION

This is the end of the brief install guide for Caucus 4.4.  There
is much more information about the management and customization
of Caucus, and other topics, in the full install guide.  See
http://caucuscare.com/TLIB.

-------------------------------------------------------------------
