
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

#endif //SHUTTER
