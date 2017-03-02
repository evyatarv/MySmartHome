
void system_error()
{
  device_err();
}


bool connect_wifi_network(bool set_static_ip)
{
  return wifi_connect(WIFI_CURRENT_SSID_NAME.c_str(), WIFI_CURRENT_SSID_PASSWORD.c_str(), set_static_ip);
}

void set_device_api(struct _device_api *api)
{
  Serial.println("in set_device_api");
  dev_api = api; 
}

