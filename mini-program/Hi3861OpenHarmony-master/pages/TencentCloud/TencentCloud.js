const app = getApp()

Page({
  data: {
    productId: app.globalData.productId,
    deviceName: app.globalData.deviceName,
    stateReported: {},
  }, 
  onLoad: function (options) {
    console.log("index onLoad")
    if (!app.globalData.productId) {
      wx.showToast({
        title: "产品ID不能为空",
        icon: 'none',
        duration: 3000
      })
      return
    } else if (!app.globalData.deviceName) {
      wx.showToast({
        title: "设备名称不能为空",
        icon: 'none',
        duration: 3000
      })
      return
    }
    // this.update()
  },
  update() {
    wx.showLoading()
    wx.cloud.callFunction({
      name: 'iothub-shadow-query',
      data: {
        ProductId: app.globalData.productId,
        DeviceName: app.globalData.deviceName,
        SecretId: app.globalData.secretId,
        SecretKey: app.globalData.secretKey,
      },
      success: res => {
        wx.showToast({
          icon: 'none',
          title: 'Subscribe完成，获取云端数据成功',
        })
        let deviceData = JSON.parse(res.result.Data)

        this.setData({
          stateReported: deviceData.payload.state.reported
        })
        console.log("result:", deviceData)
      },
      fail: err => {
        wx.showToast({
          icon: 'none',
          title: 'Subscribe失败，获取云端数据失败',
        })
        console.error('[云函数] [iotexplorer] 调用失败：', err)
      }
    })
  },
  switchChange(e) {
    let value = 0
    if (e.detail.value == true) {
      value = 1
    }
    let item = e.currentTarget.dataset.item
    let obj = {
      [`${item}`]: value
    }
    let payload = JSON.stringify(obj)
    JSON.parse
    console.log(payload)
    wx.showLoading()
    wx.cloud.callFunction({
      name: 'iothub-publish',
      data: {
        SecretId: app.globalData.secretId,
        SecretKey: app.globalData.secretKey,
        ProductId: app.globalData.productId,
        DeviceName: app.globalData.deviceName,
        Topic: app.globalData.productId + "/" + app.globalData.deviceName + "/data",
        Payload: payload,
      },
      success: res => {
        wx.showToast({
          icon: 'none',
          title: 'publish完成',
        })
        console.log("res:", res)
      },
      fail: err => {
        wx.showToast({
          icon: 'none',
          title: 'publish失败，请连接设备',
        })
        console.error('[云函数] [iotexplorer] 调用失败：', err)
      }
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

  },
    /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {

  },
  
  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {

  },
})
