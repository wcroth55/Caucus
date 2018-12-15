#!/bin/sh
#
#---import_export.sh   "Daemon" shell process to import and export
#                      e-mail into/from Caucus conferences.
#   Purpose:
#      This script runs forever, as "daemon" (well, sort of).  It:
#
#      (a) "Imports" any outstanding email for the current userid
#          (processes it, looking for the project Subject: field that
#          indicates email bound for a Caucus conference)
#
#      (b) If this is an hour (as listed in variable "sendhours" below) when
#          we should export new material as email, do that as well.  This
#          means scanning all the conferences this userid can join (see
#          swebd.conf) Caucus configuration file parameter 'email_id'), and
#          sending new material packaged as an email to the respective
#          conferences' email participation list.
#
#      (c) Sleeps for a while, and then repeats.
#
#   Usage:
#      Typically invoked once a day from cron.  On startup, if it detects
#      another instance of itself running, then it immediately exits.
#
#:CR 10/01/04 New script.
#:CR 10/13/04 Derive caucus home dir from arg 0.
#:CR 10/18/04 Add debugging code.
#:CR 12/15/04 Determine mail client (mailx) and 'ps' format dynamically.
#:CR 01/13/05 Use better method for counting # of import_export processes.
#-----------------------------------------------------------------------

#---Note that sendhours must use 2-digit hour numbers, and *not* include "00" !
runonce=0
debug=0
sendhours="06 12 17"
sleeptime=600
if test $debug -gt 0; then
   sleeptime=120
fi
done=
cmldir=CC51

#---Reinvoke ourselves with our pid to identify ourselves in a "ps x" listing.
if test "x$1" = "x"; then
   pid=$$
   $0 $pid x >>$HOME/import_export.stdout 2>>$HOME/import_export.stderr &
#  $0 $pid x &
   exit
fi
sleep 3

#---Determine proper 'ps' command format.
ps="ps wx"
a=`$ps 2>/dev/null`
if test "x$a" = "x"; then
   userid=`id | tr "()" "  " | (read a b c; echo $b)`
   ps="ps -fu $userid"
fi

#---How many others of us are there?  If at least one, then
#   we are superfluous; exit.
procs=`$ps | grep  import_export.sh | grep -v grep | grep -v " $1 " | \
        wc | (read a b; echo $a)`
#pcount=`echo "$procs" | wc | (read a b; echo $a)`
#echo "$procs"

if test $procs -gt 0; then
   exit
fi

#---Determine best mail client.
mailx=`which mailx 2>/dev/null | tail -1`
if test "x$mailx" = "x"; then
   mailx=`which mail 2>/dev/null`
   if test "x$mailx" = "x"; then
      exit
   fi
fi

#---Make sure caucus can access mailtext file.
chmod 710 $HOME

#---Assemble base name of Caucus home dir from arg 0.
caucus=
nameparts=`echo $0 | tr "/" " "`
for x in $nameparts; do
   if test "$x" = "SCRIPTS"; then
      break
   fi
   caucus=$caucus/$x
done

#---Loop forever, importing and exporting email into/from Caucus.
while true; do

   #---Get the mail.
   rm -f mailtext
   (echo "p *";  echo "d *";  echo "q") | $mailx >mailtext 2>/dev/null
   chmod 644 mailtext
  
   #---If there was any incoming mail, import it.
   if test -s mailtext; then
      if test $debug -gt 0; then
         now=`date +"%Y-%m-%d_%H:%M:%S"`
         cp mailtext mailtext_$now
      fi
      $caucus/SWEB/sweba $caucus/SWEB/swebd.conf $cmldir/main/email_import.cml \
                         mailtext /tmp /bin/mail
      date >>import.log
   fi

   #---Exporting happens only at specific hours, as listed in $sendhours.
   #   At midnight, reset the list of hours that were "done today".
   now=`date +"%H"`
   if test "$now" = "00"; then
      done=
   fi

   sendnow=$debug
   for h in $sendhours; do

      #---Is this an hour when we should export?
      if test "$now" = "$h"; then

         #---Did we already "do it" for this hour?
         doit=1
         for d in $done; do
            if test "$d" = "$now"; then
               doit=0
            fi
         done

         #---Nope.  Do the export, and add this hour to the list of hours done.
         if test "$doit" -gt 0; then
            done="$done $h"
            sendnow=1
         fi
      fi
   done
   if test "$sendnow" -gt 0; then
      date >>export.log
      $caucus/SWEB/sweba $caucus/SWEB/swebd.conf $cmldir/main/email_sendup.cml \
                         /tmp /bin/mail
      date >>export.log
   fi

   #---Sleep
   if test "$runonce" -gt 0; then
      exit
   fi
   sleep $sleeptime
done

#-----------------------------------------------------------------------

