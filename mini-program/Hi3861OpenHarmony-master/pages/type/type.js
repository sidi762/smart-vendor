// index.js
// 获取应用实例
Page({
  data: {
    motto: 'Hello World',
    test_text:'Pegasus',
    deviceTerminal:'控制设备',
    deviceOTA:'云端互联',
    deviceAI_Terminal:'AI智能终端',
    userInfo: {},
    hasUserInfo: false,
    canIUse: wx.canIUse('button.open-type.getUserInfo'),
    canIUseGetUserProfile: false,
    canIUseOpenData: wx.canIUse('open-data.type.userAvatarUrl') && wx.canIUse('open-data.type.userNickName') // 如需尝试获取用户信息可改为false
  },
  // 事件处理函数
  bindViewTap() {
    wx.navigateTo({
      url: '../logs/logs'
    })
  },

  bindViewTap_1:function() {
    wx.navigateTo({
      url: '/pages/msgSendRead/msgSendRead',
      success:function(res){
        console.log(res.data)
      },
      fail:function(){},
      complete:function(){}
    })
  },

  bindViewTap_2:function() {
    wx.navigateTo({
      url: '/pages/TencentCloud/TencentCloud',
      success:function(res){
        console.log(res.data)
      },
      fail:function(){},
      complete:function(){}
    })
  },

  onLoad() {
    if (wx.getUserProfile) {
      this.setData({
        canIUseGetUserProfile: true
      })
    }
  },
  getUserProfile(e) {
    // 推荐使用wx.getUserProfile获取用户信息，开发者每次通过该接口获取用户个人信息均需用户确认，开发者妥善保管用户快速填写的头像昵称，避免重复弹窗
    wx.getUserProfile({
      desc: '展示用户信息', // 声明获取用户个人信息后的用途，后续会展示在弹窗中，请谨慎填写
      success: (res) => {
        console.log(res)
        this.setData({
          userInfo: res.userInfo,
          hasUserInfo: true
        })
      }
    })
  },
  getUserInfo(e) {
    // 不推荐使用getUserInfo获取用户信息，预计自2021年4月13日起，getUserInfo将不再弹出弹窗，并直接返回匿名的用户个人信息
    console.log(e)
    this.setData({
      userInfo: e.detail.userInfo,
      hasUserInfo: true
    })
  },
   /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {

  },
  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {
    wx.navigateBack({
      delta: 1,
    })
  },
})
