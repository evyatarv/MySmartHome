
void system_error()
{
  if (dev_api && dev_api->do_device_could_reset)
    dev_api->do_device_could_reset();
}


bool connect_wifi_network(bool set_static_ip)
{
  return wifi_connect(WIFI_CURRENT_SSID_NAME.c_str(), WIFI_CURRENT_SSID_PASSWORD.c_str(), set_static_ip);
}

void set_device_api(struct _device_api *api)
{
  dev_api = api; 
}

