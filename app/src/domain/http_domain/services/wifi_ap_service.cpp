#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "wifi_ap_service.h"

namespace eerie_leap::domain::http_domain::services {

LOG_MODULE_REGISTER(wifi_logger);

#ifdef CONFIG_WIFI

net_if *WifiApService::ap_interface_ = net_if_get_default();
net_mgmt_event_callback WifiApService::callback_ = {};

#define MACSTR "%02X:%02X:%02X:%02X:%02X:%02X"

#define NET_EVENT_WIFI_MASK                                                         \
       (NET_EVENT_WIFI_AP_ENABLE_RESULT | NET_EVENT_WIFI_AP_DISABLE_RESULT |        \
        NET_EVENT_WIFI_AP_STA_CONNECTED | NET_EVENT_WIFI_AP_STA_DISCONNECTED)

/* AP Mode Configuration */
#define WIFI_AP_SSID       "EerieLeap"
#define WIFI_AP_PSK        ""
#define WIFI_AP_IP_ADDRESS "192.168.4.1"
#define WIFI_AP_NETMASK    "255.255.255.0"

void WifiApService::EventHandler(net_mgmt_event_callback *callback, uint32_t mgmt_event, net_if *iface) {
    char buf[NET_IPV4_ADDR_LEN];

    switch (mgmt_event) {
    case NET_EVENT_WIFI_AP_ENABLE_RESULT: {
        LOG_INF("AP Mode is enabled. Waiting for station to connect");
        break;
    }
    case NET_EVENT_WIFI_AP_DISABLE_RESULT: {
        LOG_INF("AP Mode is disabled.");
        break;
    }
    case NET_EVENT_WIFI_AP_STA_CONNECTED: {
        auto* sta_info = (wifi_ap_sta_info *)callback->info;

        LOG_INF("station: " MACSTR " joined ", sta_info->mac[0], sta_info->mac[1],
            sta_info->mac[2], sta_info->mac[3], sta_info->mac[4], sta_info->mac[5]);

        LOG_INF("    Address[%d]: %s", net_if_get_by_iface(iface),
            net_addr_ntop(AF_INET,
            &iface->config.ip.ipv4->unicast[0].ipv4.address.in_addr,
            buf, sizeof(buf)));

        LOG_INF("    Subnet[%d]: %s", net_if_get_by_iface(iface),
            net_addr_ntop(AF_INET,
            &iface->config.ip.ipv4->unicast[0].netmask,
            buf, sizeof(buf)));

        LOG_INF("    Router[%d]: %s", net_if_get_by_iface(iface),
            net_addr_ntop(AF_INET,
            &iface->config.ip.ipv4->gw,
            buf, sizeof(buf)));
        break;
    }
    case NET_EVENT_WIFI_AP_STA_DISCONNECTED: {
        auto* sta_info = (wifi_ap_sta_info *)callback->info;

        LOG_INF("station: " MACSTR " leave ",
            sta_info->mac[0],
            sta_info->mac[1],
            sta_info->mac[2],
            sta_info->mac[3],
            sta_info->mac[4],
            sta_info->mac[5]);
        break;
    }
    default:
        break;
    }
}

bool WifiApService::DHCPv4ServerInitialize() {
    static in_addr addr;
    static in_addr netmaskAddr;

    if (net_addr_pton(AF_INET, WIFI_AP_IP_ADDRESS, &addr)) {
        LOG_ERR("Invalid address: %s", WIFI_AP_IP_ADDRESS);
        return false;
    }

    if (net_addr_pton(AF_INET, WIFI_AP_NETMASK, &netmaskAddr)) {
        LOG_ERR("Invalid netmask: %s", WIFI_AP_NETMASK);
        return false;
    }

    net_if_ipv4_set_gw(ap_interface_, &addr);

    if (net_if_ipv4_addr_add(ap_interface_, &addr, NET_ADDR_MANUAL, 0) == NULL) {
        LOG_ERR("unable to set IP address for AP interface");
    }

    if (!net_if_ipv4_set_netmask_by_addr(ap_interface_, &addr, &netmaskAddr)) {
        LOG_ERR("Unable to set netmask for AP interface: %s", WIFI_AP_NETMASK);
    }

    // Starting IPv4 address for DHCPv4 address pool.
    const uint8_t dhcp_start_ip_offset = 10;
    addr.s4_addr[3] += dhcp_start_ip_offset;

    if (net_dhcpv4_server_start(ap_interface_, &addr) != 0) {
        LOG_ERR("DHCP server is not started for desired IP");
        return false;
    }

    LOG_INF("DHCPv4 server started...\n");
    return true;
}

bool WifiApService::AccessPointInitialize() {
    if (!ap_interface_) {
        LOG_ERR("AP: is not initialized");
        return false;
    }

    LOG_INF("Turning on AP Mode");

    wifi_connect_req_params ap_config = {
        .ssid = (const uint8_t *)WIFI_AP_SSID,
        .ssid_length = static_cast<uint8_t>(strlen(WIFI_AP_SSID)),
        .psk = (const uint8_t *)WIFI_AP_PSK,
        .psk_length = static_cast<uint8_t>(strlen(WIFI_AP_PSK)),
        .band = WIFI_FREQ_BAND_2_4_GHZ,
        .channel = WIFI_CHANNEL_ANY
    };

    if (strlen(WIFI_AP_PSK) == 0)
        ap_config.security = WIFI_SECURITY_TYPE_NONE;
    else
        ap_config.security = WIFI_SECURITY_TYPE_PSK;

    if(!DHCPv4ServerInitialize()) {
        LOG_ERR("Failed to initialize DHCPv4 server");
        return false;
    }

    int ret = net_mgmt(NET_REQUEST_WIFI_AP_ENABLE, ap_interface_, &ap_config, sizeof(wifi_connect_req_params));
    if(ret) {
        LOG_ERR("NET_REQUEST_WIFI_AP_ENABLE failed, err: %d", ret);
        return false;
    }

    return true;
}

#endif // CONFIG_WIFI

bool WifiApService::Initialize() {
#ifdef CONFIG_WIFI
    net_mgmt_init_event_callback(&callback_, EventHandler, NET_EVENT_WIFI_MASK);
    net_mgmt_add_event_callback(&callback_);

    ap_interface_ = net_if_get_wifi_sap();

    if(!AccessPointInitialize()) {
        LOG_ERR("Failed to initialize access point");
        return false;
    }

    LOG_INF("Waiting for Wi-Fi interface to be up");
    const int retry_count = 10;
    const int retry_delay = 500;
    for (int i = 0; i < retry_count; ++i) {
        if (net_if_is_up(ap_interface_)) {
            LOG_INF("Wi-Fi interface is up");
            break;
        }

        k_sleep(K_MSEC(retry_delay));
    }

    return true;
#else
    LOG_WRN("WiFi is not supported");
    return true;
#endif // CONFIG_WIFI
}

} // namespace eerie_leap::domain::http_domain::services
