
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <IPAddress.h>
#include <String.h>
#include <EEPROM.h>

ESP8266WebServer http_server(80);

void device_indicator();
void device_restart();
void device_err();

bool wifi_connect(const char* ssid, const char* password, bool set_static_ip);

void prepare_gpios(); 
void prepare_serial();
void init_eeprom();
void load_config(bool set_static_ip);
bool connect_wifi_network(bool set_static_ip);
void init_http_server();
void init_device();

void setup(void)
{
  bool set_static_ip = false; 

  init_eeprom();
  
  prepare_gpios();

  prepare_serial();

  load_config(&set_static_ip);

  if (!connect_wifi_network(set_static_ip))
    device_err();

  init_http_server();

  init_device();
}
 
void loop(void){
  http_server.handleClient();
}
