/*
 * Copyright (c) 2022 Yicong Liu and Sidi Liang.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import mqtt from '../../utils/mqtt.min.js'

Page({
  data: {
    client: null,
    conenctBtnText: '连接',
    host: 'YT32IOSCAL.ap-guangzhou.iothub.tencentdevices.com:443',
    pubTopic: '$shadow/operation/YT32IOSCAL/miniprogram',
    receivedMsg: '',
    received: '',
    version: 38,
    name1: '创口贴',
    price1: 9.9,
    remain_num1: 3,
    name2: '口罩',
    price2: 1,
    remain_num2: 3,
    name3: '棉签',
    price3: 6.9,
    remain_num3: 3,
    name4: '口罩',
    price4: 1,
    remain_num4: 3,
    mqttOptions: {
      username: 'YT32IOSCALminiprogram;12010126;72bb5;1666022400',
      password: '03f6c586126cbc084d0f5b37fd0005f6a062c942c25fc9123e5a4a171044ceac;hmacsha256',
      reconnectPeriod: 1000,
      connectTimeout: 30 * 1000
    }
  },

  requestData: function () {
    var requestMsg = {
      'type': 'get',
      'clientToken': 'clientToken'
    }
    this.data.client.publish(this.data.pubTopic, JSON.stringify(requestMsg))
  },
  _nameEdit1: function (event) {
    this.setData({
      name1: event.detail.value
    })
    this.publish()
  },
  _priceEdit1: function (event) {
    this.setData({
      price1: event.detail.value
    })
    this.publish()
  },
  _remainingNumEdit1: function (event) {
    this.setData({
      remain_num1: event.detail.value
    })
    this.publish()
  },

  _nameEdit2: function (event) {
    this.setData({
      name2: event.detail.value
    })
    this.publish()
  },
  _priceEdit2: function (event) {
    this.setData({
      price2: event.detail.value
    })
    this.publish()
  },
  _remainingNumEdit2: function (event) {
    this.setData({
      remain_num2: event.detail.value
    })
    this.publish()
  },

  _nameEdit3: function (event) {
    this.setData({
      name3: event.detail.value
    })
    this.publish()
  },
  _priceEdit3: function (event) {
    this.setData({
      price3: event.detail.value
    })
    this.publish()
  },
  _remainingNumEdit3: function (event) {
    this.setData({
      remain_num3: event.detail.value
    })
    this.publish()
  },

  _nameEdit4: function (event) {
    this.setData({
      name4: event.detail.value
    })
    this.publish()
  },
  _priceEdit4: function (event) {
    this.setData({
      price4: event.detail.value
    })
    this.publish()
  },
  _remainingNumEdit4: function (event) {
    this.setData({
      remain_num4: event.detail.value
    })
    this.publish()
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

  connect () {
    try {
      wx.setNavigationBarTitle({
        title: '产品管理中心（连接中...）'
      })
      const clientId = new Date().getTime()
      this.data.client = mqtt.connect(`wxs://${this.data.host}/mqtt`, {
        ...this.data.mqttOptions,
        clientId
      })

      this.data.client.on('connect', () => {
        wx.setNavigationBarTitle({
          title: '产品管理中心'
        })

        this.data.client.on('message', (topic, payload) => {
          const currMsg = this.data.receivedMsg ? `<br/>${payload}` : payload

          this.setValue('receivedMsg', this.data.receivedMsg.concat(currMsg))
          this.data.version = JSON.parse(this.data.receivedMsg).payload.version
          console.log(this.data.version)
          this.data.receivedMsg = ''
        })

        this.data.client.on('error', (error) => {
          wx.setNavigationBarTitle({
            title: '产品管理中心（连接错误）'
          })
          console.log('onError', error)
        })

        this.data.client.on('reconnect', () => {
          wx.setNavigationBarTitle({
            title: '产品管理中心（重连中...）'
          })
          console.log('reconnecting...')
        })

        this.data.client.on('offline', () => {
          wx.setNavigationBarTitle({
            title: '产品管理中心（未连接）'
          })
          console.log('onOffline')
        })
      })
    } catch (error) {
      wx.setNavigationBarTitle({
        title: '产品管理中心（连接错误）'
      })
      console.log('mqtt.connect error', error)
    }
  },

  subscribe () {
    if (this.data.client) {
      return
    }
  },

  unsubscribe () {
    if (this.data.client) {
      this.data.client.unsubscribe(this.data.subTopic)
      return
    }
  },

  deal () // 需要根据通信协议更改的地方
    {
    return {
      type: 'update',
      state: {
        reported: {
          product_info: [{
            slot_num: '1',
            product_price: this.data.price1,
            remaining_num: this.data.remain_num1,
            product_name: this.data.name1
          }, {
            slot_num: '2',
            product_price: this.data.price2,
            remaining_num: this.data.remain_num2,
            product_name: this.data.name2
          }, {
            slot_num: 3,
            product_price: this.data.price3,
            remaining_num: this.data.remain_num3,
            product_name: this.data.name3
          }, {
            slot_num: 4,
            product_price: this.data.price4,
            remaining_num: this.data.remain_num4,
            product_name: this.data.name4
          }]
        }
      },
      version: this.data.version,
      clientToken: ''
    }
  },

  publish () {
    if (this.data.client) {
      this.data.client.publish(this.data.pubTopic, JSON.stringify(this.deal()))
      this.data.version++
      console.log(JSON.stringify(this.deal()))
      return
    }
  },

  onLoad: function () {
    this.connect()
    this.subscribe()
  }
})
