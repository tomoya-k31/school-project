obj_nnn10 = new Array('nnn2.png', 'nnn0.png', 'nnn1.png', ''); // cat
obj_nnn18 = new Array('nnn3.png', 'nnn4.png', 'nnn5.png', 'nnn6.png', ''); // end
obj_nnn14 = new Array('nnn7.png', 'nnn8.png', 'nnn2.png', 'nnn9.png', ''); // kill

function trans_nnn10_nnn12(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[0], 45, 0);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    if (e.iasl_count+1 >= 10) { e.iasl_func = trans_nnn10_nnn11; return; }
    { e.iasl_func = trans_nnn10_nnn12; return; }
}

function trans_nnn10_nnn15(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[0], 45, 0);
        move_rel(e, 8, 8);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    if (e.iasl_bumped & (1<<iasl_bumped_d)) { e.iasl_func = trans_nnn10_nnn12; return; }
    if (e.iasl_bumped & (1<<iasl_bumped)
     && isBumpedObj(e, obj_nnn14) == true) { scoreAdd(); e.iasl_func = trans_nnn10_nnn11; return; }
    if (e.iasl_count+1 >= 10) { e.iasl_func = trans_nnn10_nnn13; return; }
    { e.iasl_func = trans_nnn10_nnn15; return; }
}

function trans_nnn10_nnn13(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[1], 0, 0);
        move_rel(e, 0, 8);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    if (gamble(50)) { e.iasl_func = trans_nnn10_nnn16; return; }
    { e.iasl_func = trans_nnn10_nnn15; return; }
}

function trans_nnn10_nnn17(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[2], 0, 0);
        move_abs(e,getRandX(),e.iasl_y);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    if (gamble(10)) { e.iasl_func = trans_nnn10_nnn16; return; }
    { e.iasl_func = trans_nnn10_nnn17; return; }
}

function trans_nnn10_nnn16(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[0], -45, 0);
        move_rel(e, -8, 8);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    if (e.iasl_bumped & (1<<iasl_bumped_d)) { e.iasl_func = trans_nnn10_nnn12; return; }
    if (e.iasl_bumped & (1<<iasl_bumped)
     && isBumpedObj(e, obj_nnn14) == true) { scoreAdd(); e.iasl_func = trans_nnn10_nnn11; return; }
    if (e.iasl_count+1 >= 10) { e.iasl_func = trans_nnn10_nnn13; return; }
    { e.iasl_func = trans_nnn10_nnn16; return; }
}

function trans_nnn10_nnn11(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[2], 0, 0);
        move_abs(e, 200, 0);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    { e.iasl_func = trans_nnn10_nnn17; return; }
}

function trans_nnn18_nnn19(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[0], 0, 0);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    { e.iasl_func = trans_nnn18_nnn19; return; }
}

function trans_nnn18_nnn22(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[1], 0, 0);
        follow_mouse(e);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    if (e.iasl_bumped & (1<<iasl_bumped)
     && isBumpedObj(e, obj_nnn10) == true) { e.iasl_func = trans_nnn18_nnn20; return; }
    if (e.iasl_bumped & (1<<iasl_bumped)
     && isBumpedObj(e, obj_nnn14) == true) { /*scoreAdd()*/; e.iasl_func = trans_nnn18_nnn20; return; }
    if (e.iasl_count+1 >= 7) { e.iasl_func = trans_nnn18_nnn21; return; }
    { e.iasl_func = trans_nnn18_nnn22; return; }
}

function trans_nnn18_nnn21(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[2], 0, 0);
        follow_mouse(e);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    if (e.iasl_clicked == iasl_clicked_l) { e.iasl_func = trans_nnn18_nnn23; return; }
    if (e.iasl_bumped & (1<<iasl_bumped)
     && isBumpedObj(e, obj_nnn10) == true) { e.iasl_func = trans_nnn18_nnn20; return; }
    if (e.iasl_bumped & (1<<iasl_bumped)
     && isBumpedObj(e, obj_nnn14) == true) { e.iasl_func = trans_nnn18_nnn20; return; }
    { e.iasl_func = trans_nnn18_nnn21; return; }
}

function trans_nnn18_nnn23(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[2], 0, 0);
        follow_mouse(e);
        if (delayflg) { e.iasl_pending = 1; return; }
        { var sibling = new Array();
          iasl_fork(e, obj_nnn14, trans_nnn14_nnn24, sibling);
          e.iasl_children.push(sibling); }
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    if (e.iasl_bumped & (1<<iasl_bumped)
     && isBumpedObj(e, obj_nnn10) == true) { e.iasl_func = trans_nnn18_nnn20; return; }
    { e.iasl_func = trans_nnn18_nnn22; return; }
}

function trans_nnn18_nnn20(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[3], 0, 0);
        rove(e, 1, 0);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    if (e.iasl_count+1 >= 9) { scoreStore(); e.iasl_func = trans_nnn18_nnn19; return; }
    { e.iasl_func = trans_nnn18_nnn20; return; }
}

function trans_nnn14_nnn24(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[0], 0, 0);
        move_rel(e, 12, -12);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    { e.iasl_func = trans_nnn14_nnn25; return; }
}

function trans_nnn14_nnn27(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[1], 15, 0);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    if (e.iasl_count+1 >= 8) { e.iasl_func = trans_nnn14_nnn26; return; }
    { e.iasl_func = trans_nnn14_nnn27; return; }
}

function trans_nnn14_nnn25(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[0], 0, 0);
        move_rel(e, 0, -5);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    if (e.iasl_bumped & (1<<iasl_bumped)
     && isBumpedObj(e, obj_nnn10) == true) { e.iasl_func = trans_nnn14_nnn27; return; }
    if (e.iasl_bumped & (1<<iasl_bumped)
     && isBumpedObj(e, obj_nnn18) == true) { freeInstance(e); return; }
    if (e.iasl_bumped & (1<<iasl_bumped_u)) { freeInstance(e); return; }
    { e.iasl_func = trans_nnn14_nnn25; return; }
}

function trans_nnn14_nnn28(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[2], 0, 0);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    { freeInstance(e); return; }
}

function trans_nnn14_nnn26(e, out, delayflg){
    if (out) { // action
        setImage(e, e.iasl_img[3], 0, 0);
        move_abs(e, 380, 580);
        return;
    }

    // transition
    if (e.iasl_pending) { e.iasl_pending = 0; return; }
    { freeInstance(e); return; }
}

function group_main(e) {
    var sibling = new Array();
    iasl_fork(e, obj_nnn18, trans_nnn18_nnn21, sibling);
    iasl_fork(e, obj_nnn10, trans_nnn10_nnn11, sibling);
    iasl_fork(e, obj_nnn10, trans_nnn10_nnn11, sibling);
    iasl_fork(e, obj_nnn10, trans_nnn10_nnn11, sibling);
    e.iasl_children.push(sibling);
}

function iasl_start(){
    iasl_timeritvl = 100;
    newboard(20,60,400,600,'#33CCCC');
}

// 得点の加点
function scoreAdd(){
  var score = Number($('#score').text());
  score = score+1;
  //console.log(score);
  $('#score').text(score);
}

function scoreStore(){
  var score = $('#score').text();
  //console.log(score);
  $.post("/cgi/store.cgi", "score="+score, function(result){
    //console.log(result);
  });
}
