/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// < this demo make the wifi to connect to the specified AP
#include <unistd.h>
#include <hi_wifi_api.h>
#include <lwip/ip_addr.h>
#include <lwip/netifapi.h>
#include <hi_types_base.h>
#include <hi_task.h>
#include <hi_mem.h>
#include "iot_config.h"
#include "iot_log.h"
#include "wifi_device.h"
#include "cmsis_os2.h"
#include "wifi_device_config.h"
#include "lwip/api_shell.h"

#define APP_INIT_VAP_NUM    2
#define APP_INIT_USR_NUM    2

static struct netif *gLwipNetif = NULL;
static hi_bool  gScanDone = HI_FALSE;
unsigned char wifiStatus = 0;

unsigned char wifiFirstConnecting = 0;
unsigned char wifiSecondConnecting = 0;
unsigned char wifiSecondConnected = 0;
static struct netif* g_iface = NULL;
void WifiStopSta(int netId);
static int WifiStartSta(void);
int cnetId = -1;
int g_connected = 0;

#define WIFI_CONNECT_STATUS ((unsigned char)0x02)

void wifiReconnected(int netId)
{
    int recnetId = netId;
    if (wifiFirstConnecting == WIFI_CONNECT_STATUS) {
        wifiSecondConnecting = HI_TRUE;
        wifiFirstConnecting = HI_FALSE;
        WifiStopSta(recnetId);
        ip4_addr_t ipAddr;
        ip4_addr_t ipAny;
        IP4_ADDR(&ipAny, 0, 0, 0, 0);
        IP4_ADDR(&ipAddr, 0, 0, 0, 0);
        recnetId = WifiStartSta();
        netifapi_dhcp_start(gLwipNetif);
        while (0 == memcmp(&ipAddr, &ipAny, sizeof(ip4_addr_t))) {
            IOT_LOG_DEBUG("<Wifi reconnecting>:Wait the DHCP READY");
            netifapi_netif_get_addr(gLwipNetif, &ipAddr, NULL, NULL);
            hi_sleep(1000); /* 1000: cpu sleep 1000 ms */
        }
        wifiSecondConnected = HI_FALSE;
        wifiFirstConnecting = WIFI_CONNECT_STATUS;
        wifiStatus = HI_WIFI_EVT_CONNECTED;
    }
}
/* clear netif's ip, gateway and netmask */
static void StaResetAddr(struct netif *lwipNetif)
{
    ip4_addr_t st_gw;
    ip4_addr_t st_ipaddr;
    ip4_addr_t st_netmask;

    if (lwipNetif == NULL) {
        IOT_LOG_ERROR("hisi_reset_addr::Null param of netdev");
        return;
    }

    IP4_ADDR(&st_gw, 0, 0, 0, 0);
    IP4_ADDR(&st_ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&st_netmask, 0, 0, 0, 0);

    netifapi_netif_set_addr(lwipNetif, &st_ipaddr, &st_netmask, &st_gw);
}

static void WpaEventCB(const hi_wifi_event *hisiEvent)
{
    if (hisiEvent == NULL)
        return;
    IOT_LOG_DEBUG("EVENT_TYPE:%d", hisiEvent->event);
    switch (hisiEvent->event) {
        case HI_WIFI_EVT_SCAN_DONE:
            IOT_LOG_DEBUG("WiFi: Scan results available");
            gScanDone = HI_TRUE;
            break;
        case HI_WIFI_EVT_CONNECTED:
            IOT_LOG_DEBUG("WiFi: Connected");
            netifapi_dhcp_start(gLwipNetif);
            wifiStatus = HI_WIFI_EVT_CONNECTED;
            if (wifiSecondConnected) {
                wifiSecondConnected = HI_FALSE;
                wifiFirstConnecting = WIFI_CONNECT_STATUS;
            }
            break;
        case HI_WIFI_EVT_DISCONNECTED:
            IOT_LOG_DEBUG("WiFi: Disconnected");
            netifapi_dhcp_stop(gLwipNetif);
            StaResetAddr(gLwipNetif);
            wifiStatus = HI_WIFI_EVT_DISCONNECTED;
            wifiReconnected(cnetId);
            break;
        case HI_WIFI_EVT_WPS_TIMEOUT:
            IOT_LOG_DEBUG("WiFi: wps is timeout");
            wifiStatus = HI_WIFI_EVT_WPS_TIMEOUT;
            break;
        default:
            break;
    }
}

static int StaStartConnect(void)
{
    int ret;
    errno_t rc;
    hi_wifi_assoc_request assoc_req = {0};

    /* copy SSID to assoc_req */
    rc = memcpy_s(assoc_req.ssid, HI_WIFI_MAX_SSID_LEN + 1, CONFIG_AP_SSID, strlen(CONFIG_AP_SSID));
    if (rc != EOK) {
        return -1;
    }

    /*
     * OPEN mode
     * for WPA2-PSK mode:
     * set assoc_req.auth as HI_WIFI_SECURITY_WPA2PSK,
     * then memcpy(assoc_req.key, "12345678", 8).
     */
    assoc_req.auth = HI_WIFI_SECURITY_WPA2PSK;
    rc = memcpy_s(assoc_req.key, HI_WIFI_MAX_KEY_LEN + 1, CONFIG_AP_PWD, strlen(CONFIG_AP_PWD));
    if (rc != EOK) {
        return -1;
    }

    ret = hi_wifi_sta_connect(&assoc_req);
    if (ret != HISI_OK) {
        return -1;
    }

    return 0;
}

static void PrintLinkedInfo(WifiLinkedInfo* info)
{
    int ret = 0;
    if (!info) {
        return;
    }

    static char macAddress[32] = {0};
    unsigned char* mac = info->bssid;
    if (snprintf_s(macAddress, sizeof(macAddress) + 1, sizeof(macAddress), "%02X:%02X:%02X:%02X:%02X:%02X",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]) < 0) { /* mac地址从0,1,2,3,4,5位 */
            return;
    }
}

static void OnWifiConnectionChanged(int state, WifiLinkedInfo* info)
{
    if (!info) {
        return;
    }

    printf("%s %d, state = %d, info = \r\n", __FUNCTION__, __LINE__, state);
    PrintLinkedInfo(info);

    if (state == WIFI_STATE_AVALIABLE) {
        g_connected = 1;
    } else {
        g_connected = 0;
    }
}

static void OnWifiScanStateChanged(int state, int size)
{
    printf("%s %d, state = %X, size = %d\r\n", __FUNCTION__, __LINE__, state, size);
}

static WifiEvent g_defaultWifiEventListener = {
    .OnWifiConnectionChanged = OnWifiConnectionChanged,
    .OnWifiScanStateChanged = OnWifiScanStateChanged
};

static int WifiStartSta(void)
{
    WifiDeviceConfig apConfig = {0};
    strcpy_s(apConfig.ssid, sizeof(CONFIG_AP_SSID), CONFIG_AP_SSID);
    strcpy_s(apConfig.preSharedKey, sizeof(CONFIG_AP_PWD), CONFIG_AP_PWD);
    apConfig.securityType = WIFI_SEC_TYPE_PSK;

    WifiErrorCode errCode;
    int netId = -1;

    errCode = RegisterWifiEvent(&g_defaultWifiEventListener);
    printf("RegisterWifiEvent: %d\r\n", errCode);

    errCode = EnableWifi();
    printf("EnableWifi: %d\r\n", errCode);

    errCode = AddDeviceConfig(&apConfig, &netId);
    printf("AddDeviceConfig: %d\r\n", errCode);

    g_connected = 0;
    errCode = ConnectTo(netId);
    printf("ConnectTo(%d): %d\r\n", netId, errCode);

    while (!g_connected) { // wait until connect to AP
        osDelay(10); /* 10: os sleep 10ms */
    }
    printf("g_connected: %d\r\n", g_connected);

    g_iface = netifapi_netif_find("wlan0");
    if (g_iface) {
        err_t ret = netifapi_dhcp_start(g_iface);
        printf("netifapi_dhcp_start: %d\r\n", ret);

        osDelay(100); // 100: os sleep 100ms wait DHCP server give me IP
        ret = netifapi_netif_common(g_iface, dhcp_clients_info_show, NULL);
        printf("netifapi_netif_common: %d\r\n", ret);
    }
    return netId;
}

void WifiStopSta(int netId)
{
    int stopNetId = netId;

    if (g_iface) {
        err_t ret = netifapi_dhcp_stop(g_iface);
        printf("netifapi_dhcp_stop: %d\r\n", ret);
    }

    WifiErrorCode errCode = Disconnect(); // disconnect with your AP
    printf("Disconnect: %d\r\n", errCode);

    errCode = UnRegisterWifiEvent(&g_defaultWifiEventListener);
    printf("UnRegisterWifiEvent: %d\r\n", errCode);

    RemoveDevice(stopNetId); // remove AP config
    printf("RemoveDevice: %d\r\n", errCode);

    errCode = DisableWifi();
    printf("DisableWifi: %d\r\n", errCode);
}

void WifiStaReadyWait(void)
{
    ip4_addr_t ipAddr;
    ip4_addr_t ipAny;
    IP4_ADDR(&ipAny, 0, 0, 0, 0);
    IP4_ADDR(&ipAddr, 0, 0, 0, 0);
    cnetId = WifiStartSta();
    IOT_LOG_DEBUG("wifi sta dhcp done");
}