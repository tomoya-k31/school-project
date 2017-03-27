$(function() {
    $('#post').keyup(function(){
      if($(this).val().length == 7){
          $.post("/cgi/data.cgi", $('#post').serialize()+"&flag=post", function(result){
            $('#addr').val(result);
        });
      }
    });

    $('#submit').click(function(){
      var flag=true;
      $('.entry').each(function(){
        if($(this).val()==""){
          alert("入力されていない項目があります");
          flag=false;
          return false;
        }
      });
      if(flag && $('input[name=gender]:checked').val() == null){
          alert("入力されていない項目があります");
          flag=false;
      }
      if(flag && !$('input[name=age]').val().match(/^[0-9]+$/)){
          alert("年齢は半角数字で入力してください");
          flag=false;
      }
      if(flag){
        var tmp = [];
        $('.entry').each(function(){
          tmp.push($(this).val());
        });
        tmp.push($('input[name=gender]:checked').val());

        $('.conf').each(function(){
          $(this).text(tmp.shift());
        });
        $('#input').hide();
        $('#confirm').show();
      }
    });

    $('#edit').click(function(){
        $('#confirm').hide();
        $('#input').show();
    });

    $('#send').click(function(){
        $.post("/cgi/data.cgi",  $('#form').serialize()+"&flag=fin", function(result){
            $('#confirm').hide();
            $('#fin').show();
        });
    });
});
