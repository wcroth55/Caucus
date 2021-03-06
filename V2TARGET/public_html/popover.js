//
//---popover.js     DHTML "pop overs".  Simulate a pop-up "window" on 
//                  mouseover or other event.
//   Purpose:
//      Provide the mechanism and functions for doing DHTML style
//      "pop over" little windows on "mouse over" of a link or other
//      event, without actually opening a new window.
//
//   Usage example:
//      <a href="..." onMouseOver="popOver('divId', true);" 
//                     onMouseOut="popOver('divId', false);">link text</a>
//      ...
//      <div id="divId" style="POSITION: absolute; visibility: hidden">
//         (pop-up text here)
//      </div>
//
//      Causes div 'divId' to pop (or "float") over the link text when the
//      user mouses-over the link, and to disappear again when the user
//      moves the mouse away.  WARNING -- 'divId' must be defined in the
//      top-level positioning context, i.e. it must not be within another
//      <div> or other element that is CSS positioned.  A good place to
//      put these <div>'s is right before the </body> tag.
//
//      The full calling sequence for popOver is actually:
//         popOver (divid, updown, [x, [y]]);
//
//      where x and y can optionally be used to set the location of the pop-up.
//      X and Y are specified as "R" (relative) or "A" (absolute), followed 
//      by a number. 
//
//      E.g. if x is "R32", the top-left corner of the pop-up appears 32
//      pixels to the right of the mouse position.  "R-10" means 10 pixels
//      to the left. "A100" means 100 pixels from the window left border,
//      and so on.  If A or R is not specified, assumes R.
//
//      Known to work with: IE 6 & 7, Netscape 6+, Mozilla 1.3+, FireFox
//      1.5 & 2, Safari.
//
//   Portions Copyright (C) 2004-2007 CaucusCare.com, permission granted 
//   to anyone to use for any purpose, so long as the copyright notice 
//   is retained.  No warranty for fitness for any purpose is granted.
//
//   Portions taken from Danny Goodman's JavaScript and DHTML cookbook,
//   with permission.
//
//   Version history.  
//:CR 12/07/04 New file, based very loosely on John Faucher's wizard script.
//:CR 12/22/04 Add "A" and "R" prefixes to x and y optional args 
//:CR 01/07/05 Finish "A" and "R" features.
//:CR 03/30/07 Explicitly use 'px' units when setting position.
//:CR 04/03/07 Rewrite to use page coordinates in a better way.
//--------------------------------------------------------------------------

//---Netscape/Mozilla needs explicit mouse capture.
//if (! document.all) document.captureEvents(Event.MOUSEMOVE)

//---Set-up to use popOverMouse function onMouseMove
document.onmousemove = popOverMouse;

var popOverY;
var popOverX;

function popOver (what, up) {  //---with optional args xoffset, yoffset
   xnum = 0;   xabs = false;
   ynum = 0;   yabs = false;
   if (arguments.length >= 3) {
      x  = arguments[2] + "";
      x  = x.toLowerCase();
      x0 = x.substring(0,1);
      if      (x0 == "a")  { xabs = true;   xnum = x.substring(1,100) - 0; }
      else if (x0 == "r")                   xnum = x.substring(1,100) - 0;
      else                                  xnum = x - 0;
   }
   if (arguments.length >= 4) {
      y  = arguments[3] + "";
      y  = y.toLowerCase();
      y0 = y.substring(0,1);
      if      (y0 == "a")  { yabs = true;   ynum = y.substring(1,100) - 0; }
      else if (y0 == "r")                   ynum = y.substring(1,100) - 0;
      else                                  ynum = y - 0;
   }

   obj = document.getElementById(what);
   if (obj == null) { confirm ("Error: no div named " + what);   return false; }

   if (up) {
      xmouse = popOverX;
      ymouse = popOverY;

      obj.style.left       = Math.max (0, xnum + (xabs ? 0 : xmouse)) + "px";
      obj.style.top        = Math.max (0, ynum + (yabs ? 0 : ymouse)) + "px";
      obj.style.visibility = 'visible';
   }

   else obj.style.visibility='hidden';

   return false;   //---in case we're called from onClick.
}

//---Retrieve mouse x-y positions.
function popOverMouse(e) {
   evt = (e) ? e : event;
   var coords = getPageEventCoords(evt);

   popOverX = coords.left;
   popOverY = coords.top;

   return true;
}

function getPageEventCoords(evt) {
   var coords = {left:0, top:0};
   if (evt.pageX) {
      coords.left = evt.pageX;
      coords.top  = evt.pageY;
   } else if (evt.clientX) {
      coords.left = 
         evt.clientX + document.body.scrollLeft - document.body.clientLeft;
      coords.top = 
         evt.clientY + document.body.scrollTop  - document.body.clientTop;
      // include html element space, if applicable
      if (document.body.parentElement && document.body.parentElement.clientLeft) {
         var bodParent = document.body.parentElement;
         coords.left += bodParent.scrollLeft - bodParent.clientLeft;
         coords.top  += bodParent.scrollTop  - bodParent.clientTop;
      }
   }
   return coords;
}
