
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

MDNSResponder mdns;

// network credentials
const char* SSID_NAME = "*******";
const char* SSID_PASSWORD = "*******";

// http authentication 
const char* USER = "*******";
const char* USER_PASSWORD = "*******";

// return http page
String RETURN_WEB_PAGE = "";

int gpio13Led = 13;
int gpio12Relay = 12;

ESP8266WebServer server(80);

void wifi_connect(const char* ssid, const char* password)
{
  WiFi.begin(ssid, password);
  Serial.println("");

  // Set ESP to stasion mode
  WiFi.mode(WIFI_STA);
  Serial.print("WIFI mode set to stasion.");
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // print wifi connection data
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
}

void setup(void){
  // preparing GPIOs
  pinMode(gpio13Led, OUTPUT);
  digitalWrite(gpio13Led, HIGH);
  
  pinMode(gpio12Relay, OUTPUT);
  digitalWrite(gpio12Relay, HIGH);
 
  Serial.begin(115200); 
  delay(5000);

  wifi_connect(SSID_NAME, SSID_PASSWORD); 
  
  server.on("/", [](){
    if(!server.authenticate(USER, USER_PASSWORD))
      return server.requestAuthentication();
    server.send(200, "text/html", RETURN_WEB_PAGE);
  });
  server.on("/on", [](){
    if(!server.authenticate(USER, USER_PASSWORD))
      return server.requestAuthentication();
    server.send(200, "text/html", RETURN_WEB_PAGE);
    digitalWrite(gpio13Led, LOW);
    digitalWrite(gpio12Relay, HIGH);
    delay(1000);
  });
  server.on("/off", [](){
    if(!server.authenticate(USER, USER_PASSWORD))
      return server.requestAuthentication();      
    server.send(200, "text/html", RETURN_WEB_PAGE);
    digitalWrite(gpio13Led, HIGH);
    digitalWrite(gpio12Relay, LOW);
    delay(1000); 
  });
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();
}

