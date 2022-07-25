import mqtt from '../../utils/mqtt.min.js'

Page({
  data: {
    client: null,
    conenctBtnText: '连接',
    host: 'YT32IOSCAL.ap-guangzhou.iothub.tencentdevices.com:443',
    subTopic: 'YT32IOSCAL/UI/data',
    syncTopic: 'YT32IOSCAL/UI/sync',
    shadowTopic: '$shadow/operation/result/YT32IOSCAL/UI',
    shadowReqTopic: '$shadow/operation/YT32IOSCAL/UI',
    pubTopic: 'YT32IOSCAL/UI/event',
    receivedMsg: '',
    received: '',
    paying: 0,
    shadowMsg: '',
    shadow: '',
    shadowVersion: '',
    lastReportedToShadowMsg: '',
    name1: '口罩',
    name2: '酒精棉片',
    name3: '碘酒',
    name4: '创可贴',
    price1: 1,
    price2: 9.9,
    price3: 5.9,
    price4: 9.9,
    mqttOptions: {
      username: 'YT32IOSCALUI;12010126;88a3d;1664208000',
      password:
        '2788c933085788d342950273e749b9c5f328221ef752a3392c0f590ec660e7c0;hmacsha256',
      reconnectPeriod: 5000,
      connectTimeout: 10 * 1000,
      KeepAlive: 900
    }
  },

  setValue (key, value) {
    this.setData({
      [key]: value
    })
  },

  setHost (e) {
    this.setValue('host', e.detail.value)
  },
  setSubTopic (e) {
    this.setValue('subTopic', e.detail.value)
  },
  setPubTopic (e) {
    this.setValue('pubTopic', e.detail.value)
  },
  setPubMsg (e) {
    this.setValue('pubMsg', e.detail.value)
  },
  setRecMsg (msg) {
    this.setValue('receivedMsg', msg)
  },

  startDataUpdateTimer: function () {
    var that = this
    that.data.dataUpdateTimer = setInterval(function () {
      that.sendDataRequest()
    }, 1000)
  },

  sendDataRequest: function () {
    var requestMsg = {
      type: 'get',
      clientToken: 'clientToken'
    }
    this.data.client.publish(
      this.data.shadowReqTopic,
      JSON.stringify(requestMsg)
    )
  },

  connect () {
    try {
      wx.setNavigationBarTitle({
        title: '商品选择（连接中...）'
      })
      const clientId = new Date().getTime()
      this.data.client = mqtt.connect(`wxs://${this.data.host}/mqtt`, {
        ...this.data.mqttOptions,
        clientId
      })

      this.data.client.on('connect', () => {
        wx.setNavigationBarTitle({
          title: '商品选择'
        })
        this.subscribe()
        console.log('online')
      })

      this.data.client.on('message', (topic, payload) => {
        if (topic === 'YT32IOSCAL/UI/data') {
          const currMsg = this.data.receivedMsg ? `<br/>${payload}` : payload
          this.setValue('receivedMsg', this.data.receivedMsg.concat(currMsg))
          this.data.paying = 1
          wx.navigateTo({
            url: '../face/face'
          })
        }
        if (topic === '$shadow/operation/result/YT32IOSCAL/UI') {
          console.log('receivedMessage')
          const currMsg = this.data.shadowMsg ? `<br/>${payload}` : payload
          this.setValue('shadowMsg', this.data.shadowMsg.concat(currMsg))
          this.processShadowMessage()
          this.data.shadowMsg = ''
        }
        if (topic === 'YT32IOSCAL/UI/sync') {
          console.log('receivedMessage')
          const currMsg = this.data.shadowMsg ? `<br/>${payload}` : payload
          this.setValue('shadowMsg', this.data.shadowMsg.concat(currMsg))
          this.processSyncMessage()
          this.data.shadowMsg = ''
        }
      })

      this.data.client.on('error', (error) => {
        console.log('onError', error)
        wx.setNavigationBarTitle({
          title: '商品选择（连接失败）'
        })
      })

      this.data.client.on('reconnect', () => {
        console.log('reconnecting...')
        wx.setNavigationBarTitle({
          title: '商品选择（重连中...）'
        })
      })

      this.data.client.on('offline', () => {
        wx.setNavigationBarTitle({
          title: '商品选择（未连接）'
        })
        console.log('Offline')
      })
    } catch (error) {
      this.setValue('conenctBtnText', '连接')
      wx.setNavigationBarTitle({
        title: '商品选择（出现错误）'
      })
      console.log('mqtt.connect error', error)
    }
  },

  disconnect () {
    this.data.client.end()
    this.data.client = null
    this.setValue('conenctBtnText', '连接')
    wx.setNavigationBarTitle({
      title: '商品选择（未连接）'
    })
  },

  subscribe () {
    if (this.data.client) {
      this.data.client.subscribe(this.data.subTopic)
      this.data.client.subscribe(this.data.shadowTopic)
      this.data.client.subscribe(this.data.syncTopic)
      console.log('subscribe success')
    }
  },

  unsubscribe () {
    if (this.data.client) {
      this.data.client.unsubscribe(this.data.subTopic)
      this.data.client.unsubscribe(this.data.shadowTopic)
      this.data.client.unsubscribe(this.data.syncTopic)
    }
  },

  processShadowMessage () {
    const received = JSON.parse(this.data.shadowMsg)
    // console.log(received)
    if (received.type === 'get') {
      this.data.name1 =
        received.payload.state.reported.product_info[0].product_name
      this.setData({
        name1: this.data.name1
      })
      this.data.name2 =
        received.payload.state.reported.product_info[1].product_name
      this.setData({
        name2: this.data.name2
      })
      this.data.name3 =
        received.payload.state.reported.product_info[2].product_name
      this.setData({
        name3: this.data.name3
      })
      this.data.name4 =
        received.payload.state.reported.product_info[3].product_name
      this.setData({
        name4: this.data.name4
      })
      this.data.price1 =
        received.payload.state.reported.product_info[0].product_price
      this.setData({
        price1: this.data.price1
      })
      this.data.price2 =
        received.payload.state.reported.product_info[1].product_price
      this.setData({
        price2: this.data.price2
      })
      this.data.price3 =
        received.payload.state.reported.product_info[2].product_price
      this.setData({
        price3: this.data.price3
      })
      this.data.price4 =
        received.payload.state.reported.product_info[3].product_price
      this.setData({
        price4: this.data.price4
      })
    } else if (received.type === 'update') {
      console.log(received)
      this.shadowVersion = received.payload.version
      console.log('version updated to ')
      console.log(this.shadowVersion)
      if (received.result === 5005) {
        this.reportToShadow(this.lastReportedToShadowMsg)
      }
    }
  },

  processSyncMessage () {
    const received = JSON.parse(this.data.shadowMsg)
    console.log(received)
    this.data.name1 = received.state.reported.product_info[0].product_name
    this.setData({
      name1: this.data.name1
    })
    this.data.name2 = received.state.reported.product_info[1].product_name
    this.setData({
      name2: this.data.name2
    })
    this.data.name3 = received.state.reported.product_info[2].product_name
    this.setData({
      name3: this.data.name3
    })
    this.data.name4 = received.state.reported.product_info[3].product_name
    this.setData({
      name4: this.data.name4
    })
    this.data.price1 = received.state.reported.product_info[0].product_price
    this.setData({
      price1: this.data.price1
    })
    this.data.price2 = received.state.reported.product_info[1].product_price
    this.setData({
      price2: this.data.price2
    })
    this.data.price3 = received.state.reported.product_info[2].product_price
    this.setData({
      price3: this.data.price3
    })
    this.data.price4 = received.state.reported.product_info[3].product_price
    this.setData({
      price4: this.data.price4
    })
    this.reportToShadow(received.state.reported)
  },

  reportToShadow: function (msgObj) {
    const reportMsg = {
      type: 'update',
      state: {
        reported: {
          ...msgObj
        }
      },
      version: this.shadowVersion,
      clientToken: 'clientToken'
    }
    if (this.data.client) {
      this.data.client.publish(
        this.data.shadowReqTopic,
        JSON.stringify(reportMsg)
      )
      console.log(JSON.stringify(reportMsg))
      this.lastReportedToShadowMsg = msgObj
    }
  },

  deal () {
    const received = JSON.parse(this.data.receivedMsg)
    if (received.slot_num === 1) {
      return {
        chosen_slot: 'slot_1'
      }
    }
    if (received.slot_num === 2) {
      return {
        chosen_slot: 'slot_2'
      }
    }
    if (received.slot_num === 3) {
      return {
        chosen_slot: 'slot_3'
      }
    }
    if (received.slot_num === 4) {
      return {
        chosen_slot: 'slot_4'
      }
    }
  },

  publish () {
    if (this.data.client) {
      this.data.client.publish(this.data.pubTopic, JSON.stringify(this.deal()))
      console.log(JSON.stringify(this.deal()))
      // this.requestShadowData()
      this.data.receivedMsg = ''
    }
  },

  onShow: function () {
    if (this.data.paying) {
      this.publish()
      this.data.paying = 0
    }
  },
  onLoad: function () {
    this.connect()
    this.sendDataRequest()
    this.startDataUpdateTimer()
  }
})
