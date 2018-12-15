if(!window.$){ window.$ = function (elem){return document.getElementById(elem);} }
try {document.execCommand("BackgroundImageCache", false, true);} catch(err) {}
//---------------------
var xform = $(x_form_id);
var show_fname_chars=56;
var UID,NF=0,cx=0,slots=1,fnames,scrOfX,scrOfY;
if(!window.x_tmpl_name)x_tmpl_name='';
if(!window.x_mode)x_mode=1;

var ext_allowed='',
    ext_not_allowed='',
    max_upload_files='1',
    max_upload_size='100',
    enable_file_descr=0,
    pass_required='',
    email_required='';

function openStatusWindow(f1,UID)
{ 
 var pbmode;
 if(f1.pbmode)
 {
     for(var i=0; i<f1.pbmode.length; i++)
     {
       if(f1.pbmode[i].checked)pbmode = f1.pbmode[i].value;
     }
     if(f1.pbmode.length == undefined)pbmode=f1.pbmode.value;
 }
 else
 {
     pbmode=3;
 }

 var cgi_url = f1.action.split('upload.cgi')[0]+'upload_status.cgi';
 var url = cgi_url+'?uid='+UID+'&nfiles='+NF+'&tmpl='+x_tmpl_name+'&xmode='+x_mode+'&files='+fnames;

 if(pbmode=='1') //pop-up
 {
   var win1 = window.open(url,"win1","width=390,height=320,resizable=1,status=0,left=450,top=250");
   win1.focus();
 }
 if(pbmode=='2') //lightbox 1
 {
   Viewport();
   $('xoverlay').style.display='block';
   $('xoverlay').style.height = window.pageY + 'px';
   $('xwindow2').style.display='block';
   $('xwindow2').style.marginLeft = -parseInt($('xwindow2').clientWidth/2)+'px';
   $('xwindow2').style.marginTop  = (window.scrollY - parseInt($('xwindow2').clientHeight/2))+'px';
   self.xiframe2.location = url+'&inline=1';
 }
 if(pbmode=='3') //lightbox 2
 {
   Viewport();
   $('xoverlay').style.display='block';
   $('xoverlay').style.height = window.pageY + 'px';
   $('xwindow3').style.display='block';
   $('xwindow3').style.marginLeft = -parseInt($('xwindow3').clientWidth/2)+'px';
   $('xwindow3').style.marginTop  = (window.scrollY - parseInt($('xwindow3').clientHeight/2))+'px';
   document.getElementsByTagName("body")[0].style.overflow = "hidden";
   self.xiframe3.location = url+'&inline=1';
 }
 if(pbmode=='4') //over the form
 {
   xy = findPos( $('div1') );
   var p4  = $('xiframe4');
   p4.style.display='block';
   p4.style.left = 1+parseInt(xy[0])+'px';
   p4.style.top = 1+parseInt(xy[1])+'px';
   p4.style.height = $('div1').clientHeight+'px';
   p4.style.width = $('div1').clientWidth + 'px';
   self.xiframe4.location = url+'&inline=1';
 }
 if(pbmode=='5')
 {
     if($('progress_bar'))$('progress_bar').style.display='block';
     if($('stop_btn'))$('stop_btn').style.visibility='visible';
     self.xiframe4.location = url+'&inline_pro=1';
 }
}

function StartUpload(f1)
{
    NF=0;
    f1.target='xupload';

    for (var i=0;i<f1.length;i++)
    {
      current = f1.elements[i];
      if(current.type=='file' && current.value!='')
      {
         if(!checkExt(current))return false;
         NF++;
      }
    }
    cx=0;
    fnames='';
    for (var i=0;i<=f1.length;i++)
    {
      current = f1[i];
      if(current && current.type && current.type=='file')
      {
         var descr = $(current.name+'_descr');
         if(descr)descr.name = 'file_'+cx+'_descr';
         current.name = 'file_'+cx;
         cx++;
         xname = current.value.match(/[^\\\/]+$/);
         if(xname && xname!='null')fnames+=':'+xname;
      }
    }

    if(NF==0){alert('Select at least one file to upload');return false;};
    if(pass_required && f1.upload_password && f1.upload_password.value==''){alert("Password required");return false;}
    if(email_required && f1.email_notification && !f1.email_notification.value.match(/^\w+[\+\.\w-]*@([\w-]+\.)*\w+[\w-]*\.([a-z]{2,4}|\d+)$/i)){alert("Valid email address required");return false;}
    if($('submit_btn'))$('submit_btn').style.display='none';
    var UID='';
    for(var i=0;i<12;i++)UID+=''+Math.floor(Math.random()*10);

    setTimeout( function (){openStatusWindow(f1,UID)}, 500);
    var password='';
    if(f1.upload_password)password=b64_md5(f1.upload_password.value);
    f1.action = f1.action.split('?')[0]+'?upload_id='+UID+'&js_on=1&xpass='+password+'&xmode='+x_mode; //cleaning old query to avoid ReUpload bugs
}

function checkExt(obj)
{
    value = obj.value;
    if(value=="")return false;
    var re1 = new RegExp("^.+\.("+ext_allowed+")$","i");
    var re2 = new RegExp("^.+\.("+ext_not_allowed+")$","i");
    if( (ext_allowed && !re1.test(value)) || (ext_not_allowed && re2.test(value)) )
    {
        str='';
        if(ext_allowed)str+="\nOnly these extensions are allowed: "+ext_allowed.replace(/\|/g,',');
	if(ext_not_allowed)str+="\nThese extensions are not allowed:"+ext_not_allowed.replace(/\|/g,',');
        alert("Extension not allowed for file: \"" + value + '"'+str);
        return false;
    }

    return true;
}

function addUploadSlot()
{
  cx++;
  slots++;
  if(slots==max_upload_files){$('x_add_slot').style.visibility='hidden';}

  var new_slot = document.createElement( 'input' );
  new_slot.type = 'file';
  new_slot.name = 'file_'+cx;
  $('upload_input').appendChild(new_slot);
  $('upload_input').appendChild( document.createElement('br') );
}

function fixLength(str)
{
 var arr = str.split(/\\/);
 str = arr[arr.length-1];
 if(str.length<show_fname_chars)return str;
 return '...'+str.substring(str.length-show_fname_chars-1,str.length);
}

function MultiSelector( list_obj )
{
	this.list_target = list_obj;
	this.count = 0;
	this.id = 0;
	this.addElement = function( element )
    {
       element.name = 'file_' + this.id++;
       element.multi_selector = this;
       element.onchange = function()
       {
           if(!checkExt(element))return;
           if (navigator.appVersion.indexOf("Mac")>0 && navigator.appVersion.indexOf("MSIE")>0)return;
           var new_element = document.createElement( 'input' );
           new_element.type = 'file';
           new_element.size = element.size;

           this.parentNode.appendChild( new_element, this );
           this.multi_selector.addElement( new_element );
           this.multi_selector.addListRow( this );

           this.style.position = 'absolute';
           this.style.left = '-9999px';
       };
       // Reached maximum number, disable input element
       if( this.count >= max_upload_files ){element.disabled = true;};

       this.count++;
       this.current_element = element;
	};

	this.addListRow = function( element )
    {
		var new_row = document.createElement( 'div' );
        new_row.className = 'xrow';

        var div1 = document.createElement( 'div' );
        div1.className = 'xfname';
        div1.appendChild( document.createTextNode(" "+fixLength(element.value)) );
        new_row.appendChild( div1 );

        var adel = document.createElement( 'a' );
        adel.href='#';
        adel.title='Delete';
        var img = document.createElement( 'img' );
        img.src='/del.gif';
        adel.appendChild( img );

		adel.onclick= function()
        {
			this.parentNode.element.parentNode.removeChild( this.parentNode.element );
			this.parentNode.parentNode.removeChild( this.parentNode );
			this.parentNode.element.multi_selector.count--;
			this.parentNode.element.multi_selector.current_element.disabled = false;
			return false;
		};

		new_row.appendChild( adel );

        var br = document.createElement( 'br' );
        br.setAttribute('clear', 'all');
        new_row.appendChild( br );
		
/*
        if(enable_file_descr==1) {
            var new_row_descr = document.createElement( 'input' );
    		new_row_descr.type = 'text';
            new_row_descr.name = element.name+'_descr';
            new_row_descr.className='fdescr';
            new_row_descr.setAttribute('maxlength', 128);
            var div2 = document.createElement( 'div' );
                div2.className = 'xdescr';
                div2.appendChild( document.createTextNode('Description:') );
            div2.appendChild( new_row_descr );
            new_row.appendChild( div2 );
        }
*/
        
        new_row.element = element;
		this.list_target.appendChild( new_row );
	};
};

function getPageSize()
{ 
             
         var xScroll, yScroll; 
         
        if (window.innerHeight && window.scrollMaxY) {     
            xScroll = window.innerWidth + window.scrollMaxX; 
            yScroll = window.innerHeight + window.scrollMaxY; 
        } else if (document.body.scrollHeight > document.body.offsetHeight){ // all but Explorer Mac 
            xScroll = document.body.scrollWidth; 
            yScroll = document.body.scrollHeight; 
        } else { // Explorer Mac...would also work in Explorer 6 Strict, Mozilla and Safari 
            xScroll = document.body.offsetWidth; 
            yScroll = document.body.offsetHeight; 
        } 
         
        var windowWidth, windowHeight; 
         
        if (self.innerHeight) {    // all except Explorer 
            if(document.documentElement.clientWidth){ 
                windowWidth = document.documentElement.clientWidth;  
            } else { 
                windowWidth = self.innerWidth; 
            } 
            windowHeight = self.innerHeight; 
        } else if (document.documentElement && document.documentElement.clientHeight) { // Explorer 6 Strict Mode 
            windowWidth = document.documentElement.clientWidth; 
            windowHeight = document.documentElement.clientHeight; 
        } else if (document.body) { // other Explorers 
            windowWidth = document.body.clientWidth; 
            windowHeight = document.body.clientHeight; 
        }     
         
        // for small pages with total height less then height of the viewport 
        if(yScroll < windowHeight){ 
            pageHeight = windowHeight; 
        } else {  
            pageHeight = yScroll; 
        } 
     
        // for small pages with total width less then width of the viewport 
        if(xScroll < windowWidth){     
            pageWidth = xScroll;         
        } else { 
            pageWidth = windowWidth; 
        } 
         
        return [pageWidth,pageHeight]; 
} 

function getScrollXY()
{
  if( typeof( window.pageYOffset ) == 'number1' ) {
    //Netscape compliant
    scrOfY = window.pageYOffset;
    scrOfX = window.pageXOffset;
  } else if( document.body && ( document.body.scrollLeft || document.body.scrollTop ) ) {
    //DOM compliant
    scrOfY = document.body.scrollTop;
    scrOfX = document.body.scrollLeft;
  } else if( document.documentElement && ( document.documentElement.scrollLeft || document.documentElement.scrollTop ) ) {
    //IE6 standards compliant mode
    scrOfY = document.documentElement.scrollTop;
    scrOfX = document.documentElement.scrollLeft;
  }
  if(!scrOfY)scrOfY=0;
  return [scrOfY,scrOfX];
}

function findPos(obj) {
	var curleft = curtop = 0;
	if (obj.offsetParent) {
		curleft = obj.offsetLeft
		curtop = obj.offsetTop
		while (obj = obj.offsetParent) {
			curleft += obj.offsetLeft
			curtop += obj.offsetTop
		}
	}
	return [curleft,curtop];
}

//--MD5 part ----
var b64pad  = "";
var chrsz   = 8;
function b64_md5(s){ return binl2b64(core_md5(str2binl(s), s.length * chrsz));}
function core_md5(x, len)
{
x[len >> 5] |= 0x80 << ((len) % 32);
x[(((len + 64) >>> 9) << 4) + 14] = len;
var a =  1732584193;
var b = -271733879;
var c = -1732584194;
var d =  271733878;
for(var i = 0; i < x.length; i += 16)
{
var olda = a;
var oldb = b;
var oldc = c;
var oldd = d;
a = md5_ff(a, b, c, d, x[i+ 0], 7 , -680876936);
d = md5_ff(d, a, b, c, x[i+ 1], 12, -389564586);
c = md5_ff(c, d, a, b, x[i+ 2], 17,  606105819);
b = md5_ff(b, c, d, a, x[i+ 3], 22, -1044525330);
a = md5_ff(a, b, c, d, x[i+ 4], 7 , -176418897);
d = md5_ff(d, a, b, c, x[i+ 5], 12,  1200080426);
c = md5_ff(c, d, a, b, x[i+ 6], 17, -1473231341);
b = md5_ff(b, c, d, a, x[i+ 7], 22, -45705983);
a = md5_ff(a, b, c, d, x[i+ 8], 7 ,  1770035416);
d = md5_ff(d, a, b, c, x[i+ 9], 12, -1958414417);
c = md5_ff(c, d, a, b, x[i+10], 17, -42063);
b = md5_ff(b, c, d, a, x[i+11], 22, -1990404162);
a = md5_ff(a, b, c, d, x[i+12], 7 ,  1804603682);
d = md5_ff(d, a, b, c, x[i+13], 12, -40341101);
c = md5_ff(c, d, a, b, x[i+14], 17, -1502002290);
b = md5_ff(b, c, d, a, x[i+15], 22,  1236535329);
a = md5_gg(a, b, c, d, x[i+ 1], 5 , -165796510);
d = md5_gg(d, a, b, c, x[i+ 6], 9 , -1069501632);
c = md5_gg(c, d, a, b, x[i+11], 14,  643717713);
b = md5_gg(b, c, d, a, x[i+ 0], 20, -373897302);
a = md5_gg(a, b, c, d, x[i+ 5], 5 , -701558691);
d = md5_gg(d, a, b, c, x[i+10], 9 ,  38016083);
c = md5_gg(c, d, a, b, x[i+15], 14, -660478335);
b = md5_gg(b, c, d, a, x[i+ 4], 20, -405537848);
a = md5_gg(a, b, c, d, x[i+ 9], 5 ,  568446438);
d = md5_gg(d, a, b, c, x[i+14], 9 , -1019803690);
c = md5_gg(c, d, a, b, x[i+ 3], 14, -187363961);
b = md5_gg(b, c, d, a, x[i+ 8], 20,  1163531501);
a = md5_gg(a, b, c, d, x[i+13], 5 , -1444681467);
d = md5_gg(d, a, b, c, x[i+ 2], 9 , -51403784);
c = md5_gg(c, d, a, b, x[i+ 7], 14,  1735328473);
b = md5_gg(b, c, d, a, x[i+12], 20, -1926607734);
a = md5_hh(a, b, c, d, x[i+ 5], 4 , -378558);
d = md5_hh(d, a, b, c, x[i+ 8], 11, -2022574463);
c = md5_hh(c, d, a, b, x[i+11], 16,  1839030562);
b = md5_hh(b, c, d, a, x[i+14], 23, -35309556);
a = md5_hh(a, b, c, d, x[i+ 1], 4 , -1530992060);
d = md5_hh(d, a, b, c, x[i+ 4], 11,  1272893353);
c = md5_hh(c, d, a, b, x[i+ 7], 16, -155497632);
b = md5_hh(b, c, d, a, x[i+10], 23, -1094730640);
a = md5_hh(a, b, c, d, x[i+13], 4 ,  681279174);
d = md5_hh(d, a, b, c, x[i+ 0], 11, -358537222);
c = md5_hh(c, d, a, b, x[i+ 3], 16, -722521979);
b = md5_hh(b, c, d, a, x[i+ 6], 23,  76029189);
a = md5_hh(a, b, c, d, x[i+ 9], 4 , -640364487);
d = md5_hh(d, a, b, c, x[i+12], 11, -421815835);
c = md5_hh(c, d, a, b, x[i+15], 16,  530742520);
b = md5_hh(b, c, d, a, x[i+ 2], 23, -995338651);
a = md5_ii(a, b, c, d, x[i+ 0], 6 , -198630844);
d = md5_ii(d, a, b, c, x[i+ 7], 10,  1126891415);
c = md5_ii(c, d, a, b, x[i+14], 15, -1416354905);
b = md5_ii(b, c, d, a, x[i+ 5], 21, -57434055);
a = md5_ii(a, b, c, d, x[i+12], 6 ,  1700485571);
d = md5_ii(d, a, b, c, x[i+ 3], 10, -1894986606);
c = md5_ii(c, d, a, b, x[i+10], 15, -1051523);
b = md5_ii(b, c, d, a, x[i+ 1], 21, -2054922799);
a = md5_ii(a, b, c, d, x[i+ 8], 6 ,  1873313359);
d = md5_ii(d, a, b, c, x[i+15], 10, -30611744);
c = md5_ii(c, d, a, b, x[i+ 6], 15, -1560198380);
b = md5_ii(b, c, d, a, x[i+13], 21,  1309151649);
a = md5_ii(a, b, c, d, x[i+ 4], 6 , -145523070);
d = md5_ii(d, a, b, c, x[i+11], 10, -1120210379);
c = md5_ii(c, d, a, b, x[i+ 2], 15,  718787259);
b = md5_ii(b, c, d, a, x[i+ 9], 21, -343485551);
a = safe_add(a, olda);
b = safe_add(b, oldb);
c = safe_add(c, oldc);
d = safe_add(d, oldd);
}
return Array(a, b, c, d);
}
function md5_cmn(q, a, b, x, s, t){return safe_add(bit_rol(safe_add(safe_add(a, q), safe_add(x, t)), s),b);}
function md5_ff(a, b, c, d, x, s, t){return md5_cmn((b & c) | ((~b) & d), a, b, x, s, t);}
function md5_gg(a, b, c, d, x, s, t){return md5_cmn((b & d) | (c & (~d)), a, b, x, s, t);}
function md5_hh(a, b, c, d, x, s, t){return md5_cmn(b ^ c ^ d, a, b, x, s, t);}
function md5_ii(a, b, c, d, x, s, t){return md5_cmn(c ^ (b | (~d)), a, b, x, s, t);}
function safe_add(x, y){var lsw = (x & 0xFFFF) + (y & 0xFFFF);var msw = (x >> 16) + (y >> 16) + (lsw >> 16);return (msw << 16) | (lsw & 0xFFFF);}
function bit_rol(num, cnt){return (num << cnt) | (num >>> (32 - cnt));}
function str2binl(str)
{
  var bin = Array();
  var mask = (1 << chrsz) - 1;
  for(var i = 0; i < str.length * chrsz; i += chrsz)
    bin[i>>5] |= (str.charCodeAt(i / chrsz) & mask) << (i%32);
  return bin;
}
function binl2b64(binarray)
{
  var tab = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  var str = "";
  for(var i = 0; i < binarray.length * 4; i += 3)
  {
    var triplet = (((binarray[i   >> 2] >> 8 * ( i   %4)) & 0xFF) << 16)
                | (((binarray[i+1 >> 2] >> 8 * ((i+1)%4)) & 0xFF) << 8 )
                |  ((binarray[i+2 >> 2] >> 8 * ((i+2)%4)) & 0xFF);
    for(var j = 0; j < 4; j++)
    {
      if(i * 8 + j * 6 > binarray.length * 32) str += b64pad;
      else str += tab.charAt((triplet >> 6*(3-j)) & 0x3F);
    }
  }
  return str;
}

function BlankIframes()
{
    blank_txt="<HTML><Body bgcolor='#FFFFFF'><Center><br><br>Initializing Upload...</Center></Body></HTML>";
    if(self.xiframe2){self.xiframe2.document.open();self.xiframe2.document.write(blank_txt);self.xiframe2.document.close();}
    if(self.xiframe3){self.xiframe3.document.open();self.xiframe3.document.write(blank_txt);self.xiframe3.document.close();}
    if(self.xiframe4){self.xiframe4.document.open();self.xiframe4.document.write(blank_txt);self.xiframe4.document.close();}
}

function StopUpload(no_close)
{
    var agt=navigator.userAgent.toLowerCase();
    var is_opera = (agt.indexOf("opera") != -1);
    if(self.xiframe2 && self.xiframe2.interval)self.xiframe2.clearTimeout(self.xiframe2.interval);
    if(self.xiframe3 && self.xiframe3.interval)self.xiframe3.clearTimeout(self.xiframe3.interval);
    if(self.xiframe4 && self.xiframe4.interval)self.xiframe4.clearTimeout(self.xiframe4.interval);
    if(!!top.execScript)
    {
        window.document.execCommand('Stop');
        window.frames['xupload'].document.execCommand('Stop');
    }
    else if (!document.all || is_opera)
    {
        window.stop();
        window.frames['xupload'].stop();
    }
    if(no_close)return;
    popupClose();
    return false;
}

function popupClose()
{
   //if( $('upload_form'))$('upload_form').style.position='static';
   if( $('submit_btn'))$('submit_btn').style.display='inline';
   if( $('xoverlay') )$('xoverlay').style.display='none';
   if( $('xwindow2') )$('xwindow2').style.display='none';
   if( $('xwindow3') )$('xwindow3').style.display='none';
   if( $('xiframe4') )$('xiframe4').style.display='none';
   if( $('progress_bar'))$('progress_bar').style.display='none';
   if( $('stop_btn'))$('stop_btn').style.visibility='hidden';
   if( window.SPI )window.SPI(0,0,0,0,0,0);
   BlankIframes();
   document.getElementsByTagName("body")[0].style.overflow = "";
   return false;
}

function Viewport(){ this.windowX = (document.documentElement && document.documentElement.clientWidth) || window.innerWidth || self.innerWidth || document.body.clientWidth; this.windowY = (document.documentElement && document.documentElement.clientHeight) || window.innerHeight || self.innerHeight || document.body.clientHeight; this.scrollX = (document.documentElement && document.documentElement.scrollLeft) || window.pageXOffset || self.pageXOffset || document.body.scrollLeft; this.scrollY = (document.documentElement && document.documentElement.scrollTop) || window.pageYOffset || self.pageYOffset || document.body.scrollTop; this.pageX = (document.documentElement && document.documentElement.scrollWidth) ? document.documentElement.scrollWidth : (document.body.scrollWidth > document.body.offsetWidth) ? document.body.scrollWidth : document.body.offsetWidth; this.pageY = (document.documentElement && document.documentElement.scrollHeight) ? document.documentElement.scrollHeight : (document.body.scrollHeight > document.body.offsetHeight) ? document.body.scrollHeight : document.body.offsetHeight;
}


// Safari fix
if(!document.all)window.frames['xupload'].stop();

//--Initialization------

document.write('<div id="xwindow2"><div id="xwindow2hdr">Upload in progress...<img id="xwindow2close" src="btn-close.gif" onclick="return StopUpload()" title="Close this window" alt="Close this window"></div><iframe src="" name="xiframe2" id="xiframe2" frameborder=0 SCROLLING=NO></iframe></div>');
document.write('<div id="xwindow3"><iframe src="" name="xiframe3" id="xiframe3" frameborder=0 SCROLLING=NO></iframe><a href="#" id="xwindow_close" onClick="return StopUpload()"></a></div>');
document.write('<iframe src="" name="xiframe4" id="xiframe4" frameborder=0 SCROLLING=NO></iframe>');
document.write('<div id="xoverlay"></div>');

document.write('<script src="' + xform.action +'?settings' + '&xmode=' +x_mode+ '" type="text/javascript"><\/script>');

if($('files_list'))
{
    var multi_selector = new MultiSelector( $('files_list') );
    multi_selector.addElement( $('my_file_element') );
}

BlankIframes();
