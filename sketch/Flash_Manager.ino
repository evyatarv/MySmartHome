
void eeprom_read_buffer(uint8_t* buff, int len, int addr_offset)
{
  if (!buff || len < 0 || addr_offset < 0)
    return;
  
  for(int i=0; i < len ; i++)
    buff[i] = EEPROM.read(addr_offset+i);
}

void eeprom_write_buffer(const uint8_t* buff, int len, int addr_offset)
{
  if (!buff || len < 0 || addr_offset < 0)
    return;
    
  for(int i=0; i < len ; i++)
    EEPROM.write(addr_offset+i, buff[i]);

  EEPROM.commit();
}

bool epprom_is_initialize(bool to_init)
{
  uint8_t first_boot_str[EEPROM_FIRST_BOOT_STR_LEN] = {0};

  eeprom_read_buffer(first_boot_str, EEPROM_FIRST_BOOT_STR_LEN, 0);

  if (strncmp((const char*)first_boot_str, EEPROM_FIRST_BOOT_STR, EEPROM_FIRST_BOOT_STR_LEN))
  {
    if(to_init) 
      eeprom_init();
    
    return false;
  }

  return true;
}

void eeprom_init()
{
  String tmp = (const char*)EEPROM_FIRST_BOOT_STR;
  Serial.println("init EEPROM ...\n");
  Serial.println(tmp);
    
  eeprom_write_buffer((const uint8_t*)tmp.c_str(), EEPROM_FIRST_BOOT_STR_LEN, 0);
}

