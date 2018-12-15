#
#---unarchiver.sh.   Unarchive a Caucus 5.1 archive file, into a new conference.
#
#   Arguments:
#     $1 Caucus site home directory
#     $2 new conference id number (key)
#     $3 archive file name (not path, just name)
#     $4 restore student's grades? (0=no, 1=yes)
#     $5 full pathname of java
#
#   Output (to stdout)
#     0  success
#     1  bad arguments
#     2  no caucus site in directory $1
#     3  no such file $3
#     4  cannot load swebd.conf
#     5  cannot access MySQL with parameters from swebd.conf
#     6  cannot run java
#
#:CR 04/25/2007 New script.
#:CR 10/08/2007 Put log file in $HOME, so we can share UNARCHIVER
#        from a master directory.
#:CR 01/10/2008 Add $5 to get proper path to java.
#---------------------------------------------------------------------

if test "x$3" = "x"; then
   echo "1"
   exit
fi

if test ! -f $1/SWEB/swebd.conf; then
   echo "2"
   exit
fi

if test ! -f $1/ARCHIVE5/$3; then
   echo "3"
   exit
fi

if test ! -f $5; then
   echo "6"
   exit
fi

tmpdir=/tmp/unarchiver.$$
rm -rf $tmpdir
mkdir  $tmpdir

log=$1/unarchiver.log
echo " " >>$log
date     >>$log

cd $tmpdir
if which gzcat >/dev/null 2>/dev/null; then
   zcat=gzcat
else
   zcat=zcat
fi
echo "$zcat $1/ARCHIVE5/$3 | tar xf -" >>$log
$zcat $1/ARCHIVE5/$3 | tar xf - 2>/dev/null

echo "Attempting to unarchive $3 into conference number $2." >>$log

ls *.xml */*.xml >>$log

echo "$5 -jar $1/UNARCHIVER/CaucusUnarchiver.jar $1 $tmpdir $2 1" >>$log

umask 022
ls *.xml */*.xml | \
    $5 -jar $1/UNARCHIVER/CaucusUnarchiver.jar $1 $tmpdir $2 $4 2>&1 >>$log

cd
rm -rf $tmpdir

#---------------------------------------------------------------------
