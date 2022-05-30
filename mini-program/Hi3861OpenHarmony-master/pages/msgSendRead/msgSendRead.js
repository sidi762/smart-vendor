// const app = getApp()

var udp  =  wx.createUDPSocket();
var port = udp.bind(6655);//绑定本机端口号，端口号可以自己设置
var num = 0;
var light_flag =false;
var receive_flag = false;
var sendMsg_flag = false;

Page({
  data:{
    hostport:'6655',
    inputIp:'',
    inputPort:'',
    sendMsgbuff:'',
    message:'ReceiveMsg here',
    selects:['灯控制'],
    clickId:-1,
    lightOnMsg:'_light_on',
    lightOffMsg:'_light_off',
    unloadMsg:'UnoladPage'
  },

  changeColor:function(res) {
    if (this.data.clickId == res.currentTarget.id) {
      this.setData({
        clickId:-1
      })
      return;
    } else {
      this.setData({
        clickId:res.currentTarget.id
      })
    }
  },
    /**
    * 输入进来的ip地址，端口号和要发送的信息
    */
  addIp: function(res) {
    // this.setData({
    //   inputIp:res.detail.value
    // })
    this.data.inputIp=res.detail.value
  },
  addPort: function (res) {
    // this.setData({
    //   inputPort:res.detail.value
    // })
    this.data.inputPort=res.detail.value
  },
  addMessage(res) {
    // this.setData({
    //   sendMsgbuff:res.detail.value
    // })
    this.data.sendMsgbuff=res.detail.value
  },
  /*休眠函数*/
  sleep: function(milSec) {
    return new Promise(resolve => {
    setTimeout(resolve, milSec)
    })
  },

  onLoad() {
    var that = this;

    udp.onMessage(function (res) {
      console.log(res);
      var unit8Arr = new Uint8Array(res.message);
      var encodedString = String.fromCharCode.apply(null, unit8Arr),
      decodedString = decodeURIComponent(escape(encodedString)); //防止中文乱码
      that.setData({
        message:decodedString
      })
      /*接收到消息在界面上显示*/
      if (that.data.message != 'ReceiveMsg here') {
        receive_flag = true;
        sendMsg_flag = true;
        wx.showToast({
          title: '发送消息成功！',
        })
      }
    });
    /*监听数据包消息*/
    udp.onListening(function (res) {
      console.log(res);
    });
  },
  
  //判断输入数据的有效性
  JudgeDataValidity:function(){
    if (!this.data.inputIp) {
      wx.showModal({
        title: '提示',
        content:'请输入要连接设备的IP',
      })
      return false
    } else if (!this.data.inputPort) {
      wx.showModal({
        title: '提示',
        content:'请输入要连接设备的端口号',
      })
      return false
    } else if (!this.data.inputIp && !this.data.inputPort) {
      wx.showModal({
        title: '提示',
        content:'请输入要连接设备的IP和端口号',
      })
      return false
    } else if (!this.data.sendMsgbuff && !light_flag) {
        wx.showModal({
          title: '提示',
          content:'请输入要发送到设备的消息',
        })
        return false
    } else {
      return true
    }
  },

  SendMsg: function SendMsg() {
    light_flag = false;
    receive_flag = false;
    if (this.JudgeDataValidity()) {
      udp.send({
        address:this.data.inputIp,
        port:this.data.inputPort,
        message:this.data.sendMsgbuff,
      });
      /*设置延时等待对端数据过来 */
      setTimeout(function(){
        /*发送按键按下，没有接收到对端发来数据的界面显示*/
        if (!receive_flag) {//延时函数时间内，如果接收到数据会改变receive_flag状态
          wx.showToast({
            title: '发送消息失败！',
          })
        }
      }, 200);
    }
  },
  light_off: function light_off() {
    light_flag = true;
    receive_flag = false;
    if (this.JudgeDataValidity()) {
      udp.send({
        address:this.data.inputIp,
        port:this.data.inputPort,
        message:this.data.lightOnMsg,
      });
      /*设置延时等待对端数据过来 */
      setTimeout(function(){
        /*发送按键按下，没有接收到对端发来数据的界面显示*/
        if (!receive_flag) {//延时函数时间内，如果接收到数据会改变receive_flag状态
          wx.showToast({
            title: '打开照明失败！',
          })
        }
      }, 200);
   }
  },
  light_on: function light_on() {
    light_flag = true;
    receive_flag = false;
    if (this.JudgeDataValidity()) {
      udp.send({
        address:this.data.inputIp,
        port:this.data.inputPort,
        message:this.data.lightOffMsg,
      });
      /*设置延时等待对端数据过来 */
      setTimeout(function(){
        /*发送按键按下，没有接收到对端发来数据的界面显示*/
        if (!receive_flag) {//延时函数时间内，如果接收到数据会改变receive_flag状态
          wx.showToast({
            title: '关闭照明失败！',
          })
        }
      }, 200);
    }
  },
  unloadPage:function(){
    udp.send({
      address:this.data.inputIp,
      port:this.data.inputPort,
      message:this.data.lightOffMsg,
    });
  },
  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {

  },

  onUnload:function() {
    this.unloadPage();
    // udp.close();//退出页面时将socket关闭
  }
})

