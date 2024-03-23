(function ($) {

    "use strict";

        // PRE loader
        $(window).load(function(){
          $('.preloader').fadeOut(1000); // set duration in brackets    
        });


        //Navigation Section
        $('.navbar-collapse a').on('click',function(){
          $(".navbar-collapse").collapse('hide');
        });

        $(window).scroll(function() {
          if ($(".navbar").offset().top > 50) {
            $(".navbar-fixed-top").addClass("top-nav-collapse");
              } else {
                $(".navbar-fixed-top").removeClass("top-nav-collapse");
              }
        });


        // Smoothscroll js
        $(function() {
          $('.custom-navbar a, #home a').bind('click', function(event) {
            var $anchor = $(this);
            $('html, body').stop().animate({
                scrollTop: $($anchor.attr('href')).offset().top - 49
            }, 1000);
            event.preventDefault();
          });
        });  

        $(document).ready(function() {
          $('form').on('submit', function(event) {
              event.preventDefault(); // 阻止表单的默认提交行为
      
              // 这里可以添加验证用户名和密码的代码
              // Get the values of the username and password fields
              var username = $('#username').val();
              var password = $('#password').val();

              // Perform validation
              if (username === 'admin' && password === 'password') {
                // Validation passed, proceed with form submission
                $('form').unbind('submit').submit();
                window.location.href = 'https://www.baidu.com';
              } 
              else 
              {
                // Validation failed, display an error message
                alert('Invalid username or password');
              }
      
              // 如果验证通过，跳转到新的页面
              
          });
      });


        // WOW Animation js
        new WOW({ mobile: false }).init();

})(jQuery);
