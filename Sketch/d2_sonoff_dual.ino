
#ifdef SONOFF_DUAL

#define SONOFF_DUAL_LED        (13)

//globals vars
device_api g_sonoff_dual_api;
relays g_sonoff_dual_status = none;    //if init is not to zero, you need to move the delay in "sonoff_dual_send_relay_cmd" to the start of the function


//================ Device Driver function - private ===================

void sonoff_dual_send_relay_cmd()
{
  delay(DEIVCE_DELAY_AFTER_SER_WRITE); // wait for a 0.5 seconds, Omron LY2N-J on/off time is 25ms max
  Serial.write(0xA0);
  Serial.write(0x04);
  Serial.write(g_sonoff_dual_status);                //00 - all off, 01 - relay 1 on, 02 - relay 2 on, 03 - both on
  Serial.write(0xA1);
  Serial.flush();
}

//================ Device Driver function - private - END =============


//================ Device Driver function - public ===================

/* the table for the function logic
 *  
 *  relay  g_stat on  xor res
 *  01     00     0   01  00 noChange
 *  01     01     0   00  00    
 *  01     10     0   11  10 noChange
 *  01     11     0   10  10                       
 *  10     00     0   10  00 noChange 
 *  10     01     0   11  01 noChange 
 *  10     10     0   00  00    
 *  10     11     0   01  01    
 *               
 *  01     00     1   01  01
 *  01     01     1   00  00 noChange
 *  01     10     1   11  11 
 *  01     11     1   10  11 noChange
 *  10     00     1   10  10
 *  10     01     1   11  11
 *  10     10     1   00  10 noChange
 *  10     11     1   01  11 noChange
 * 
 *  off => relay & g_stat == 0 => noChange
 *  on => relay & g_stat == relay => noChange
 * 
 */
void sonoff_dual_set_state(byte relay, bool set_on)
{
  byte res = (set_on)? relay : 0;
  byte tmp = (byte)g_sonoff_dual_status;
  
  if (relay < 1 || relay > 2) //if not 1 or 2 then iligal
    return;
  
  /* - for devices that supports more than 2 relays
  byte mask = 0;
  SETBIT(mask, relay);
  relay = mask;
  */
  
  if (relay & tmp == res)
  {
    return;
  }

  tmp = tmp ^ relay;
  g_sonoff_dual_status = (relays)tmp;
  
  sonoff_dual_send_relay_cmd();
}

#ifndef SHUTTER

void sonoff_dual_set_all(bool to_on)
{
  if ((to_on && g_sonoff_dual_status == both_relays) ||
      (!to_on && g_sonoff_dual_status == none))
        return; //noting to do

  g_sonoff_dual_status = (to_on?) both_relays : none;

  sonoff_dual_send_relay_cmd();
}

#endif

byte get_sonoff_dual_state()
{
  return g_sonoff_dual_status;
}

void sonoff_dual_prepare_serial()
{
  Serial.begin(UART_CONTROLER_SPEED);
  delay(SECOND);
}

void sonoff_dual_led_on()
{
  digitalWrite(SONOFF_DUAL_LED, LOW);
}

void sonoff_dual_led_off()
{
  digitalWrite(SONOFF_DUAL_LED, HIGH);
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
  digitalWrite(SONOFF_DUAL_LED, !state);
}

void sonoff_dual_indicate()
{
  sonoff_dual_led_tick();
}

void sonoff_dual_led(int index, int cmd)
{
  if (index > 0) //have only one led
    return;

  if (cmd == led_on)
    sonoff_dual_led_on();
  else if (cmd == led_off)
    sonoff_dual_led_off();
  else
    Serial.println("SONOFF DUAL INVALID COMMAND");
}

void sonoff_dual_restart()
{
  Serial.println("PREFORMING DEVICE RESET ...");
  ESP.restart();
  delay(1000);
}

void start_init()
{
  sonoff_dual_prepare_serial();
  sonoff_dual_prepare_gpios();
  Serial.println("INIT SONOFF DUAL");
  
  g_sonoff_dual_api.init = sonoff_dual_prepare_gpios;

}

void end_init()
{
  g_sonoff_dual_status = none;
  
  set_device_api(&g_sonoff_dual_api);
  
  sonoff_dual_led_on();
}

//================ Device Driver function - public - END =============

#endif //SONOFF_DUAL
