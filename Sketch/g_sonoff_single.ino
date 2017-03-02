
// gpios
#define SONOFF_BUTTON     (0)
#define SONOFF_RELAY      (12)
#define SONOFF_LED        (13)
#define SONOFF_INPUT      (14)


device_api api;

void sonoff_single_prepare_serial()
{
  Serial.begin(115200); 
  delay(5000);
}

void sonoff_single_led_tick()
{
  //toggle state
  int state = digitalRead(SONOFF_LED);
  digitalWrite(SONOFF_LED, !state);
}

void sonoff_single_relay(int cmd)
{
  switch(cmd)
  {
    case relay_on:
    case all_relays_on:
    {
        Serial.println("relay ON");
        digitalWrite(SONOFF_RELAY, HIGH);
        digitalWrite(SONOFF_LED, LOW);
    }break;

    case relay_off:
    case all_relays_off:
    {
      Serial.println("relay OFF");
      digitalWrite(SONOFF_RELAY, LOW);
      digitalWrite(SONOFF_LED, HIGH);
    }break;

    default: {return;}
  }
}

void sonoff_single_led_on()
{
  digitalWrite(SONOFF_LED, LOW);
}

void sonoff_single_led_off()
{
  digitalWrite(SONOFF_LED, HIGH);
}

void sonoff_single_restart()
{
  Serial.println("PREFORMING DEVICE RESET ...");
  ESP.restart();
  delay(1000);
}

void sonoff_single_enter_err_mode()
{
  unsigned int counter = 0;
  
  int buttonState = LOW;
  
  while (true)
  {
    // Print err msg to serial every ~1 min
    if (!(++counter % DEVICE_ERR_MESSAGE_PULL_TIME))
      Serial.println("device err.. DO RESET");
        
    delay(DEVICE_STATUS_ERR_DELAY_TIME);
    sonoff_single_led_tick();

    buttonState = digitalRead(SONOFF_BUTTON);
    if (buttonState == LOW) 
    {
      if (!(++restart_counter % DEVICE_RESTART_TIMEOUT))
      {
        sonoff_single_restart();
      }
    }
  }
}

void sonoff_single_btn_toggle_state()
{
  restart_counter = 0;
}

void sonoff_single_prepare_gpios()
{
  // setup led
  pinMode(SONOFF_LED, OUTPUT);
  digitalWrite(SONOFF_LED, LOW);

  // setup relay
  pinMode(SONOFF_RELAY, OUTPUT);
  digitalWrite(SONOFF_RELAY, LOW);

  //setup button
  pinMode(SONOFF_BUTTON, INPUT);
  attachInterrupt(SONOFF_BUTTON, sonoff_single_btn_toggle_state, CHANGE);
}

void sonoff_single_indicate()
{
  sonoff_single_led_tick();
}

void init_device()
{
  sonoff_single_prepare_serial();
  
  device_init init = sonoff_single_prepare_gpios;
  
  api.relay_arr[0] = sonoff_single_relay; 
  api.do_device_could_reset = sonoff_single_restart;
  api.do_indicate = sonoff_single_indicate;
  api.enter_err_mode = sonoff_single_enter_err_mode; 
  
  set_device_api(api);
  
  sonoff_single_led_on();
}


