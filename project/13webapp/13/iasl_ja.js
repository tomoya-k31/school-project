////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2003-2016 Shusuke Okamoto, All rights reserved.
//
// iasl_ja.js:
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//   notice and this list of conditions.
// 2. All advertising materials mentioning features or use of this 
//   software must display the following acknowledgement:
//
//      This product includes software developed by Shusuke Okamoto 
//
// 3. The name of Shusuke Okamoto should not be used to endorse or promote
//   products derived from this software without specific prior
//   written permission.
////////////////////////////////////////////////////////////////////////////
// canvas 2d version
var iasl_init = new Object;        // ancester of all characters
var iasl_inst  = new Array();      // array of instance
var iasl_pinst = new Array();
var iasl_event_curr = new Array();
var iasl_event_prev = new Array();
var iasl_reuse_id = new Array();  // id manager to reuse
var iasl_max_id = 0;

var iasl_mouse = new Array();
var iasl_mousemove = new Object();

var iasl_timer = 0;
var iasl_timeritvl = 300;
var iasl_key = 0;
var iasl_bumped   = 0;
var iasl_bumped_r = 1;
var iasl_bumped_l = 2;
var iasl_bumped_u = 3;
var iasl_bumped_d = 4;
var iasl_clicked_l = 1;
var iasl_clicked_m = 2;
var iasl_clicked_r = 3;
var iasl_instnum = 0;
var iasl_quit = false;
var boardx0 = 0;
var boardy0 = 0;
var boardx1 = 0;
var boardy1 = 0;
var boardwd = 0;	// board width
var boardht = 0;	// board height
var iaslDefaultStatus = "Welcome to IASL world!";
var statusmsg = iaslDefaultStatus;

function setImage(e, src, angle, abs) {
    e.img.src = src;
    if (abs) e.iasl_angle  = angle;
    else     e.iasl_angle += angle;

    var x = 0;
    var y = 0;
    var x1 = e.iasl_x + e.img.width  - 1;
    var y1 = e.iasl_y + e.img.height - 1;
    if (x1 > boardwd) x = boardwd - x1;
    if (y1 > boardht) y = boardht - y1;
    if (x || y) move_rel(e, x, y);
}

/////////////////////////////////
function get_callee(p) {
   while (p && p.iasl_callee) p = p.iasl_callee;
   return p;
}

function get_caller(p) {
   if (p == iasl_init) return p;
   while (p && p.iasl_caller) p = p.iasl_caller;
   return p;
}
////////////////////////////////////
// move to absolute position
function move_abs(e,x,y){
    var w = e.img.width;
    var h = e.img.height;

    if(x < 0){ x = 0; e.iasl_bumped |= (1<<iasl_bumped_l);}
    if(y < 0){ y = 0; e.iasl_bumped |= (1<<iasl_bumped_u);}
    if(x+w >= boardwd) {
        x = boardwd - w - 1;
	e.iasl_bumped |= (1<<iasl_bumped_r);
    }
    if(y+h >= boardht) {
        y = boardht - h - 1;
	e.iasl_bumped |= (1<<iasl_bumped_d);
    }

    e.iasl_x = x;
    e.iasl_y = y;

    x += boardx0;
    y += boardy0;
}

// move relative
function move_rel(e,x,y) { move_abs(e, e.iasl_x + x, e.iasl_y + y); }

// furafura :-)
function rove(e,x,y){
    var xx, yy;
    if (x == 0)     xx = Math.random()* 2;
    else if (x < 0) xx = Math.random()*(-x/2);
    else            xx = Math.random()*(x/2);
    if (Math.round(Math.random()*100) % 2 != 0) xx = -xx;

    if (y == 0)     yy = Math.random()* 2;
    else if (y < 0) yy = Math.random()*(-y/2);
    else            yy = Math.random()*(y/2);
    if (Math.round(Math.random()*100) % 2 != 0) yy = -yy;

    move_abs(e, e.iasl_x + x + xx, e.iasl_y + y + yy);
}

function move_forward(e,x) {
   var dx, dy;
   if (e.iasl_angle < 0) e.iasl_angle += 360;
   dx = x * Math.cos(e.iasl_angle*Math.PI/180);
   dy = x * Math.sin(e.iasl_angle*Math.PI/180);
   move_abs(e, e.iasl_x+dx, e.iasl_y+dy);
}

function follow(c, cs, p, ps) {
    var c0 = c + cs - 1;
    var p0 = p + ps - 1;
    if (c <= p0 && p <= c0) return c;
    if (p < c) return c - (c-p0)/2;
    if (c < p) return c + (p-c0)/2;
    return c;
}

function follow_you(e, o) {
    var x = e.iasl_x;
    var y = e.iasl_y;
    var w = e.img.width;
    var h = e.img.height;
    var px = o.iasl_x;
    var py = o.iasl_y;
    var pw = o.img.width;
    var ph = o.img.height;

    x = follow(x, w, px, pw);
    y = follow(y, h, py, ph);
    move_abs(e, x, y);
}

function follow_other(e, o) {
    // find the nearest iaslChar
    var nearest = e;
    var mdist2 = boardwd*boardwd + boardht*boardht;
    var x0 = e.iasl_x + e.img.width/2;
    var y0 = e.iasl_y + e.img.height/2;
    var i;
    for (i = 0; i < iasl_inst.length; i++) {
        var ch = iasl_inst[i];
        if (ch.iasl_active == 0) continue;
        if (e == ch) continue;
        if (o != ch.iasl_img) continue;

        var x1 = ch.iasl_x + ch.img.width/2;
        var y1 = ch.iasl_y + ch.img.height/2;
        var dist2 = (x0-x1)*(x0-x1)+(y0-y1)*(y0-y1);
        if (nearest == e || dist2 < mdist2) {
           nearest = ch;
           mdist2 = dist2;
        }
    }

   // do move
   if (nearest != e) follow_you(e, nearest);
}

function follow_parent(e) {
    if (e.iasl_parent == iasl_init) return;
    var p = get_callee(e.iasl_parent);
    if (p != 0) follow_you(e, p);
}

function follow_mouse(e) {
   move_abs(e, iasl_mousemove.x - e.img.width/2,
               iasl_mousemove.y - e.img.height/2);
}

////////////////////////////////////////////////////
function getRandX(){
    var x = Math.random()*(boardwd);
    return Math.round(x);
}

function getRandY(){
    var y = Math.random()*(boardht);
    return Math.round(y);
}

function gamble(x){
    return (x > Math.floor(Math.random()*100)) ? 1:0;
}


////////////////////////////////////////////
function ret_from_macro(p, e){
    p.iasl_return = 1;
    p.iasl_callee = 0;
    move_abs(p, e.iasl_x, e.iasl_y);
    p.iasl_bumped = e.iasl_bumped;
    p.iasl_clicked = e.iasl_clicked;

    p.iasl_bumpedlist = new Array();
    var i;
    for (i = 0; i < e.iasl_bumpedlist.length; i++) 
        p.iasl_bumpedlist.push(e.iasl_bumpedlist[i]);

    var func = p.iasl_func; // to countup iasl_count
    func(p, 0, 1);   // transition, no-repeat
    p.iasl_bumped = 0;
    p.iasl_clicked = 0;
    p.iasl_bumpedlist = new Array();
    if (func == p.iasl_func) ++ p.iasl_count;
    else                        p.iasl_count = 0;
    if (p.iasl_func == 0) p.iasl_return = 0;
}

function unlink_from_family_chain(e) {
    var i, j;
    -- iasl_instnum;
    if (e.iasl_caller != 0) return;
    // unlink this from sibling list
    var sib = e.iasl_sibling;
    for (i = 0; i < sib.length; i++)
        if (sib[i] == e) break;
    sib.splice(i,1);        // remove it

    if (sib.length == 0) {
       // unlink e's sibling from parent's children list
       var cld = e.iasl_parent.iasl_children;
       for (i = 0; i < cld.length; i++)
           if (cld[i] == sib) break;
       cld.splice(i,1);
    }

    // replace each children's parent with e's parent
    for (i = 0; i < e.iasl_children.length; i++) {
        sib = e.iasl_children[i];
        for (j = 0; j < sib.length; j++) {
            sib[j].iasl_parent = e.iasl_parent;
        }
    }
    e.iasl_parent.iasl_children =
    e.iasl_parent.iasl_children.concat(e.iasl_children);
}

function freeInstance(e){
    if (e.iasl_caller != 0)
       ret_from_macro(e.iasl_caller, e);
    else
       unlink_from_family_chain(e);

    e.iasl_active = 0;
    e.iasl_func = 0;
}

function addEventListener(o, type, handler) {
    if (document.all) { // e4,e5,e6,o6
       if (type == "mousedown") o.onmousedown = handler;
       else if (type == "mousemove") o.onmousemove = handler;
       else alert("addEventListener: not supported");
    }
    else if (o.addEventListener)      // Mozilla
       o.addEventListener(type, handler, true);
    else
       alert("addEventListener: not supported");
}

function mousedown_handler(e){
    var x = new Object();
    if (document.all) {                      // e4,e5,e6,o6
       x.x = event.offsetX;
       x.y = event.offsetY;
       if (event.button == 1) 
          x.clicked = iasl_clicked_l;
       if (event.button == 2) 
          x.clicked = iasl_clicked_r;
       if (event.button == 3 || event.button ==4) 
          x.clicked = iasl_clicked_m;
    } else {                                 // n6
       x.clicked = e.which;
       x.x = e.layerX;
       x.y = e.layerY;
    }
    iasl_mouse.push(x);
    return false;
}

function mousemove_handler(e){
    if (document.all) {                      // e4,e5,e6,o6
       iasl_mousemove.x = event.offsetX;
       iasl_mousemove.y = event.offsetY;
    } else {
       iasl_mousemove.x = e.layerX;
       iasl_mousemove.y = e.layerY;
    }
    return false;
}

function allocInstance(){
    var id;
    if (iasl_reuse_id.length > 0)
       id = iasl_reuse_id.pop();
    else
       id = "iasl_inst" + iasl_max_id++; 

    var e = new Object();
    e.img = new Image();

    iasl_pinst.push(e);
    return e;
}

function iasl_activate_inst(e, func, src, parent, caller, sib, children) {
    e.iasl_active     = 1;
    e.iasl_clicked    = 0;
    e.iasl_bumped     = 0;
    e.iasl_count      = 0;
    e.iasl_func       = func;
    e.iasl_img        = src;
    e.iasl_parent     = parent
    e.iasl_caller     = caller;
    e.iasl_callee     = 0;
    e.iasl_return     = 0;
    e.iasl_bumpedlist = new Array();
    e.iasl_pending    = 0;
    e.iasl_sibling    = sib;
    e.iasl_children   = children;
    e.iasl_event_prev = new Array();
    e.iasl_event_curr = new Array();
    e.iasl_angle      = 0;
}

function iasl_macro(p, src, func){
    p.iasl_active = 0;
    var e = allocInstance();
    var org = get_caller(p);
    iasl_activate_inst(e, func, src, org.iasl_parent, org, 
                       org.iasl_sibing, org.iasl_children);
    p.iasl_callee = e;
    move_abs(e, p.iasl_x, p.iasl_y);
    e.iasl_func(e, 1, 1);    // output, no-repeat
    return e;
}

function iasl_fork(p, src, func, sib){
    var x, y;
    if (p != iasl_init) {
         x = p.iasl_x;
         y = p.iasl_y;
    } else { x = y = 0; }
    var e = allocInstance();
    var parent = get_caller(p);
    iasl_activate_inst(e, func, src, parent, 0, sib, new Array());
    sib.push(e);          // regist itself
    move_abs(e, x, y);
    e.iasl_func(e, 1, 1);    // output, no-repeat
    ++ iasl_instnum;
    return e;
}


///////////////////////////
function iasl_event_next(e) {
    if (e != 0) {
       e.iasl_event_prev = e.iasl_event_curr;
       e.iasl_event_curr = new Array();
    } else {
       iasl_event_prev = iasl_event_curr;
       iasl_event_curr = new Array();
    }
}

function iasl_event_cause(e, evt) {
    var i;
    if (e != 0) {
       for (i = 0; i < e.iasl_event_curr.length; i++)
           if (evt == e.iasl_event_curr[i]) return;
       e.iasl_event_curr.push(evt);
    } else {
       for (i = 0; i < iasl_event_curr.length; i++)
           if (evt == iasl_event_curr[i]) return;
       iasl_event_curr.push(evt);
    }
}

function iasl_event_notice(e, evt) {
}

function iasl_event_is_happened(e, evt) {
    var i;
    for (i = 0; i < e.iasl_event_prev.length; i++)
        if (evt == e.iasl_event_prev[i]) return true;
    for (i = 0; i < iasl_event_prev.length; i++)
        if (evt == iasl_event_prev[i]) return true;
    return false;
}

function iasl_event_cause_parent(e, evt) {
    if (e.iasl_parent == iasl_init) return;
    var p = get_callee(e.iasl_parent);
    iasl_event_cause(p, evt);
}

function iasl_event_cause_sibling(e, evt) {
    var i;
    for (i = 0; i < e.iasl_sibling.length; i++)
        iasl_event_cause(e.iasl_sibling[i], evt);
}

function iasl_event_cause_children(e, evt) {
    var i, j;
    for (i = 0; i < e.iasl_children.length; i++) {
        var sib = e.iasl_children[i];
        for (j = 0; j < sib.length; j++)
            iasl_event_cause(sib[j], evt);
    }
}

///////////////////////////
function is_orphan(e) { return (e.iasl_parent == iasl_init); }

function isBumpedObj(e,o) {
    var i;
    for (i = 0; i < e.iasl_bumpedlist.length; i++) {
        var x = e.iasl_bumpedlist[i];
        if (o == x.iasl_img) return true;
    }
    return false;
}


///////////////////////////
function checkBumped(){
    var i, j;
    for (i = 0; i < iasl_inst.length; i++) {
        var e0 = iasl_inst[i];
        if (e0.iasl_active == 0) continue;

        iasl_event_next(e0); // piggybacked

        var x0 = e0.iasl_x + e0.img.width  - 1;
        var y0 = e0.iasl_y + e0.img.height - 1;;
        for (j = i+1; j < iasl_inst.length; j++) {
            var e1 = iasl_inst[j];
            if (e1.iasl_active == 0) continue;
            var x1 = e1.iasl_x + e1.img.width  - 1;
            var y1 = e1.iasl_y + e1.img.height - 1;
            if (e1.iasl_x <= x0 && e0.iasl_x <= x1
             && e1.iasl_y <= y0 && e0.iasl_y <= y1) {
                e0.iasl_bumped |= (1<<iasl_bumped);
		e0.iasl_bumpedlist.push(e1);
                e1.iasl_bumped |= (1<<iasl_bumped);
		e1.iasl_bumpedlist.push(e0);
            }
        }
    }
}

function checkClicked(e) {
    var i, x, y;
    for (i = 0; i < iasl_mouse.length; i++) {
        x = iasl_mouse[i].x;
        y = iasl_mouse[i].y;
        if (x >= e.iasl_x && x <= e.iasl_x+e.img.width
         && y >= e.iasl_y && y <= e.iasl_y+e.img.height) {
           e.iasl_clicked = iasl_mouse[i].clicked;	
           break;
        }
    }
}

function iaslTimeoutFunc(){ 
    // clear mainboard
    var e = document.getElementById('mainboard');
    if (!e.getContext) return;
    var conObj = e.getContext("2d");
    conObj.clearRect(0,0,boardwd,boardht);

    // update status message bar
    var e = document.getElementById('statusbar');
    e.replaceChild(document.createTextNode(statusmsg), e.childNodes[0] );

    // push new instances onto list
    // while (iasl_pinst.length > 0) iasl_inst.push( iasl_pinst.pop() );
    for (var i = 0; i < iasl_pinst.length; i++)
        iasl_inst.push( iasl_pinst[i] );
    iasl_pinst.length = 0;  // delete all elements
    


    checkBumped();
    iasl_event_next(0);
    var n = 0;
    while (n < iasl_inst.length) {
        e = iasl_inst[n];

        if (e.iasl_active == 0 && e.iasl_callee == 0 && e.iasl_return == 0) {
           iasl_inst.splice(n,1);   // remove it
           if (e.id == "iasl_inst" + (iasl_max_id-1))
              -- iasl_max_id;
           else
              iasl_reuse_id.push(e.id);
           continue;
        }

        if (e.iasl_active) {
            checkClicked(e);
            var func = e.iasl_func;
            e.iasl_func(e, 0, 0); // transition
            e.iasl_bumped = 0;
            e.iasl_clicked = 0;	
            e.iasl_bumpedlist = new Array();
            if (e.iasl_active == 0) { ++ n; continue; }
            e.iasl_func(e, 1, 0); // output, repeatable
            if (func == e.iasl_func)
                ++ e.iasl_count;
            else
                e.iasl_count = 0;
        }

        if (e.iasl_return) {
           // e.style.visibility = 'visible';
           e.iasl_return = 0;
           e.iasl_active = 1;
           e.iasl_func(e, 1, 1); // output, no-repeat
        }

        if (e.iasl_active && e.img.complete) {
           var xc = e.img.width/2;
           var yc = e.img.height/2;
           conObj.save();
           conObj.translate(e.iasl_x + xc, e.iasl_y + yc);
           conObj.rotate(e.iasl_angle * Math.PI/180);
           conObj.drawImage(e.img, -xc, -yc );
           conObj.restore();
        }
        ++n;
    }
    iasl_key = 0;
    iasl_mouse = new Array();
    if (!iasl_quit)
    iasl_timer = setTimeout("iaslTimeoutFunc()", iasl_timeritvl); 
} 
///////////////////////////

function start_iasl_timer() {
    if (iasl_timer==0)
        iasl_timer = setTimeout("iaslTimeoutFunc()", iasl_timeritvl); 
    iasl_quit = false;
    return false;
}

function stop_iasl_timer() {
    if (iasl_timer) clearTimeout(iasl_timer);
    iasl_timer = 0;
    iasl_quit = true;
    return false;
}

function onkeydown_handler(){
    if (event.keyCode>=65 && event.keyCode <=90)
       return false;
    iasl_key = event.keyCode;
    return false;
}

function keypress_handler(e){
    if (document.all)  // e4,e5,e6,o6
       iasl_key = event.keyCode;
    else if (document.getElementById) 
       iasl_key = (e.keyCode!=0)?e.keyCode:e.charCode;
    else if(document.layers)  
       iasl_key = e.which;
    //   iasl_key = String.fromCharCode(iasl_key);
    // for safari
    if (iasl_key == 63234) iasl_key = 37; // left-arrow
    if (iasl_key == 63232) iasl_key = 38; // upper-arrow
    if (iasl_key == 63235) iasl_key = 39; // right-arrow
    if (iasl_key == 63233) iasl_key = 40; // lower-arrow
    return false;
}

function newboard(x,y,w,h,bgc) {
    /* stop appearing menu by right click */
    document.body.oncontextmenu = function(id) { return false; }

    // main board
    var e = document.createElement("canvas");
    if (!e.getContext) {
       alert('getContext not found');
       return;
    }
    e.setAttribute('id','mainboard');
    e.width  = w;
    e.height = h;
    document.body.appendChild(e);
    e.style.position   = 'absolute';
    e.style.visibility = 'visible';
    e.style.width  = w + 'px';
    e.style.height = h + 'px';
    e.style.left   = x + 'px';
    e.style.top    = y + 'px';
    e.style.background = bgc;

    addEventListener(e, "mousedown", mousedown_handler);
    addEventListener(e, "mousemove", mousemove_handler);

    document.onkeypress = keypress_handler;
    if (document.all) document.onkeydown = onkeydown_handler;

    boardx0 = x;
    boardy0 = y;
    boardwd = w;
    boardht = h;
    boardx1 = boardx0 + boardwd - 1;
    boardy1 = boardy0 + boardht - 1;

    // status bar
    var e = document.createElement("div");
    e.setAttribute('id','statusbar');
    e.appendChild(document.createTextNode(statusmsg));
    e.setAttribute('id','statusbar');
    e.style.position   = 'absolute';
    e.style.background  = bgc;
    var status_ht = 40;
    e.style.left   = boardx0 + 'px';
    e.style.top    = boardy1 + 1 + 'px';
    e.style.width  = boardwd + 'px';
    e.style.height = status_ht + 'px';
    e.style.fontSize = '15pt';
    document.body.appendChild(e);

    // start button
    e = document.createElement("div");
    e.setAttribute('id','startbutton');
    e.style.position   = 'absolute';
    e.style.left       = boardx0 + 'px';
    e.style.top        = boardy1 + status_ht + 1 + 'px';
    if (document.uniqueID) { // ie6
        e.innerHTML= "<form><input type='button' style='width:" +
         boardwd/2 + "px' value='Start' onclick='start_iasl_timer()'></form>";
    } else {
       var f1 = document.createElement('form');
       var i1 = document.createElement('input');
       i1.setAttribute('type', 'button');
       i1.setAttribute('style', 'width:' +  boardwd/2 + 'px');
       i1.setAttribute('value', 'Start');
       i1.setAttribute('onclick', 'start_iasl_timer()');
       f1.appendChild(i1);
       e.appendChild(f1);
    }
    document.body.appendChild(e);

    // stop button
    e = document.createElement("div");
    e.setAttribute('id','stopbutton');
    e.style.position   = 'absolute';
    e.style.left       = boardx0 + boardwd/2 + 'px';
    e.style.top        = boardy1 + status_ht + 1 + 'px';
    if (document.uniqueID) {  // ie6
        e.innerHTML= "<form><input type='button' style='width:" +
         boardwd/2 + "px' value='Stop' onclick='stop_iasl_timer()'></form>";
    } else {
        f1 = document.createElement('form');
        i1 = document.createElement('input');
        i1.setAttribute('type', 'button');
        i1.setAttribute('style', 'width:' +  boardwd/2 + 'px');
        i1.setAttribute('value', 'Stop');
        i1.setAttribute('onclick', 'stop_iasl_timer()');
        f1.appendChild(i1);
        e.appendChild(f1);
    }
    document.body.appendChild(e);


    // create ancester of all characters
    iasl_init.iasl_active = 0;
    iasl_init.iasl_x      = 0;
    iasl_init.iasl_y      = 0;
    iasl_init.caller      = 0;
    iasl_init.iasl_children   = new Array();
    iasl_init.iasl_event_curr = new Array();

    group_main(iasl_init);
    iaslTimeoutFunc();
}
