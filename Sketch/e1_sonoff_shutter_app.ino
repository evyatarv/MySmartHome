
#ifdef SHUTTER

#define SHUTTER_SML_FULL_OPEN_CLOSE   (18)    //time in sec for small shutter
//#define SHUTTER_SML_CRACKS            (2)     //time in sec for small shutter
//#define SHUTTER_SML_HALF              ((SHUTTER_FULL_OPEN_CLOSE - SHUTTER_CRACKS) >> 1)

void shutterSendCmd(int relay);
void shutterMove(bool upDown, int moveTime);

uint8_t g_max_time;
uint8_t g_crack_time;

void sonoff_dual_shutter(int index, uint8_t duration)
{
  if (duration > g_max_time)
    duration = g_max_time;

  //in shutter mode - in order to protect the motor it MUST NOT set both relay in the same time
  //therefore only one relay canbe active in a given time
  if ((index != first_relay) ||        
     (index != second_relay))
    return;

  sonoff_dual_led_tick();
  sonoff_dual_set_state(index, true);
  delay(duration * SECOND);
  sonoff_dual_set_state(index, false);
  sonoff_dual_led_tick();
}

void init_shutter_params()
{
  //uint8_t data_len = 0;
  uint8_t data[2] = {0};

  //set full move duration if needed
  eeprom_read_buffer(&data[0], 1, EEPROM_SHUTTER_FULL_DURATION);
  if (data[0] == 0)
  {
    data[0] = 1;
    data[1] = SHUTTER_SML_FULL_OPEN_CLOSE;
    eeprom_write_buffer(data, SHUTTER_FULL_DURATION_LENGTH + EEPROM_DATA_LENGTH_OFFSET, EEPROM_SHUTTER_FULL_DURATION, false);
  }
  //set cracks move duration if needed
  /*eeprom_read_buffer(&data_len[0], 1, EEPROM_SHUTTER_CRACKS_DURATION);
  if (data_len == 0)
  {
    data[0] = 1;
    data[1] = SHUTTER_SML_CRACKS;
    eeprom_write_buffer(data, SHUTTER_CRACKS_DURATION_LENGTH + EEPROM_DATA_LENGTH_OFFSET, EEPROM_SHUTTER_CRACKS_DURATION,false);
  }*/

  //read values and set globals
  eeprom_read_buffer(&g_max_time, 1, SHUTTER_FULL_DURATION_LENGTH);
  //eeprom_read_buffer(&g_crack_time, 1, SHUTTER_FULL_DURATION_LENGTH);
}

void update_full_move_duration(uint8_t duration)
{
  eeprom_write_buffer(&duration, SHUTTER_FULL_DURATION_LENGTH, EEPROM_SHUTTER_FULL_DURATION + EEPROM_DATA_LENGTH_OFFSET, false);
  g_max_time = duration;
}

//==========commod to all apps ===========

void init_device()
{
  init_dual_drv();

  init_shutter_params();
  
  g_max_time = SHUTTER_SML_FULL_OPEN_CLOSE;

  g_sonoff_dual_api.init = sonoff_dual_prepare_gpios;
  g_sonoff_dual_api.relay = sonoff_dual_shutter; 
  g_sonoff_dual_api.do_device_could_reset = sonoff_dual_restart;
  g_sonoff_dual_api.do_indicate = sonoff_dual_indicate;
  g_sonoff_dual_api.led = sonoff_dual_led;

}

#endif //SHUTTER
