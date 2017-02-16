
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

void hexDump (void *addr, int len, int r_size) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    if (len == 0) {
        Serial.printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        Serial.printf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }
    Serial.println();
    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of r_size means new line (with line offset).
        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
            {
                Serial.printf ("  %s", buff);
                Serial.println();
            }
            // Output the offset.
            Serial.printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        Serial.printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        Serial.printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    Serial.printf ("  %s\n", buff);
    Serial.println();
}

void eeprom_read_buffer(uint8_t* buff, int len, int addr_offset)
{
  if (!buff || len < 0 || addr_offset < 0)
    return;
  
  for(int i=0; i < len ; i++)
    buff[i] = EEPROM.read(addr_offset+i);
}

void eeprom_write_buffer(const uint8_t* buff, int len, int addr_offset)
{
  if (!buff || len < 0 || addr_offset < 0)
    return;
    
  for(int i=0; i < len ; i++)
    EEPROM.write(addr_offset+i, buff[i]);

  EEPROM.commit();
}

bool epprom_is_initialize()
{
  uint8_t first_boot_str[EEPROM_FIRST_BOOT_STR_LEN] = {0};

  eeprom_read_buffer(first_boot_str, EEPROM_FIRST_BOOT_STR_LEN, 0);

  if (strncmp((const char*)first_boot_str, EEPROM_FIRST_BOOT_STR, EEPROM_FIRST_BOOT_STR_LEN))
  {
    String tmp = (const char*)EEPROM_FIRST_BOOT_STR;
    Serial.println("init EEPROM ...\n");
    Serial.println(tmp);
    
    eeprom_write_buffer((const uint8_t*)tmp.c_str(), EEPROM_FIRST_BOOT_STR_LEN, 0);
    
    return false;
  }

  return true;
}

void led_tick()
{
  //toggle state
  int state = digitalRead(SONOFF_LED);
  digitalWrite(SONOFF_LED, !state);
}

void device_restart()
{
  Serial.println("PREFORMING DEVICE RESET ...");
  ESP.restart();
  delay(1000);
}

void device_err()
{
  unsigned int counter = 0;
  
  int buttonState = LOW;
  
  while (true)
  {
    // Print err msg to serial every ~1 min
    if (!(++counter % DEVICE_ERR_MESSAGE_PULL_TIME))
      Serial.println("device err.. DO RESET");
        
    delay(DEVICE_STATUS_ERR_DELAY_TIME);
    led_tick();

    buttonState = digitalRead(SONOFF_BUTTON);
    if (buttonState == LOW) 
    {
      if (!(++restart_counter % DEVICE_RESTART_TIMEOUT))
      {
        device_restart();
      }
    }
  }
}

void btn_toggle_state()
{
  restart_counter = 0;
}

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
      return false;

    Serial.print(".");
    led_tick();
  }

  // print wifi connection data
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // save latest success connection
  WIFI_CURRENT_SSID_NAME = ssid;
  WIFI_CURRENT_SSID_PASSWORD = password;
  
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

void http_on()
{
  Serial.println("relay ON");
  digitalWrite(SONOFF_RELAY, HIGH);
  digitalWrite(SONOFF_LED, LOW);
  delay(HTTP_RESPONSE_DELAY_TIME);
}

void http_off()
{
  Serial.println("relay OFF");
  digitalWrite(SONOFF_RELAY, LOW);
  digitalWrite(SONOFF_LED, HIGH);
  delay(HTTP_RESPONSE_DELAY_TIME); 
}

void http_restart()
{
  device_restart();
}

void http_change_ssid()
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
    device_err();

  // save to flash
  int len = ssid.length();
  eeprom_write_buffer((const uint8_t*)&len, EEPROM_WIFI_SSID_OFFSET, EEPROM_DATA_LENGTH_OFFSET);
  eeprom_write_buffer((const uint8_t*)ssid.c_str(), ssid.length(), EEPROM_WIFI_SSID_OFFSET + EEPROM_DATA_LENGTH_OFFSET);

  len = password.length();
  eeprom_write_buffer((const uint8_t*)&len, EEPROM_DATA_LENGTH_OFFSET, EEPROM_WIFI_PASSWORD_OFFSET);
  eeprom_write_buffer((const uint8_t*)password.c_str(), password.length(), EEPROM_WIFI_PASSWORD_OFFSET + EEPROM_DATA_LENGTH_OFFSET);
}

bool http_change_ip()
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

void prepare_gpios()
{
  // setup led
  pinMode(SONOFF_LED, OUTPUT);
  digitalWrite(SONOFF_LED, LOW);

  // setup relay
  pinMode(SONOFF_RELAY, OUTPUT);
  digitalWrite(SONOFF_RELAY, LOW);

  //setup button
  pinMode(SONOFF_BUTTON, INPUT);
  attachInterrupt(SONOFF_BUTTON, btn_toggle_state, CHANGE);
}

void prepare_serial()
{
  Serial.begin(115200); 
  delay(5000);
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
  Serial.println("in load config");
  // check if eeprom initialized
  if (!epprom_is_initialize())
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

  http_server.on("/", HTTP_POST, [](){
    if(!http_server.authenticate(HTTP_AUTH_USER, HTTP_AUTH_PASSWORD))
      return http_server.requestAuthentication();
    http_server.send(200, "text/html", HTTP_RETURN_WEB_PAGE);
  });
  
  http_server.on("/on", HTTP_POST, [](){
    if(!http_server.authenticate(HTTP_AUTH_USER, HTTP_AUTH_PASSWORD))
      return http_server.requestAuthentication();
      
    http_on();
    http_server.send(200, "text/html", HTTP_RETURN_WEB_PAGE);
  });
  
  http_server.on("/off", HTTP_POST, [](){
    if(!http_server.authenticate(HTTP_AUTH_USER, HTTP_AUTH_PASSWORD))
      return http_server.requestAuthentication(); 

  http_off();
  http_server.send(200, "text/html", HTTP_AUTH_PASSWORD);
  });
  
  http_server.on("/set_network", HTTP_POST, [](){
    if(!http_server.authenticate(HTTP_AUTH_USER, HTTP_AUTH_PASSWORD))
      return http_server.requestAuthentication();
      
    delay(HTTP_RESPONSE_DELAY_TIME);
    http_server.send(200, "text/html", HTTP_RETURN_WEB_PAGE);
    
    http_change_ssid();    
  });

  http_server.on("/set_address", HTTP_POST, [](){
    if(!http_server.authenticate(HTTP_AUTH_USER, HTTP_AUTH_PASSWORD))
      return http_server.requestAuthentication();
      
    delay(HTTP_RESPONSE_DELAY_TIME);
    http_server.send(200, "text/html", HTTP_RETURN_WEB_PAGE);
    
    http_change_ip();    
  });

  http_server.on("/restart", HTTP_POST, [](){
    if(!http_server.authenticate(HTTP_AUTH_USER, HTTP_AUTH_PASSWORD))
      return http_server.requestAuthentication();
      
    delay(10);
    http_server.send(200, "text/html", HTTP_RETURN_WEB_PAGE);
    
    http_restart();    
  });
  
  http_server.begin();
  
  digitalWrite(SONOFF_LED, LOW);
  Serial.println("HTTP server started");
}
 
void loop(void){
  http_server.handleClient();
}

