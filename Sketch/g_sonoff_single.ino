
// gpios
#define SONOFF_BUTTON     (0)
#define SONOFF_RELAY      (12)
#define SONOFF_LED        (13)
#define SONOFF_INPUT      (14)

struct _device_api api;

  
void device_led_tick()
{
  //toggle state
  int state = digitalRead(SONOFF_LED);
  digitalWrite(SONOFF_LED, !state);
}

void device_relay_on()
{
  Serial.println("relay ON");
  digitalWrite(SONOFF_RELAY, HIGH);
  digitalWrite(SONOFF_LED, LOW);
}

void device_relay_off()
{
  Serial.println("relay OFF");
  digitalWrite(SONOFF_RELAY, LOW);
  digitalWrite(SONOFF_LED, HIGH);
}

void device_led_on()
{
  digitalWrite(SONOFF_LED, LOW);
}

void device_led_off()
{
  digitalWrite(SONOFF_LED, HIGH);
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
    device_led_tick();

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

void device_indicator()
{
  device_led_tick();
}

void init_device()
{
  api.relay_on_arr[0] = device_relay_on; 
  api.relay_off_arr[0] = device_relay_off; 
  api.do_device_could_reset = device_restart;

  set_web_service_device_api(&api);
  
  device_led_on();
}


