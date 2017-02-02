
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IPAddress.h>
#include <String.h>

MDNSResponder mdns;

// device
#define DEVICE_STATUS_ERR_DELAY_TIME  (100)
#define DEVICE_ERR_MESSAGE_PULL_TIME  (1000)
#define DEVICE_RESTART_TIMEOUT        (40)
unsigned int restart_counter = 0;

// wifi connection
#define WIFI_STATUS_PULL_INTERVAL (500)
#define WIFI_CONNECTION_TIMEOUT   (120)

// configurations inputs sizes
#define WIFI_MAX_SSID_LENGTH      (32)
#define WIFI_MAX_PASSWORD_LENGTH  (32)
#define WIFI_MAX_IPV4_LENGTH      (15)
#define WIFI_MIN_IPV4_LENGTH      (7)

// wifi configuration 
enum WIFI_CONF_NUM{ SSID_NAME=0, PASSWORD, GATEWAY, MASK_IP, DEVICE_IP, MAX_CONF_LEN };
const char* WIFI_CONF[] = { "ssid", "password", "gateway", "mask_ip", "device_ip" };
const char* DEFAULT_SSID_NAME = "******";
const char* DEFAULT_SSID_PASSWORD = "******";
String CURRENT_SSID_NAME = DEFAULT_SSID_NAME;
String CURRENT_SSID_PASSWORD = DEFAULT_SSID_PASSWORD;
IPAddress DEFAULT_GATEWAY;
IPAddress SUB_MASK_IP;
IPAddress DEV_IP;

// esp8266 gpios
#define SONOFF_BUTTON     (0)
#define SONOFF_RELAY      (12)
#define SONOFF_LED        (13)
#define SONOFF_INPUT      (14)

// http authentication 
const char* USER = "******";
const char* USER_PASSWORD = "******";

// return http page
String RETURN_WEB_PAGE = "";

ESP8266WebServer server(80);

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

bool wifi_try_connect(const char* ssid, const char* password, bool is_static_ip)
{
  int timeout_counter = 0; 

  WiFi.disconnect();
  delay(2000);
  
  Serial.print("Connecteing to: ");
  Serial.println(ssid);

  // start wifi
  WiFi.begin(ssid, password); 

  // set static ip or DHCP
  if (is_static_ip)
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
  CURRENT_SSID_NAME = ssid;
  CURRENT_SSID_PASSWORD = password;
  
  return true;
}

bool wifi_connect(const char* ssid, const char* password)
{
  // Set ESP to stasion mode
  WiFi.mode(WIFI_STA);
  Serial.println("WIFI mode set to stasion.");
  
  if (wifi_try_connect(ssid, password, false))
    return true; 
     
  Serial.print("Fail to connect: ");
  Serial.println(ssid);
  Serial.println("Try connecting previous ssid");

  if (wifi_try_connect(CURRENT_SSID_NAME.c_str(), CURRENT_SSID_PASSWORD.c_str(), false))
    return true;

  Serial.print("Fail to connect: ");
  Serial.println(ssid);
  Serial.println("Try connecting default ssid");

  if (wifi_try_connect(DEFAULT_SSID_NAME, DEFAULT_SSID_PASSWORD, false))
    return true;

  return false;
}

void http_on()
{
  Serial.println("relay ON");
  digitalWrite(SONOFF_RELAY, HIGH);
  delay(1000);
}

void http_off()
{
  Serial.println("relay OFF");
  digitalWrite(SONOFF_RELAY, LOW);
  delay(1000); 
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

  conf = server.arg(WIFI_CONF[SSID_NAME]);
  if (conf.length())
  {
    ssid = conf;
    Serial.print("SSID set to: ");
    Serial.println(ssid);
  }
  else
    return;

  conf = server.arg(WIFI_CONF[PASSWORD]);
  if (conf.length())
  {
    password = conf;
    Serial.print("PASSWORD set to: ");
    Serial.println(password);
  }

  if (!wifi_connect(ssid.c_str(), password.c_str()))
    device_err();
}

bool http_change_ip()
{
  String conf;
  
  do
  {
    conf = server.arg(WIFI_CONF[DEVICE_IP]);
    if (conf.length() > WIFI_MIN_IPV4_LENGTH &&
          conf.length() < WIFI_MAX_IPV4_LENGTH)
    {
      DEV_IP.fromString(conf);
      Serial.print("DEVICE IP set to: ");
      Serial.println(DEV_IP);
    }
    else
      break;
  
    conf = server.arg(WIFI_CONF[MASK_IP]);
    if (conf.length() > WIFI_MIN_IPV4_LENGTH &&
          conf.length() < WIFI_MAX_IPV4_LENGTH)
    {
      SUB_MASK_IP.fromString(conf);
      Serial.print("MUSK IP set to: ");
      Serial.println(SUB_MASK_IP);
    }
    else
      break;
  
    conf = server.arg(WIFI_CONF[GATEWAY]);
    if (conf.length() > WIFI_MIN_IPV4_LENGTH &&
          conf.length() < WIFI_MAX_IPV4_LENGTH)
    {
      DEFAULT_GATEWAY.fromString(conf);
      Serial.print("GATEWAY IP set to: ");
      Serial.println(DEFAULT_GATEWAY);
    }
    else
      break;
      
    return wifi_try_connect(CURRENT_SSID_NAME.c_str(), CURRENT_SSID_PASSWORD.c_str(), true);
  }
  while(false);
    
  return false;
}

void prepare_gpios()
{
  // setup led
  pinMode(SONOFF_LED, OUTPUT);
  digitalWrite(SONOFF_LED, HIGH);

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

void setup(void)
{
  prepare_gpios();

  prepare_serial();
  
  if (!wifi_connect(DEFAULT_SSID_NAME, DEFAULT_SSID_PASSWORD))
    device_err();

  server.on("/", HTTP_POST, [](){
    if(!server.authenticate(USER, USER_PASSWORD))
      return server.requestAuthentication();
    server.send(200, "text/html", RETURN_WEB_PAGE);
  });
  
  server.on("/on", HTTP_POST, [](){
    if(!server.authenticate(USER, USER_PASSWORD))
      return server.requestAuthentication();
      
    http_on();
    server.send(200, "text/html", RETURN_WEB_PAGE);
  });
  
  server.on("/off", HTTP_POST, [](){
    if(!server.authenticate(USER, USER_PASSWORD))
      return server.requestAuthentication(); 

    http_off();
    server.send(200, "text/html", RETURN_WEB_PAGE);
  });
  
  server.on("/set_network", HTTP_POST, [](){
    if(!server.authenticate(USER, USER_PASSWORD))
      return server.requestAuthentication();
      
    delay(1000);
    server.send(200, "text/html", RETURN_WEB_PAGE);
    
    http_change_ssid();    
  });

  server.on("/set_address", HTTP_POST, [](){
    if(!server.authenticate(USER, USER_PASSWORD))
      return server.requestAuthentication();
      
    delay(1000);
    server.send(200, "text/html", RETURN_WEB_PAGE);
    
    http_change_ip();    
  });

  server.on("/restart", HTTP_POST, [](){
    if(!server.authenticate(USER, USER_PASSWORD))
      return server.requestAuthentication();
      
    delay(1000);
    server.send(200, "text/html", RETURN_WEB_PAGE);
    
    http_restart();    
  });
  
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();
}

