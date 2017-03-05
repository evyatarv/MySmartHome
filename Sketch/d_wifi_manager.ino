
bool wifi_try_connect(const char* ssid, const char* password, bool set_static_ip)
{
  int timeout_counter = 0; 

  WiFi.disconnect();
  delay(2000);
  
  Serial.print("Connecteing to: ");
  Serial.println(ssid);

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

    Serial.print(".");
    dev_api->do_indicate();
  }

  // print wifi connection data
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

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
  Serial.println("WIFI mode set to stasion.");
  
  if (wifi_try_connect(ssid, password, set_static_ip))
    return true; 
     
  Serial.print("Fail to connect: ");
  Serial.println(ssid);
  Serial.println("Try connecting previous ssid");

  if (wifi_try_connect(WIFI_CURRENT_SSID_NAME.c_str(), WIFI_CURRENT_SSID_PASSWORD.c_str(), set_static_ip))
    return true;

  Serial.print("Fail to connect: ");
  Serial.println(ssid);
  Serial.println("Try connecting default ssid");

  if (wifi_try_connect(WIFI_DEFAULT_SSID_NAME, WIFI_DEFAULT_SSID_PASSWORD, set_static_ip))
    return true;

  return false;
}

