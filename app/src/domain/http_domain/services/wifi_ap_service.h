#pragma once

#ifdef CONFIG_WIFI

#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/dhcpv4_server.h>

#endif // CONFIG_WIFI

namespace eerie_leap::domain::http_domain::services {

class WifiApService {
private:
#ifdef CONFIG_WIFI
    static net_if *ap_interface_;
    static net_mgmt_event_callback callback_;

    static bool DHCPv4ServerInitialize();
    static bool DHCPv4ServerStart();
    static bool AccessPointInitialize();
    static void EventHandler(net_mgmt_event_callback *callback, uint32_t mgmt_event, net_if *iface);
#endif // CONFIG_WIFI

public:
    static bool Initialize();
};

} // namespace eerie_leap::domain::http_domain::services
