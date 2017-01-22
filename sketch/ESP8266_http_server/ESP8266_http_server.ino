
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Ticker.h>

MDNSResponder mdns;

// wifi connection
#define WIFI_STATUS_PULL_INTERVAL (500)
#define WIFI_CONNECTION_TIMEOUT   (60)
#define WIFI_MAX_SSID_LENGTH      (32)
#define WIFI_MAX_PASSWORD_LENGTH  (32)

// esp8266 gpios
#define SONOFF_BUTTON    0
#define SONOFF_RELAY    12
#define SONOFF_LED      13
#define SONOFF_INPUT    14

// network credentials
const char* DEFAULT_SSID_NAME = "******";
const char* DEFAULT_SSID_PASSWORD = "******";
const char* CURRENT_SSID_NAME = DEFAULT_SSID_NAME;
const char* CURRENT_SSID_PASSWORD = DEFAULT_SSID_PASSWORD;

// http authentication 
const char* USER = "******";
const char* USER_PASSWORD = "******";

// return http page
String RETURN_WEB_PAGE = "";

Ticker ticker;

ESP8266WebServer server(80);

void led_tick()
{
  //toggle state
  int state = digitalRead(SONOFF_LED);  // get the current state of GPIO1 pin
  digitalWrite(SONOFF_LED, !state);     // set pin to the opposite state
}

bool wifi_try_connect(const char* ssid, const char* password)
{
  int timeout_counter = 0; 
  
  // Set ESP to stasion mode
  WiFi.mode(WIFI_STA);
  Serial.println("WIFI mode set to stasion.");
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

void wifi_connect(const char* ssid, const char* password)
{
  if (wifi_try_connect(ssid, password))
    return; 

  Serial.print("Fail to connect: ");
  Serial.println(ssid);
  Serial.println("Try connecting previous ssid");
  
  if (wifi_try_connect(CURRENT_SSID_NAME, CURRENT_SSID_PASSWORD))
    return;

  Serial.print("Fail to connect: ");
  Serial.println(ssid);
  Serial.println("Try connecting default ssid");

  if (wifi_try_connect(DEFAULT_SSID_NAME, DEFAULT_SSID_PASSWORD))
    return;

  //TODO: open device as AP if fail do restart
}

void http_on()
{
  Serial.println("relay ON");
  digitalWrite(SONOFF_LED, LOW);
  digitalWrite(SONOFF_RELAY, HIGH);
  delay(1000);
}

void http_off()
{
  Serial.println("relay OFF");
  digitalWrite(SONOFF_LED, HIGH);
  digitalWrite(SONOFF_RELAY, LOW);
  delay(1000); 
}

int http_rest_config()
{
  Serial.println("CONFIG starts ...");
  String arg = server.arg("ip"); 
    
  Serial.println(arg.length());
  delay(1000);
}

void setup(void)
{
  // preparing GPIOs
  pinMode(SONOFF_LED, OUTPUT);
  digitalWrite(SONOFF_LED, HIGH);
  
  pinMode(SONOFF_RELAY, OUTPUT);
  digitalWrite(SONOFF_RELAY, HIGH);
 
  Serial.begin(115200); 
  delay(5000);

  wifi_connect(DEFAULT_SSID_NAME, DEFAULT_SSID_PASSWORD); 
  
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

    http_rest_config();
    server.send(200, "text/html", RETURN_WEB_PAGE);
  });
  
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();
}

