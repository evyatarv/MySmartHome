
bool wifi_try_connect(const char* ssid, const char* password, bool set_static_ip)
{
  int timeout_counter = 0; 

  WiFi.disconnect();
  delay(FOUR_SEC);
  
  PRINT_I("Connecteing to: %s\n", ssid);

  // start wifi
  WiFi.begin(ssid, password); 

  // set static ip or DHCP
  if (set_static_ip)
    WiFi.config(DEV_IP, DEFAULT_GATEWAY,SUB_MASK_IP); 
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(WIFI_STATUS_PULL_INTERVAL);
    if (++timeout_counter == WIFI_CONNECTION_TIMEOUT)
    {
      if (dev_api && dev_api->led != NULL)
        dev_api->led(first_led, led_off);
      return false;
    }

    PRINT_I("%s", ".");
    dev_api->do_indicate();
  }

  // print wifi connection data
  PRINT_I("\nIP address: ");
  PRINT_V(WiFi.localIP());


  // save latest success connection
  WIFI_CURRENT_SSID_NAME = ssid;
  WIFI_CURRENT_SSID_PASSWORD = password;

  if (dev_api && dev_api->led != NULL)
    dev_api->led(first_led, led_on);
  
  return true;
}

bool wifi_connect(const char* ssid, const char* password, bool set_static_ip)
{
  // Set ESP to stasion mode
  WiFi.mode(WIFI_STA);
  PRINT_I("%s\n", "WIFI mode set to stasion.");
  
  if (wifi_try_connect(ssid, password, set_static_ip))
    return true; 
     
  PRINT_I("Fail to connect: %s \n Try connecting previous ssid\n", ssid);

  if (wifi_try_connect(WIFI_CURRENT_SSID_NAME.c_str(), WIFI_CURRENT_SSID_PASSWORD.c_str(), set_static_ip))
    return true;

  PRINT_I("Fail to connect: %s\n Try connecting default ssid\n", ssid);

  if (wifi_try_connect(WIFI_DEFAULT_SSID_NAME, WIFI_DEFAULT_SSID_PASSWORD, set_static_ip))
    return true;

  return false;
}

