
Page({
  // 监听页面显示
  onShow: function () {
    // 自动跳转到login
    setTimeout(function () {
      wx.navigateTo({
        url: '../pay/pay'
      })
    }, 2000)
  }
})
