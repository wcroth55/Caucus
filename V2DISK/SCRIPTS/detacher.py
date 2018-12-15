
###  detacher -- "Detach" files attached to an email message.
###
###  Usage:
###     detacher.py <emailtext
###     (where emailtext is a file containing the text of a single
###      email message)
###
###  Output (stdout):
###     May write optional warnings, always followed by "path=" and the
###     full pathname of a unique temporary directory (name based
###     on process id), which in turn contains the files "detached" 
###     from the message.  For example, if the processed message
###     contained files "a.gif" and "b.doc", the list of files in
###     the temporary directory might look like:
###        extracted.111__a.gif
###        extracted.1251__b.doc
###        extracted.1775__NONAME.txt
###        extracted.18__NONAME.dat
###        extracted.23__NONAME.txt
###        message.eml
###
###     One or more of the NONAME.txt files contains the text of the message
###     (one might be plaintext, the other might be HTML text).  The
###     message.eml is a duplicate of the original and can be ignored.
###
###     It is recommended that the files in the directory be processed
###     "soon" after running detach, and the temporary directory then
###     be deleted (to prevent accidentally recycling of the directory
###     when the process id eventually gets reused).
###
###  Previous history:
###     Detach is just a slight rewrite of 'pymavis', which itself is in
###     turn a python rewrite of amavis.  The original purpose
###     of pymavis was to pluck files out of an email message and
###     scan them for viruses; in detach, only the "plucking"
###     functionality is retained.
###
###     The changes and comments are copyright (C) 2004 Charles Roth & 
###     CaucusCare.com, and are released under the terms and conditions 
###     of the GNU General Public License -- see
###     http://www.gnu.org/copyleft/gpl.html.  Current version of detacher
###     may be downloaded from http://caucuscare.com/TLIB/detacher.py.
###
###     Detacher version 1.00,  7 July 2004.
###     Detacher version 1.01, 19 August 2004.  Replace spaces and ()'s
###        in extracted filenames with underscores.
###     Detacher version 1.02,  1 October 2004.  Don't trim filenames
###        down to 32 characters in safefilename() !  Add "path=" output
###        to make it easy to skip over diagnostic output.
###
###  --------------------------------------------------------------------
###  Original header comments for pymavis follow:
###
###  pymavis - an amavis clone in python       (C) 2004, A'rpi
###
###  This file is subject to the terms and conditions of the GNU
###  General Public License. See the file COPYING in the main
###  directory of the Linux distribution for more details.
###
###  http://mplayerhq.hu/~arpi/sa-rules/
###
###  usage:   ./email5.py <message
###
###  exit codes:
###       0 = the message is safe
###       1 = error parsing message (may be resource problem, python bug etc)
###     100 = message contains potentialy unsafe attachments (like .exe files)
###     101 = message contains attachment of disallowed extensions (.pif etc)
###     102 = message contains virus (at least one virus checker said alert)
###  --------------------------------------------------------------------
###

import os
import sys
import string
import email
import email.Header
import binascii
import time
import struct
import traceback


# verify raw filename for length, syntax, invalid chars/endings, etc
# returns: (retvalue [0/1/2], reason [string], extension [string])
def chkfilename_raw(filename):
    fnstripped=filename.strip()
    ext=fnstripped[fnstripped.rfind('.')+1:].lower()
    ret=(0,"OK",ext)
    # filename sanity checks:
    if len(filename)>120:
	return (2,"filename too long (max 120 chars allowed)",ext)
    if fnstripped.find("     .")>0:
	return (2,"hidden filename extension (many spaces inserted)",ext)
    for lastchar in fnstripped:
	if ord(lastchar)<32:
	    return (2,"filename contains control chars (%d)" % ord(lastchar),ext)
    lastchar=ord(fnstripped[-1:])
    if lastchar<32 or lastchar>=128 or lastchar in (125,96,39,34):
	return (2,"filename ends with bad char (%d)" % lastchar,ext)
    while lastchar==46:
	fnstripped=fnstripped[:-1].strip()
	if not fnstripped:
	    return (2,"filename consist only of dots?",ext)
	# cut down last dot, re-calc lastchar & ext:
	lastchar=ord(fnstripped[-1:])
	ext=fnstripped[fnstripped.rfind('.')+1:].lower()
	ret=(1,"filename ends with dot",ext)
    return ret

# check file attribs: extension, compression, size, mime-type
#   zip: 0=plain file  1=zip(stored)  2=zip(compressed)  3=zip(crypted)
# returns: (retvalue [0/1/2], reason [string])
def chkfilename_attr(ext,size,zip,mimetype):
    # unusual (and potentially dangerous) scripts:
    if ext in ("hta","vb","vbs","bas"):
	return (2,"disallowed extension (unusual scripts)")
    # unusual scripts: (uncompressed only, allowed in zipped driver packages)
    if ext in ("reg","inf","dll","vxd","pif"):
	if zip:
	    return (1,"disallowed extension (unusual alone)")
	return (2,"disallowed extension (unusual)")
    # potential exe formats:
    if ext in ("exe","pif","scr","com","bat","cmd"):
	if zip==3:
	    return (2,"suspicious archive (encrypted exe file, cannot check)")
	if zip==1 and size>2000 and size<300000:
	    return (2,"suspicious archive (null-compressed small exe file)")
	if zip and size<150000:
	    return (1,"potentially dangerous executable (compressed, cannot check)")
	if ext!="exe" and ext!="com":
	    return (1,"potentially dangerous executable (not .exe/.com extension)")
    # possible but suspicious script:
    if ext in ("lnk","url","js","hlp"):
	return (1,"potentially dangerous executable/script")
    # zip in zip? html in zip?
    if zip and ext in ("zip","rar","arj"):
	return (1,"suspicious (archive in archive)")
    if zip and ext in ("html","htm"):
	return (1,"suspicious (html in archive)")
    return (0,"OK")

def chkfilename(filename,size=-1,zip=0,mimetype=None):
    ret1,reason1,ext=chkfilename_raw(filename)
    ret=chkfilename_attr(ext,size,zip,mimetype)
    if ret[0]<ret1:
	ret=(ret1,reason1)
    tmp=u"ZIP[%d] %5d %s (%s) '%s' => %d (%s)" % (zip,size,mimetype or '?',ext,filename,ret[0],ret[1])
#   print `tmp`
    return ret[0]

def unpack(fmt,f):
    return struct.unpack(fmt,f.read(struct.calcsize(fmt)))

def unp_cstr(f):
    s=""
    while 1:
	c,=unpack("c",f)
	if ord(c)==0:
	    return s
	s+=str(c)

def dump_pe(f,startoff,exesize):
  dlllist=None
  try:
    # IMAGE_FILE_HEADER 0xD8
    zero,arch,numsects,timedate,debug1,debug2,ophdrsize,charflags = unpack("<HHHLLLHH",f)
    # IMAGE_OPTIONAL_HEADER 0xF0
    magic,lnkvers,size_code,size_data,size_bss,rva1,rva2,rva3,loadaddr=unpack("<HHLLLLLLL",f)
    sect_align,file_align,os_ver,bin_ver,subsys_ver,win32_ver=unpack("<LLLLLL",f)
    image_size,header_size,CRC,subsys,dllchr=unpack("<LLLHH",f)
    stack_rvd,stack_com,heap_rvd,heap_com,loader_flags,num_dirent=unpack("<LLLLLL",f)
    # IMAGE_DATA_DIRECTORYs
    img_dir_entries=[]
    for i in range(0,16):
	tmp=unpack("<2L",f)
        img_dir_entries.append(tmp)
    # SECTIONS:
    sections=[]
    for sno in range(numsects):
	sect_name,tmp,rva,rawsize,fileoff,ptr_reloc,ptr_lno,num_reloc,num_lno,flags=unpack("<8sLLLLLLHHL",f)
	name=""
	for c in sect_name:
	    if ord(c)>=32:
		name+=c
	#     0      1       2     3    4
	tmp=(name,rawsize,fileoff,rva,flags)
	sections.append(tmp)
#	print "SECT: '%s' %d 0x%08X<-0x%08X" % (name,rawsize,fileoff,rva)
	if name=="_winzip_":
	    exesize=fileoff+0x2f
#	    print "WinZip SFX detected, ZIP file at 0x%08X" % (exesize)
	elif fileoff+rawsize>exesize:
	    exesize=fileoff+rawsize

    def rva_to_file(rva,sections):
	for sect in sections:
	    if rva>=sect[3] and rva<sect[3]+sect[1]:
		return sect[2]+(rva-sect[3])
	return rva

    tmp=img_dir_entries[1]
    if tmp[1]:
	fpos=rva_to_file(tmp[0],sections)
	if fpos:
	    # we have imports! load 'em all!
	    f.seek(startoff+fpos,0)
	    imps=[]
	    while 1:
		imp=unpack("<LLLLL",f)
		if (imp[0]==0 or imp[3]==0) and imp[4]==0:
		    break
		imps.append(imp)
#		print imp
	    dlllist=[]
	    for imp in imps:
		dllentry=[""]
		# read DLL name:
		fpos=rva_to_file(imp[3],sections)
		if fpos:
		    f.seek(startoff+fpos,0)
		    dllentry[0]=unp_cstr(f)
#		    print "DLL: "+dllentry[0]
		# dump symbols:
		if imp[0]:
		    fpos=rva_to_file(imp[0],sections)
		else:
		    fpos=rva_to_file(imp[4],sections)
		if fpos:
		  try:
#		    print fpos
		    f.seek(startoff+fpos,0)
		    syms=[]
		    while 1:
			tmp,=unpack("<L",f)
			if tmp==0:
			    break
			syms.append(tmp)
#		    print syms
		    for sym in syms:
			fpos=rva_to_file(sym,sections)
			if fpos:
			    f.seek(startoff+fpos,0)
			    tmp=unpack("<H",f)
			    dllentry.append(unp_cstr(f))
		  except:
 		    print "error parsing DLL info at 0x%08X" % (fpos)
		dlllist.append(dllentry)
  except:
    print "Exception!!! while PE-EXE parsing:"
    traceback.print_exc()
  return ("PE",exesize,dlllist)

def dump_exe(f):
    startoff=f.tell()
    try:
	MZ,=unpack("<H",f)
	if not MZ in (0x5A4D,0x4D5A):
	    return None
	size_l,size_h,relocs,hdrsize,allocmin,allocmax,SS,SP,CRC,CS,IP,relocoff,ovl = unpack("<13H",f)
    except:
	return None
    size=size_h*512
    if size_l:
	size+=size_l-512
#    if relocoff>=0x40:
    try:
        # new-exe:
	f.seek(startoff+0x3c)
	ne_off,=unpack("<L",f)
	f.seek(startoff+ne_off)
	NE,=unpack("<H",f)
	if NE==0x4550:
	    return dump_pe(f,startoff,size)
	if NE==0x454E:
	    # win 3.1 (16-bit) EXE (FIXME: parse out size!)
	    return ("NE",size)
	if NE==0x434C:
	    # DOS/32A: (FIXME: parse out size!)
	    return ("LC",size)
	if NE==0x454C:
	    # dos4gw/watcom: (FIXME: parse out size!)
	    return ("LE",size)
    except:
        NE=0
    return ("MZ",size)

def chkrar(zf,zfsize):
    ret=0
    total=0
    startoff=zf.tell()
#    print "chkrar: size=%d off=0x%X" % (zfsize,startoff)
    marker=zf.read(7)
    if marker[0:4]!="Rar!":
#	print "Not a Rar! file."
	return (0,0)
    # looks like a valid Rar file, get filelist:
    try:
      while zf.tell()<startoff+zfsize:
        hdr_start=zf.tell()
        try:
	  headcrc,headtype,flags,headsize = unpack("<HBHH",zf)
#	  print "hdr_start=0x%X  type=0x%02X hsize=%d" % (hdr_start,headtype,headsize)
	except:
#	  print "broken RAR file (truncated)"
          break
	addsize=0
	if headtype==0x74:
	    # file:
	    fh_startoff=zf.tell()
	    csize,usize,hostos,filecrc,filetime,unpver,method,namelen,attr=unpack("<LLBLLBBHL",zf)
	    filename=zf.read(namelen)
	    addsize=csize
	    total+=1
#	    print "RAR: filename='%s' size=%d/%d method=%d" % (filename,csize,usize,method)
	    if flags&4:
		ret|=chkfilename(filename,usize,3)
	    else:
		if method==0x30:
		    ret|=chkfilename(filename,usize,1)
		    ret|=chkcontent(zf,csize,filename)
		else:
		    ret|=chkfilename(filename,usize,2)
	else:
	    # special:
	    if flags&0x8000:
		addsize=unpack("<L",zf)
#	    print "RAR: special block type=0x%02X size=%d+%d" % (headtype,headsize,addsize)
	zf.seek(hdr_start+addsize+headsize,0)
    except:
       print "Exception!!! while RAR listing:"
       traceback.print_exc()
    
    return (total,ret)
    

def chkzip(zf,zfsize):
    ret=0
    total=0
    startoff=zf.tell()
#    print "chkzip: size=%d off=0x%X" % (zfsize,startoff)
    try:
      while zf.tell()<startoff+zfsize:
        try:
	  signature,version,flags,method,time,date,crc,csize,usize,namelen,extralen = unpack("<LHHHHHLLLHH",zf)
	except:
	  if total>0:
 	    print "broken ZIP file (truncated)"
          break
	if signature != 0x4034B50:
	    break;
	filename=zf.read(namelen)
	zf.seek(extralen,1)
	total+=1
	if method>0:
	    ret|=chkfilename(filename,usize,2)
	else:
	    if flags&1:
		ret|=chkfilename(filename,usize,3)
	    else:
		ret|=chkfilename(filename,usize,1)
		ret|=chkcontent(zf,csize,filename)
	zf.seek(csize,1)
    except:
       print "Exception!!! while ZIP listing:"
       traceback.print_exc()
    
    return (total,ret)

def chkcontent(f,size,filename):
# print "**** chkcontent[%s:%d] ****" % (filename,size)
  ret=0
  startoff=f.tell()
  try:
    # check for dos/win16/win32 exe files:
    exeret=0
    sfxoff=startoff
    exe=dump_exe(f)
    if exe:
#       print "EXE format detected, type=%s size=%d" % (exe[0],exe[1])
	ext=filename.strip().lower()[-4:]
	if not ext in [".exe",".dll",".scr"]:
#	    print "Hmm, EXE format but not .EXE/.DLL extension!"
	    exeret|=2
        if exe[0]=="PE":
	    if not exe[2] or len(exe[2])<3:
		exeret|=2
#		print "suspicious executable! protected/compressed? (too few imported DLLs)"
	    if exe[2]:
	      for dll in exe[2]:
		dllname=dll[0].lower().strip()
		if dllname in ["wsock32.dll","wininet.dll","iphlpapi.dll","ws2_32.dll","urlmon.dll","netapi32.dll","netman.dll","winsock.dll","rpcrt4.dll"]:
		    exeret|=2
#		    print "suspicious dependancy! network related:"
#		print dll
	if exe[1]+32<size and exe[1]>2:
	    # .exe has some (>32 bytes) data appended, maybe SFX?
	    sfxoff=startoff+exe[1]
	if exe[0] in ("PE","NE") and size<80000 and exeret==0:
#	    print "too small (<80k) win16/win32 .exe file -> suspicious"
	    exeret|=1

    if not exe or sfxoff!=startoff:
      # check for archives:
      for chkarchive in (chkzip,chkrar):
	f.seek(sfxoff)
	atotal,aret=chkarchive(f,size-(sfxoff-startoff))
	if atotal>0:
	    # ignore archive warnings in case >=3 files:
	    if atotal<3 or aret!=1:
		ret|=aret
	    # ignore .exe warnings in case of SFX:
	    if exeret==1:
		exeret=0
	    break
	
    ret|=exeret
  except:
    print "Exception!!! while content scan (file %s):" % (filename)
    traceback.print_exc()
  f.seek(startoff)
  return ret

def chkfile(filename):
    ret=0
#    print "chkfile: "
#    print filename
    size=os.stat(filename[0]).st_size
    if filename[1]:
	ret|=chkfilename(filename[1],size,0,filename[3])
    if filename[2] and (not filename[1] or filename[1]!=filename[2]):
	ret|=chkfilename(filename[2],size,0,filename[3])
    if size>30:
	f=open(filename[0],"r")
	ret|=chkcontent(f,size,filename[0])
	f.close()
    return ret


def hdrparse(s,pmode=1):
    global mimeerror
    params=[]
    qflag=0
    iflag=0
    icnt=0
    cflag=0
    comment=""
    key=""
    value=""
    keymode=1
    lastnonws=0
    wsflag=1
    for c in s:
	if not qflag:
	    # process control chars, by priority:
	    if c=='\\':
		qflag=1
		continue;
	    if c=='"':
		iflag=not iflag
		icnt+=1
		if icnt>2:
		    mimeerror+=1
		wsflag=0
		continue;
	    if not iflag:
		if wsflag:
		    # skip initial whitespaces, unless they're quoted
		    if c in ('\t',' '):
			continue
		    wsflag=0
		if c=='(':
		    cflag+=1
		    continue
		if c==')':
		    cflag-=1
		    continue
		if cflag==0 and pmode:
		    if c=='=' and keymode:
			keymode=0
			lastnonws=0
			wsflag=1
			continue
		    if c==';':
			params.append((key.strip().lower(),value[:lastnonws],comment.strip()))
			comment=""
			key=""
			value=""
			keymode=1
			icnt=0
			continue
	if cflag>0:
	    comment+=c
	else:
	    if keymode:
		key+=c
	    else:
		value+=c
		if iflag or qflag or not c in ('\t',' '):
		    lastnonws=len(value)
	qflag=0

    if cflag!=0 or qflag or iflag:
	mimeerror+=2

    if not pmode:
	return (key.strip(),comment.strip())
    params.append((key.strip().lower(),value[:lastnonws],comment.strip()))
    return params

# inspired by Header.py::__unicode__()
def hdrdecode(h):
    if not h:
	return u""
    uchunks = []
    last = None
    for s,enc in email.Header.decode_header(h):
        if last not in (None, 'us-ascii'):
            if enc in (None, 'us-ascii'):
                uchunks.append(u' ')
        else:
	    if enc not in (None, 'us-ascii'):
        	uchunks.append(u' ')
	last=enc
	try:
	    uchunks.append(unicode(s,enc or "latin-1"))
	except:
	    uchunks.append(unicode(s,"latin-1"))
    return u''.join(uchunks)

# makes safe filename from unicode string:
def safefilename(s):
   fn=s.encode("us-ascii","replace").strip()
   fn=fn.replace("\\","_").replace("/","_").replace(":","_").replace(" ","_").replace("(","_").replace(")","_")
   while fn:
      lastchar=fn[-1:]
      if lastchar<=' ' or lastchar>'~' or lastchar=='.' or lastchar=='?':
         fn=fn[:-1]
         continue
      break
#  return fn[-32:].strip()
   return fn.strip()

############################### MAIN ####################################

if len(sys.argv)>1:
    # if filename given, check it as local file, not email message!
    filename=[sys.argv[1],sys.argv[1],None,None]
    ret=chkfile(filename)
#   print "exit code: %d" % (ret)
    sys.exit(ret)


#path="/var/pymavis/scandir-"+str(time.time())+"-"+str(os.getpid())
path="/tmp/detacher"+str(os.getpid())
os.mkdir(path)
os.chdir(path)


filelist=[]
rawout=open("message.eml","w")
filelist.append("message.eml")

file=sys.stdin
lineno=0
ret=0
mimeerror=0

try:
  in_header=1
  bounds={}
  header=[]
  body=""
  outfile=None
  while 1:
    line=file.readline()
    if not line:
	# EOF!
	if outfile:
	    outfile.close()
	    outfile=None
	    ret|=chkfile(filename)
	break;
    rawout.write(line)
    lineno+=1
#   while line[0:2]=="> ":
#	line=line[2:]
#    line=line.rstrip("\n")
#    print 'Line: "'+line+'"'
    if in_header:
	# if line is (really) empty => EOH
	if not line or (not line.strip() and len(line)<=4):
#	    print "EOH"
	    filename=[None,None,None,None]
	    coding=None
	    try:
		p_ctype=None
		p_cdisp=None
		p_ctenc=None
		for hs in header:
		    fpos=hs.find(':')
		    if fpos<=0:
			# not a field, check if it's a boundary!
			b_ok=0
			for b in bounds.keys():
			    if hs.find("--"+b)==0:
#				print "HDR: found a boundary inside a header!"
				b_ok=1
				break
			if not b_ok:
			    print "HDR: Invalid header field: '"+hs+"'"
			    # mimeerror+=2
			continue
		    if len(hs)>998:
#			print "HDR: Header line too long! (%d chars)" % (len(hs))
			mimeerror+=1
		    field=hs[0:fpos].strip().lower()
		    fval=hs[fpos+1:]
		    # parse content-type: header
		    if field=="content-type":
			if p_ctype:
#			    print "HDR: MIME error: content-type redefined!"
			    mimeerror+=5
			p_ctype=hdrparse(fval)
			filename[3]=p_ctype[0][0]
			for param in p_ctype:
			    if param[0]=="name":
				filename[2]=hdrdecode(param[1])
			    elif param[0]=="boundary":
				# not sure if we have to strip() it:
				bounds[param[1].strip()]=1
#			    print param
			continue
		    # parse content-disposition: header
		    if field=="content-disposition":
			if p_cdisp:
#			    print "HDR: MIME error: content-disposition redefined!"
			    mimeerror+=5
			p_cdisp=hdrparse(fval)
			for param in p_cdisp:
			    if param[0]=="filename":
				filename[1]=hdrdecode(param[1])
			continue
		    if field=="content-transfer-encoding":
			if p_ctenc:
#			    print "HDR: MIME error: transfer-encoding redefined!"
			    mimeerror+=10
			    # ignore re-defination, just like Outlook does
			else:
			    p_ctenc=hdrparse(fval,0)
			    coding=p_ctenc[0]
			continue
	    except:
		print "Exception!!! while MIME header parsing (line %d):" % (lineno)
		traceback.print_exc()

#	    print "!!!parsed!!! coding: "+(coding or "???")
#	    print filename

	    filename[0]=filename[1] or filename[2]
	    if filename[0]:
		filename[0]=safefilename(filename[0])
	    else:
		if not filename[3] or filename[3]=="text/plain":
		    filename[0]="NONAME.txt"
		elif filename[3]=="text/html":
		    filename[0]="NONAME.htm"
		elif filename[3]=="text/richtext" or filename[3]=="application/rtf":
		    filename[0]="NONAME.rtf"
		elif filename[3]=="message/rfc822" or filename[3]=="message/rfc822-headers" or filename[3]=="text/rfc822-headers":
		    filename[0]="NONAME.eml"
		elif filename[3]=="image/gif":
		    filename[0]="NONAME.gif"
		elif filename[3]=="image/jpeg" or filename[3]=="image/jpg" or filename[3]=="image/pjpeg":
		    filename[0]="NONAME.jpg"
		elif filename[3]=="image/bmp":
		    filename[0]="NONAME.bmp"
		elif filename[3]=="image/png" or filename[3]=="image/x-png":
		    filename[0]="NONAME.png"
		elif filename[3]=="application/zip" or filename[3]=="application/x-zip-compressed":
		    filename[0]="NONAME.zip"
		elif filename[3]=="application/pdf":
		    filename[0]="NONAME.pdf"
		elif filename[3]=="application/pgp-signature":
		    filename[0]="NONAME.pgp"
		elif filename[3]=="application/msword" or filename[3]=="application/x-ms-word" or filename[3]=="application/vnd.ms-word":
		    filename[0]="NONAME.doc"
		elif filename[3]=="application/msexcel" or filename[3]=="application/x-ms-excel" or filename[3]=="application/vnd.ms-excel":
		    filename[0]="NONAME.xls"
		elif filename[3]=="text/x-vcard":
		    filename[0]="NONAME.vcf"
		else:
		    filename[0]="NONAME.dat"
		filename[1]=filename[0]
#		    print 'NOFILENAME="'+header+'"'
	    filename[0]="extracted."+str(lineno)+"__"+filename[0]
#	    print "PARSEDHDR: file="+filename

#	    print "PARSEDHDR: coding="+(coding or "-")
	    decoder=None
	    binhex=0
	    if coding:
		coding=coding.strip().lower()
#		print "ENCODING: "+coding
		if coding=="quoted-printable":
		    decoder=binascii.a2b_qp
		elif coding=="base64":
		    decoder=binascii.a2b_base64
		elif coding in ('x-uuencode', 'uuencode', 'uue', 'x-uue'):
		    decoder=binascii.a2b_uu
		elif filename[3]=="application/mac-binhex40":
		    binhex=1
#		elif coding!="7-bit" and coding!="8-bit":
#		    print "CODING: unkwnown: "+coding
	    checkend=0
	    in_header=0
	    header=[]
	    outfile=open(filename[0],"w")
	    filelist.append(filename[0])
	    continue
    else:
	# check for new mail:
#	if line[0:5]=="From ":
#	    if outfile:
#		outfile.close()
#		ret|=chkfile(filename)
#	    bounds={}
#	    in_header=1
#	    print "HDR: unix-from found, switching to in_header"
	# check for boundaries:
	for b in bounds.keys():
	    bpos=line.find("--"+b)
	    if bpos>=0:
		if outfile:
		    outfile.close()
		    outfile=None
		    ret|=chkfile(filename)
		in_header=1
#		print "HDR: boundary found, switching to in_header"
	#
	if in_header:
	    continue
	# handle forwarded/bounced MIME emails:
#	spos=line.find(' ')
#	fpos=line.find(':',0,spos)
#	if fpos>=0 and fpos<spos:
	lline=line.strip().lower()
	fpos=lline.find(':')
	if fpos>0:
	    field=lline[0:fpos]
#	    print 'FIELD: "'+field+'"'
	    if field in ("content-type","content-transfer-encoding","content-disposition","content-id"):
		if outfile:
		    outfile.close()
		    outfile=None
		    ret|=chkfile(filename)
		in_header=1
#		print "HDR: looks like header, switching to in_header"
	# handle inline uuencoded/base64 block:
	if checkend and decoder==binascii.a2b_base64:
	    if not lline or lline[0:4]=="====":
		decoder=None
		outfile.close()
		ret|=chkfile(filename)
		outfile=oldoutfile
		filename=oldfilename
		checkend=0
		continue
	if lline[0:40]=="(this file must be converted with binhex":
	    binhex=1
	    continue
	if lline[0:6]=="begin " or lline[0:13]=="begin-base64 ":
	    begin,mode,name=line.split(' ',2)
	    try:
	      if int(mode,8)>0 and name:
		oldoutfile=outfile
		oldfilename=filename
		filename=[None,None,None,None]
		filename[1]=unicode(name.strip(),"latin-1")
		filename[0]="extracted."+str(lineno)+"__"+safefilename(filename[1])
#		print "INLINE: type="+begin+" file="+filename
		outfile=open(filename[0],"w")
		filelist.append(filename[0])
		if begin.lower()=="begin":
		    decoder=binascii.a2b_uu
		else:
		    decoder=binascii.a2b_base64
		checkend=1
		continue
	    except:
	        checkend+=0
    if in_header:
#	if line[0:5]=="From ":
#	    print "HDR: unix-from found, resetting bounds[]"
#	    bounds={}
#	else:

	# strip CRLF:
	c=line[-1:]
	if c in ('\r','\n'):
	    line=line[0:-1]
	    if line[-1:] in ('\r','\n') and c!=line[-1:]:
		line=line[0:-1]
	# do the header un-folding:
	if line[0:1] in (' ','\t'):
	    if header:
		header[-1]+=line
	    else:
#		print "syntax error: bad unfolded header line: '"+line+"'"
		mimeerror+=2
	else:
	    header.append(line)
    else:
	if decoder:
	  # Base64/UUEncode:
	  try:
	    outfile.write(decoder(line))
	  except:
	    if checkend and line.strip().lower()[0:3]=="end":
		decoder=None
		outfile.close()
		ret|=chkfile(filename)
		outfile=oldoutfile
		filename=oldfilename
		checkend=0
		continue
#	    print "Exception!!! while attachment decoding (line %d):" % (lineno)
#	    traceback.print_exc()
	    outfile.write(line)
	elif not binhex:
	    # Plain Text:
	    outfile.write(line)
	else:
	    # binhex:
	    line=line.strip()
	    if line:
		if binhex==1:
		    cp=line.find(':')
		    if cp<0:
			print "BinHex: missing colon-char at begin of BinHex file!"
		    else:
			binhexline=line[cp+1:]
			binhexdata=""
			binhex=2
		else:
		    binhexline+=line
	    if binhex==2 and binhexline:
		l=len(binhexline)
		eof=0
		while l>0:
#		    print "l=%d" % (l)
		    try:
			curbin,eof=binascii.a2b_hqx(binhexline[:l])
#			print "BinHex: %d bytes decoded from %d/%d chars, EOF: %d" % (len(curbin),l,len(binhexline),eof)
			binhexdata+=curbin
			binhexline=binhexline[l:]
			break
		    except binascii.Incomplete:
			l-=1
		if eof:
#		    print "BinHex: done hqx decoding, size: %d" % (len(binhexdata))
		    binhexdata=binascii.rledecode_hqx(binhexdata)
		    hqxsize=len(binhexdata)
#		    print "BinHex: done rle decoding, size: %d" % (hqxsize)
		    # check for the header:
		    fnlen=ord(binhexdata[0])
#		    print "BinHex fnlen=%d" % (fnlen)
		    if hqxsize<=22+fnlen:
#			print "BinHex: invalid header / file truncated! writting out raw data."
			hqxfilename=[None,"BinHex.dat",None,None]
			hdrlen=0
		    else:
			hqxfilename=[None,unicode(binhexdata[1:1+fnlen].strip(),"latin-1"),None,binhexdata[2+fnlen:2+fnlen+4]]
			hdrlen=fnlen+22
			hqxsize-=hdrlen
			dsize,=struct.unpack('>l',binhexdata[fnlen+12:fnlen+16])
#			print "BinHex: dsize=%d" % (dsize)
			if dsize+8>=hqxsize:
			    hqxsize=dsize
		    hqxfilename[0]="extracted."+str(lineno)+"__"+safefilename(hqxfilename[1])
#		    print hqxfilename
		    hqxoutfile=open(hqxfilename[0],"w")
		    filelist.append(hqxfilename[0])
		    hqxoutfile.write(binhexdata[hdrlen:hdrlen+dsize])
		    hqxoutfile.close()
		    ret|=chkfile(hqxfilename)
		    binhex=0

  rawout.close()

except:
#  print "Exception!!! while message parsing (line %d):" % (lineno)
#  traceback.print_exc()
  # copy the rest:
  while 1:
    line=file.readline()
    if not line:
      break
    rawout.write(line)
  rawout.close()
# os.system("/usr/local/bin/uudeview -i +a -m -d +o message.eml")

# print "Result: "+str(ret)
print "path=" + path

sys.stdout.flush()

#os.system("ls -la")

# check for virii with clamav:
#retcode=os.system("/usr/local/bin/clamscan "+path+" 2>&1")
#if retcode==256:
#    ret|=4

#retcode=os.system("/usr/local/bin/fsav --dumb "+path+" 2>&1")
#if retcode==(3<<8):
#    ret|=8
#elif retcode==(8<<8):
#    ret|=1

#retcode=os.system("/usr/local/bin/vbuster -s -f -a -t -kx -vx -hx -h75 -hm50 -hmx "+path+" 2>&1")
#if retcode==256:
#    ret|=16

sys.stdout.flush()

# cleanup temp:
#if ret==0:
#  try:
#    for filename in filelist:
#	os.remove(filename)
#    try:
#	os.rmdir(path+"/viruses")
#	os.remove(path+"/vbuster.log")
#    except:
#	print "no vbuster logs deleted"
#    os.rmdir(path)
#  except:
#    try:
#      os.rmdir(path)
#    finally:
#      traceback.print_exc()

#print "email scan result: %d (%d)  mime errors: %d" % (ret,ret&3,mimeerror)

if ret>=4:
    ret=102
elif ret>=2 or mimeerror>=10:
    ret=101
elif ret==1 or mimeerror>2:
    ret=100

# exitcode: 0=ok 1=error 100=susp-rename 101=ext-block 102=virus-block
sys.exit(ret)
