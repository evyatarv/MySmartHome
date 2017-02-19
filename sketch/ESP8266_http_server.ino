
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IPAddress.h>
#include <String.h>
#include <EEPROM.h>

MDNSResponder mdns;

// device
#define DEVICE_STATUS_ERR_DELAY_TIME  (100)
#define DEVICE_ERR_MESSAGE_PULL_TIME  (1000)
#define DEVICE_RESTART_TIMEOUT        (40)

unsigned int restart_counter = 0;

// wifi
#define WIFI_STATUS_PULL_INTERVAL (500)
#define WIFI_CONNECTION_TIMEOUT   (120)
#define WIFI_MAX_SSID_LENGTH      (32)
#define WIFI_MAX_PASSWORD_LENGTH  (32)
#define WIFI_MAX_IPV4_STR_LENGTH  (15)
#define WIFI_MIN_IPV4_STR_LENGTH  (7)
#define WIFI_IPV4_RAW_DATA_LENGTH (4)

enum WIFI_CONF_NUM{ SSID_NAME=0, PASSWORD, GATEWAY, MASK_IP, DEVICE_IP, MAX_CONF_LEN };
const char* WIFI_CONF[] = { "ssid", "password", "gateway", "mask_ip", "device_ip" };
const char* WIFI_DEFAULT_SSID_NAME = "******";
const char* WIFI_DEFAULT_SSID_PASSWORD = "******";
String WIFI_CURRENT_SSID_NAME = WIFI_DEFAULT_SSID_NAME;
String WIFI_CURRENT_SSID_PASSWORD = WIFI_DEFAULT_SSID_PASSWORD;
IPAddress DEFAULT_GATEWAY;
IPAddress SUB_MASK_IP;
IPAddress DEV_IP;

// http
#define HTTP_MAX_USER_AUTH_LENGTH     (32)
#define HTTP_MAX_PASSWORD_AUTH_LENGTH (32)
#define HTTP_RESPONSE_DELAY_TIME      (20)

const char* HTTP_AUTH_USER = "******";
const char* HTTP_AUTH_PASSWORD = "******";
String HTTP_RETURN_WEB_PAGE = "";
ESP8266WebServer http_server(80);

// eeprom
#define EEPROM_SIZE                         (256)
#define EEPROM_DATA_LENGTH_OFFSET           (1)
#define EEPROM_FIRST_BOOT_STR               ("OK")
#define EEPROM_FIRST_BOOT_STR_LEN           (2)                 
#define EEPROM_WIFI_SSID_OFFSET             (2)
#define EEPROM_WIFI_PASSWORD_OFFSET         (EEPROM_WIFI_SSID_OFFSET + EEPROM_DATA_LENGTH_OFFSET + WIFI_MAX_SSID_LENGTH)
#define EEPROM_WIFI_DEVICE_IP_OFFSET        (EEPROM_WIFI_PASSWORD_OFFSET + EEPROM_DATA_LENGTH_OFFSET + WIFI_MAX_PASSWORD_LENGTH)
#define EEPROM_WIFI_SUBMASK_OFFSET          (EEPROM_WIFI_DEVICE_IP_OFFSET + EEPROM_DATA_LENGTH_OFFSET + WIFI_IPV4_RAW_DATA_LENGTH)
#define EEPROM_WIFI_DEFUALT_GATEWAY_OFFSET  (EEPROM_WIFI_SUBMASK_OFFSET + EEPROM_DATA_LENGTH_OFFSET + WIFI_IPV4_RAW_DATA_LENGTH)
#define EEPROM_HTTP_USER_AUTH_OFFSET        (EEPROM_WIFI_DEFUALT_GATEWAY_OFFSET + EEPROM_DATA_LENGTH_OFFSET + WIFI_IPV4_RAW_DATA_LENGTH)
#define EEPROM_HTTP_PASSWORD_AUTH_OFFSET    (EEPROM_HTTP_USER_AUTH_OFFSET + EEPROM_DATA_LENGTH_OFFSET + HTTP_MAX_USER_AUTH_LENGTH)

// gpios
#define SONOFF_BUTTON     (0)
#define SONOFF_RELAY      (12)
#define SONOFF_LED        (13)
#define SONOFF_INPUT      (14)


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
  Serial.println("in load config");
  // check if eeprom initialized
  if (!epprom_is_initialize(true))
    return;

  // load wifi ssid and password
  load_wifi_ssid_and_password();

  // load wifi device static ip
  *set_static_ip = load_wifi_device_ip();

  // load http auth
  load_http_auth();
}

void setup(void)
{
  bool set_static_ip = false; 

  EEPROM.begin(EEPROM_SIZE);
  
  prepare_gpios();

  prepare_serial();

  load_config(&set_static_ip);

  if (!wifi_connect(WIFI_CURRENT_SSID_NAME.c_str(), WIFI_CURRENT_SSID_PASSWORD.c_str(), set_static_ip))
    device_err();

  init_http_server();
  
  led_on();
}
 
void loop(void){
  http_server.handleClient();
}

