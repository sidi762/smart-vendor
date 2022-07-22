Page({

  //监听页面显示
  
  onShow: function () {
  
      //自动跳转到login
  
      setTimeout(function(){
  
      //页面跳转相当于   
  
        wx.navigateBack({
          delta: 2
        })
  
      },2000);
  
    },
  
  })