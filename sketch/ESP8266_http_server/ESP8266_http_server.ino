
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
IPAddress MASK_IP;
IPAddress DEVICE_IP;

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
        Serial.println("PREFORMING DEVICE RESET ...");
        ESP.restart();
        delay(1000);
      }
    }
  }
}

void btn_toggle_state()
{
  restart_counter = 0;
}

bool wifi_try_connect(const char* ssid, const char* password)
{
  int timeout_counter = 0; 
  
  Serial.print("Connecteing to: ");
  Serial.println(ssid);

  // start wifi
  WiFi.begin(ssid, password); 

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

  // set device dns name
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

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
  
  if (wifi_try_connect(ssid, password))
    return true; 
     
  Serial.print("Fail to connect: ");
  Serial.println(ssid);
  Serial.println("Try connecting previous ssid");

  if (wifi_try_connect(CURRENT_SSID_NAME.c_str(), CURRENT_SSID_PASSWORD.c_str()))
    return true;

  Serial.print("Fail to connect: ");
  Serial.println(ssid);
  Serial.println("Try connecting default ssid");

  if (wifi_try_connect(DEFAULT_SSID_NAME, DEFAULT_SSID_PASSWORD))
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
      DEVICE_IP.fromString(conf);
      Serial.print("DEVICE IP set to: ");
      Serial.println(DEVICE_IP);
    }
    else
      break;
  
    conf = server.arg(WIFI_CONF[MUSK_IP]);
    if (conf.length() > WIFI_MIN_IPV4_LENGTH &&
          conf.length() < WIFI_MAX_IPV4_LENGTH)
    {
      MASK_IP.fromString(conf);
      Serial.print("MUSK IP set to: ");
      Serial.println(MUSK_IP);
    }
    else
      break;
  
    conf = server.arg(WIFI_CONF[GATEWAY]);
    if (conf.length() > WIFI_MIN_IPV4_LENGTH &&
          conf.length() < WIFI_MAX_IPV4_LENGTH)
    {
      DEVICE_IP.fromString(conf);
      Serial.print("GATEWAY IP set to: ");
      Serial.println(GATEWAY);
    }
    else
      break;

  WiFi.config(DEVICE_IP, GATEWAY, MASK_IP);
  return true;
  }
  while(false);
    
  return false;
}

int http_config()
{
  int status = 0;
  
  Serial.println("CONFIG starts ...");

  if (server.args() == 0)
    return 0;

  http_change_ssid();
  
  http_change_ip();
  
  delay(1000);

  return 0;
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

  //TODO: void on(const String &uri, HTTPMethod method, THandlerFunction fn);
  server.on("/", [](){
    if(!server.authenticate(USER, USER_PASSWORD))
      return server.requestAuthentication();
    server.send(200, "text/html", RETURN_WEB_PAGE);
  });
  
  server.on("/on", [](){
    if(!server.authenticate(USER, USER_PASSWORD))
      return server.requestAuthentication();
    http_on();
    server.send(200, "text/html", RETURN_WEB_PAGE);
  });
  
  server.on("/off", [](){
    if(!server.authenticate(USER, USER_PASSWORD))
      return server.requestAuthentication(); 

    http_off();
    server.send(200, "text/html", RETURN_WEB_PAGE);
  });
  
  server.on("/config", [](){
    if(!server.authenticate(USER, USER_PASSWORD))
      return server.requestAuthentication();
      
    http_config();
    server.send(200, "text/html", RETURN_WEB_PAGE);
  });
  
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();
}

