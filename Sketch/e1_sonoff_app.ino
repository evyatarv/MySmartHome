


#ifdef SWITCHS

byte g_sonoff_dual_relay_status = 0;


void sonoff_dual_relay(int index, int cmd)
{
  byte relay_tmp_status = g_sonoff_dual_relay_status; 
  
  if (cmd == relay_on)
    SET_BIT(relay_tmp_status, index);
  else if (cmd == relay_off)
    CLEAR_BIT(relay_tmp_status, index);
  else
    Serial.println("SONOFF DUAL WRONG COMMAND");

  // DEBUG - remove
  Serial.print("SONOFF DUAL COMMAND: ");
  Serial.println(relay_tmp_status);
  
  //DO NOT change or remove this check!
  //this is to make sure that both relays will NOT be activate together at all times (even if the enum has changed)
  //it can burn the shutter motor if both relays will work!!!
  if (relay_tmp_status <= 0x02 && relay_tmp_status >= 0x00 )//check that the values are correct, meaning only ONE or TWO  or NONE
  {
   /* 
   * first close the relays to be on the safe 
   * side that the will not be activate together 
   */
    sonoff_dual_send_relay_cmd(0);              
    sonoff_dual_send_relay_cmd(g_sonoff_dual_relay_status);

    g_sonoff_dual_relay_status = relay_tmp_status;
  }
  else
  {
    Serial.println("SONOFF DUAL INVALID COMMAND");
    return;        
  }
}

#endif

#ifdef SHUTTER
//shutter movment in sec (time)
#define SHUTTER_SML_FULL_OPEN_CLOSE   (18000)
#define SHUTTER_SML_CRACKS            (2500)
#define SHUTTER_SML_HALF              ((SHUTTER_FULL_OPEN_CLOSE - SHUTTER_CRACKS) >> 1)

void shutterSendCmd(int relay);
void shutterMove(bool upDown, int moveTime);

int g_max_time;

void sonoff_dual_shutter(int index, int duration)
{
  if (duration < g_max_time)
    duration = g_max_time;
  
  if ((index != first_relay) ||
     (index != second_relay))
    return;

  sonoff_dual_set_state(index, true);
  delay(duration);
  sonoff_dual_set_state(index, false);

}

#endif

//==========commod to all apps ===========

void init_device()
{
  start_init();

  g_max_time = SHUTTER_SML_FULL_OPEN_CLOSE;
  
  g_sonoff_dual_api.relay = sonoff_dual_shutter; 
  g_sonoff_dual_api.do_device_could_reset = sonoff_dual_restart;
  g_sonoff_dual_api.do_indicate = sonoff_dual_indicate;
  g_sonoff_dual_api.led = sonoff_dual_led;
  
  end_init();
}

