
#define SONOFF_DUAL_LED        (13)

#define SET_BIT(x, index)   (x |= 1 << index)
#define CLEAR_BIT(x, index) (x &= ~(1 << index))

device_api g_sonoff_dual_api;

byte g_sonoff_dual_relay_status = 0;

void sonoff_dual_prepare_serial()
{
  Serial.begin(19200);    //uart speed to relays micro-controller
  delay(5000);
}

void sonoff_dual_relay(int index, int cmd)
{
  if (cmd == relay_on)
    SET_BIT(g_sonoff_dual_relay_status, index);
  else if (cmd == relay_off)
    CLEAR_BIT(g_sonoff_dual_relay_status, index);
  else
    Serial.println("SONOFF DUAL WRONG COMMAND");

  // DEBUG - remove
  Serial.print("SONOFF DUAL COMMAND: ");
  Serial.println(g_sonoff_dual_relay_status);
  
  //DO NOT change or remove this check!
  //this is to make sure that both relays will NOT be activate together at all times (even if the enum has changed)
  //it can burn the shutter motor if both relays will work!!!
  if (g_sonoff_dual_relay_status <= 0x02 && g_sonoff_dual_relay_status >= 0x00 )//check that the values are correct, meaning only ONE or TWO  or NONE
  {
    /* 
   * first close the relays to be on the safe 
   * side that the will not be activate together 
   */
    sonoff_dual_send_relay_cmd(0);              
    sonoff_dual_send_relay_cmd(g_sonoff_dual_relay_status);
  }
  else
  {
    Serial.println("SONOFF DUAL INVALID COMMAND");
    return;        
  }
}


void sonoff_dual_send_relay_cmd(int cmd)
{
  Serial.write(0xA0);
  Serial.write(0x04);
  Serial.write(cmd);                //00 - all off, 01 - relay 1 on, 02 - relay 2 on, 03 - both on
  Serial.write(0xA1);
  Serial.flush();
  delay(DEIVCE_DELAY_AFTER_SER_WRITE); // wait for a 0.5 seconds, Omron LY2N-J on/off time is 25ms max
}

void sonoff_dual_led_on()
{
  digitalWrite(SONOFF_SINGLE_LED, LOW);
}

void sonoff_dual_led_off()
{
  digitalWrite(SONOFF_SINGLE_LED, HIGH);
}

void sonoff_dual_prepare_gpios()
{
  // setup led
  pinMode(SONOFF_DUAL_LED, OUTPUT);
}

void sonoff_dual_led_tick()
{
  //toggle state
  int state = digitalRead(SONOFF_DUAL_LED);
  digitalWrite(SONOFF_SINGLE_LED, !state);
}

void sonoff_dual_indicate()
{
  sonoff_dual_led_tick();
}

void sonoff_dual_restart()
{
  Serial.println("PREFORMING DEVICE RESET ...");
  ESP.restart();
  delay(1000);
}

void init_device()
{
  sonoff_dual_prepare_serial();

  sonoff_dual_prepare_gpios();
  
  Serial.println("INIT SONOFF DUAL");
  
  g_sonoff_dual_api.init = sonoff_dual_prepare_gpios;
  
  g_sonoff_dual_api.relay_op = sonoff_dual_relay; 
  g_sonoff_dual_api.do_device_could_reset = sonoff_dual_restart;
  g_sonoff_dual_api.do_indicate = sonoff_dual_indicate;
  
  set_device_api(&g_sonoff_dual_api);
  
  sonoff_dual_led_on();
}
