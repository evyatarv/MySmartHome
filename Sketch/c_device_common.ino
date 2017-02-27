
void system_error()
{
  device_err();
}


bool connect_wifi_network(bool set_static_ip)
{
  return wifi_connect(WIFI_CURRENT_SSID_NAME.c_str(), WIFI_CURRENT_SSID_PASSWORD.c_str(), set_static_ip);
}

void prepare_serial()
{
  Serial.begin(115200); 
  delay(5000);
}
