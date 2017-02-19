
  
void init_eeprom()
{
  EEPROM.begin(EEPROM_SIZE);
}

bool flash_is_initialize(bool to_init)
{
  uint8_t first_boot_str[EEPROM_FIRST_BOOT_STR_LEN] = {0};

  eeprom_read_buffer(first_boot_str, EEPROM_FIRST_BOOT_STR_LEN, 0);

  if (strncmp((const char*)first_boot_str, EEPROM_FIRST_BOOT_STR, EEPROM_FIRST_BOOT_STR_LEN))
  {
    if(to_init) 
      flash_init();
    
    return false;
  }

  return true;
}

void flash_init()
{
  String tmp = (const char*)EEPROM_FIRST_BOOT_STR;
  Serial.println("init EEPROM ...\n");
  Serial.println(tmp);
    
  eeprom_write_buffer((const uint8_t*)tmp.c_str(), EEPROM_FIRST_BOOT_STR_LEN, 0);
}

void load_wifi_ssid_and_password()
{
  uint8_t data_len = 0; 
  uint8_t wifi_ssid[WIFI_MAX_SSID_LENGTH] = {0};
  uint8_t wifi_password[WIFI_MAX_PASSWORD_LENGTH] = {0};
  
  eeprom_read_buffer(&data_len, 1, EEPROM_WIFI_SSID_OFFSET);
  if (data_len != 0 && data_len <= WIFI_MAX_SSID_LENGTH)
  {
    eeprom_read_buffer(wifi_ssid, data_len, EEPROM_WIFI_SSID_OFFSET + EEPROM_DATA_LENGTH_OFFSET);
    WIFI_CURRENT_SSID_NAME = (const char*)wifi_ssid;

    eeprom_read_buffer(&data_len, 1, EEPROM_WIFI_PASSWORD_OFFSET);
    if (data_len != 0 && data_len <= WIFI_MAX_PASSWORD_LENGTH)
    {
      eeprom_read_buffer(wifi_password, data_len, EEPROM_WIFI_PASSWORD_OFFSET + EEPROM_DATA_LENGTH_OFFSET);
      WIFI_CURRENT_SSID_PASSWORD = (const char*)wifi_password;
    }
  }
}

bool load_wifi_device_ip()
{
  uint8_t data_len = 0; 
  uint8_t wifi_ip_buff[WIFI_IPV4_RAW_DATA_LENGTH] = {0};

  do
  {
    // set device IP
    eeprom_read_buffer(&data_len, EEPROM_DATA_LENGTH_OFFSET, EEPROM_WIFI_DEVICE_IP_OFFSET);
    if (data_len != WIFI_IPV4_RAW_DATA_LENGTH)
      return false;

    eeprom_read_buffer(wifi_ip_buff, data_len, EEPROM_WIFI_DEVICE_IP_OFFSET + EEPROM_DATA_LENGTH_OFFSET);
    DEV_IP = wifi_ip_buff;
    

    // set device SUB_MASK_IP
    eeprom_read_buffer(&data_len, 1, EEPROM_WIFI_SUBMASK_OFFSET);
    if (data_len != WIFI_IPV4_RAW_DATA_LENGTH)
      return false;

    eeprom_read_buffer(wifi_ip_buff, data_len, EEPROM_WIFI_DEVICE_IP_OFFSET + EEPROM_DATA_LENGTH_OFFSET);
    DEV_IP = wifi_ip_buff;


    // set default gateway
    eeprom_read_buffer(&data_len, 1, EEPROM_WIFI_DEFUALT_GATEWAY_OFFSET);
    if (data_len != WIFI_IPV4_RAW_DATA_LENGTH)
      return false;

    eeprom_read_buffer(wifi_ip_buff, data_len, EEPROM_WIFI_DEFUALT_GATEWAY_OFFSET + EEPROM_DATA_LENGTH_OFFSET);
    DEFAULT_GATEWAY = wifi_ip_buff;

  } while(false);

  return true;
}

void load_http_auth()
{
  uint8_t data_len = 0; 
  uint8_t user_auth[HTTP_MAX_USER_AUTH_LENGTH] = {0};
  uint8_t password_auth[HTTP_MAX_PASSWORD_AUTH_LENGTH] = {0};
  
  eeprom_read_buffer(&data_len, 1, EEPROM_HTTP_USER_AUTH_OFFSET);
  if (data_len != 0 && data_len <= HTTP_MAX_USER_AUTH_LENGTH)
  {
    eeprom_read_buffer(user_auth, data_len, EEPROM_HTTP_USER_AUTH_OFFSET + EEPROM_DATA_LENGTH_OFFSET);
    WIFI_CURRENT_SSID_NAME = (const char*)user_auth;

    eeprom_read_buffer(&data_len, 1, EEPROM_HTTP_PASSWORD_AUTH_OFFSET);
    if (data_len != 0 && data_len <= HTTP_MAX_PASSWORD_AUTH_LENGTH)
    {
      eeprom_read_buffer(password_auth, data_len, EEPROM_HTTP_PASSWORD_AUTH_OFFSET + EEPROM_DATA_LENGTH_OFFSET);
      WIFI_CURRENT_SSID_PASSWORD = (const char*)password_auth;
    }
  }
}

void load_config(bool* set_static_ip)
{
  // check if eeprom initialized
  if (!flash_is_initialize(true))
    return;

  // load wifi ssid and password
  load_wifi_ssid_and_password();

  // load wifi device static ip
  *set_static_ip = load_wifi_device_ip();

  // load http auth
  load_http_auth();
}
