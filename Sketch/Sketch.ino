
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <IPAddress.h>
#include <String.h>
#include <EEPROM.h>

ESP8266WebServer http_server(80);

void setup(void)
{
  Serial.println("in setup");
  componentes_setup();
}
 
void loop(void){
  http_server.handleClient();
}
