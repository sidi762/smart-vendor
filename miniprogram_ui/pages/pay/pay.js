
Page({
  // 监听页面显示
  onShow: function () {
    this.audioPlay()
    // 自动跳转到login
    setTimeout(function () {
    // 页面跳转
      wx.navigateBack({
        delta: 2
      })
    }, 2000)
  },
  audioPlay(){
    const successAudio = wx.createInnerAudioContext()
    successAudio.src = 'https://cdn.fgprc.org.cn/vendor/payment_success_edited.mp3'
    successAudio.obeyMuteSwitch = false
    successAudio.play()
    successAudio.onPlay(() => {
        console.log('开始播放')
      })
    successAudio.onError((res) => {
        console.log(res.errMsg)
        console.log(res.errCode)
      })
  },

})
