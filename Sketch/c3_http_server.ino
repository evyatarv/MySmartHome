
void set_network_configuration()
{
  String ssid;
  String password;
  String conf;

  conf = http_server.arg(WIFI_CONF[SSID_NAME]);
  if (conf.length() && conf.length() <= WIFI_MAX_SSID_LENGTH)
  {
    ssid = conf;
    Serial.print("SSID set to: ");
    Serial.println(ssid);
  }
  else
    return;

  conf = http_server.arg(WIFI_CONF[PASSWORD]);
  if (conf.length() && conf.length() <= WIFI_MAX_PASSWORD_LENGTH)
  {
    password = conf;
    Serial.print("PASSWORD set to: ");
    Serial.println(password);
  }

  if (!wifi_connect(ssid.c_str(), password.c_str(), false))
    dev_api->enter_err_mode();

  // save to flash
  int len = WIFI_CURRENT_SSID_NAME.length();
  eeprom_write_buffer((const uint8_t*)&len, EEPROM_WIFI_SSID_OFFSET, EEPROM_DATA_LENGTH_OFFSET);
  eeprom_write_buffer((const uint8_t*)WIFI_CURRENT_SSID_NAME.c_str(), len, EEPROM_WIFI_SSID_OFFSET + EEPROM_DATA_LENGTH_OFFSET);

  len = WIFI_CURRENT_SSID_PASSWORD.length();
  eeprom_write_buffer((const uint8_t*)&len, EEPROM_DATA_LENGTH_OFFSET, EEPROM_WIFI_PASSWORD_OFFSET);
  eeprom_write_buffer((const uint8_t*)WIFI_CURRENT_SSID_PASSWORD.c_str(), len, EEPROM_WIFI_PASSWORD_OFFSET + EEPROM_DATA_LENGTH_OFFSET);
}

bool set_device_ip()
{
  String conf;
  bool status = false;

  do
  {
    conf = http_server.arg(WIFI_CONF[DEVICE_IP]);
    if (conf.length() > WIFI_MIN_IPV4_STR_LENGTH &&
          conf.length() < WIFI_MAX_IPV4_STR_LENGTH)
    {
      DEV_IP.fromString(conf);
      Serial.print("DEVICE IP set to: ");
      Serial.println(DEV_IP);
    }
    else
      break;
  
    conf = http_server.arg(WIFI_CONF[MASK_IP]);
    if (conf.length() > WIFI_MIN_IPV4_STR_LENGTH &&
          conf.length() < WIFI_MAX_IPV4_STR_LENGTH)
    {
      SUB_MASK_IP.fromString(conf);
      Serial.print("MUSK IP set to: ");
      Serial.println(SUB_MASK_IP);
    }
    else
      break;
  
    conf = http_server.arg(WIFI_CONF[GATEWAY]);
    if (conf.length() > WIFI_MIN_IPV4_STR_LENGTH &&
          conf.length() < WIFI_MAX_IPV4_STR_LENGTH)
    {
      DEFAULT_GATEWAY.fromString(conf);
      Serial.print("GATEWAY IP set to: ");
      Serial.println(DEFAULT_GATEWAY);
    }
    else
      break;
      
    status = wifi_try_connect(WIFI_CURRENT_SSID_NAME.c_str(), WIFI_CURRENT_SSID_PASSWORD.c_str(), true);
    if (status == true)
    {
      uint32_t tmp_ip = 0; 
      uint8_t len = WIFI_IPV4_RAW_DATA_LENGTH;
      
      // save to flash
      eeprom_write_buffer(&len, EEPROM_DATA_LENGTH_OFFSET, EEPROM_WIFI_DEVICE_IP_OFFSET);      
      tmp_ip = (uint32_t)DEV_IP;
      eeprom_write_buffer((const uint8_t*)&tmp_ip, WIFI_IPV4_RAW_DATA_LENGTH, EEPROM_WIFI_DEVICE_IP_OFFSET + EEPROM_DATA_LENGTH_OFFSET);

      eeprom_write_buffer(&len, EEPROM_DATA_LENGTH_OFFSET, EEPROM_WIFI_SUBMASK_OFFSET);
      tmp_ip = (uint32_t)SUB_MASK_IP;
      eeprom_write_buffer((const uint8_t*)&tmp_ip, WIFI_IPV4_RAW_DATA_LENGTH, EEPROM_WIFI_SUBMASK_OFFSET + EEPROM_DATA_LENGTH_OFFSET);

      eeprom_write_buffer(&len, EEPROM_DATA_LENGTH_OFFSET, EEPROM_WIFI_DEFUALT_GATEWAY_OFFSET);
      tmp_ip = (uint32_t)DEFAULT_GATEWAY;
      eeprom_write_buffer((const uint8_t*)&tmp_ip, WIFI_IPV4_RAW_DATA_LENGTH, EEPROM_WIFI_DEFUALT_GATEWAY_OFFSET + EEPROM_DATA_LENGTH_OFFSET);
    }
  }
  while(false);
    
  return status;
}

void init_http_server()
{
  http_server.on("/", HTTP_POST, [](){
    if(!http_server.authenticate(HTTP_AUTH_USER, HTTP_AUTH_PASSWORD))
      return http_server.requestAuthentication();
    http_server.send(HTTP_RESPONSE_OK, "text/html", HTTP_RETURN_WEB_PAGE);
  });

  http_server.on("/relay/1/on", HTTP_POST, [](){
    if(!http_server.authenticate(HTTP_AUTH_USER, HTTP_AUTH_PASSWORD))
      return http_server.requestAuthentication();

    if (dev_api && dev_api->relay)
    {
      dev_api->relay(first_relay, relay_on);
      delay(HTTP_RESPONSE_DELAY_TIME);
      http_server.send(HTTP_RESPONSE_OK, "text/html", HTTP_RETURN_WEB_PAGE);
    }
    else
      //TODO: explore - not sure this is the right way to return error
      http_server.send(HTTP_RESPONSE_PAGE_NOT_FOUND, "text/html", HTTP_RETURN_WEB_PAGE);
  });
  
  http_server.on("/relay/1/off", HTTP_POST, [](){
    if(!http_server.authenticate(HTTP_AUTH_USER, HTTP_AUTH_PASSWORD))
      return http_server.requestAuthentication();

  if (dev_api && dev_api->relay)
    {
      dev_api->relay(first_relay, relay_off);
      delay(HTTP_RESPONSE_DELAY_TIME);
      http_server.send(HTTP_RESPONSE_OK, "text/html", HTTP_RETURN_WEB_PAGE);
    }
    else
      //TODO: explore - not sure this is the right way to return error
      http_server.send(HTTP_RESPONSE_PAGE_NOT_FOUND, "text/html", HTTP_RETURN_WEB_PAGE);
      
  });



  http_server.on("/relay/2/on", HTTP_POST, [](){
    if(!http_server.authenticate(HTTP_AUTH_USER, HTTP_AUTH_PASSWORD))
      return http_server.requestAuthentication();

    if (dev_api && dev_api->relay)
    {
      dev_api->relay(second_relay, relay_on);
      delay(HTTP_RESPONSE_DELAY_TIME);
      http_server.send(HTTP_RESPONSE_OK, "text/html", HTTP_RETURN_WEB_PAGE);
    }
    else
      //TODO: explore - not sure this is the right way to return error
      http_server.send(HTTP_RESPONSE_PAGE_NOT_FOUND, "text/html", HTTP_RETURN_WEB_PAGE);
  });
  
  http_server.on("/relay/2/off", HTTP_POST, [](){
    if(!http_server.authenticate(HTTP_AUTH_USER, HTTP_AUTH_PASSWORD))
      return http_server.requestAuthentication();

  if (dev_api && dev_api->relay)
    {
      dev_api->relay(second_relay, relay_off);
      delay(HTTP_RESPONSE_DELAY_TIME);
      http_server.send(HTTP_RESPONSE_OK, "text/html", HTTP_RETURN_WEB_PAGE);
    }
    else
      //TODO: explore - not sure this is the right way to return error
      http_server.send(HTTP_RESPONSE_PAGE_NOT_FOUND, "text/html", HTTP_RETURN_WEB_PAGE);
      
  });


  
  http_server.on("/restart", HTTP_POST, [](){
    if(!http_server.authenticate(HTTP_AUTH_USER, HTTP_AUTH_PASSWORD))
      return http_server.requestAuthentication();
      
    if (!dev_api && !dev_api->do_device_could_reset)
    {
      //TODO: explore - not sure this is the right way to return error
      http_server.send(HTTP_RESPONSE_PAGE_NOT_FOUND, "text/html", HTTP_RETURN_WEB_PAGE);
      return;
    }
      
    http_server.send(HTTP_RESPONSE_OK, "text/html", HTTP_RETURN_WEB_PAGE);
    
    delay(HTTP_RESPONSE_DELAY_TIME);
    dev_api->do_device_could_reset();
        
  });
  


  http_server.on("/set/network", HTTP_POST, [](){
    if(!http_server.authenticate(HTTP_AUTH_USER, HTTP_AUTH_PASSWORD))
      return http_server.requestAuthentication();
      
    http_server.send(HTTP_RESPONSE_OK, "text/html", HTTP_RETURN_WEB_PAGE);
    
    delay(HTTP_RESPONSE_DELAY_TIME);
    set_network_configuration();
 
  });

  http_server.on("/set/address", HTTP_POST, [](){
    if(!http_server.authenticate(HTTP_AUTH_USER, HTTP_AUTH_PASSWORD))
      return http_server.requestAuthentication();
 
    http_server.send(HTTP_RESPONSE_OK, "text/html", HTTP_RETURN_WEB_PAGE);
    
    delay(HTTP_RESPONSE_DELAY_TIME);
    set_device_ip();
    
  });

  http_server.begin();
  Serial.println("HTTP server started");
}


