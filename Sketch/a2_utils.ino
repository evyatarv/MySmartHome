
void hexDump (void *addr, int len, int r_size) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    if (len == 0) {
        PRINT_D("%s", "  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        PRINT_D("  NEGATIVE LENGTH: %i\n",len);
        return;
    }
    Serial.println();
    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of r_size means new line (with line offset).
        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
            {
                PRINT_D("  %s\n", buff);
            }
            // Output the offset.
            PRINT_D("  %04x ", i);
        }

        // Now the hex code for the specific character.
        PRINT_D(" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        PRINT_D("s", "   ");
        i++;
    }

    // And print the final ASCII bit.
    PRINT_D("  %s\n\n", buff);
}

void eeprom_read_buffer(uint8_t* dest_buff, int len, int addr_offset)
{
  if (!dest_buff || len < 0 || addr_offset < 0)
    return;
  
  for(int i=0; i < len ; i++)
    dest_buff[i] = EEPROM.read(addr_offset+i);
}

void eeprom_write_buffer(const uint8_t* src_buff, int len, int addr_offset, bool pattrn)
{
  uint8_t data;
  
  if (!src_buff || len < 0 || addr_offset < 0)
    return;

  data = src_buff[0];
  for(int i=0; i < len ; i++){
    if (!pattrn)
      data = src_buff[i];    
    EEPROM.write(addr_offset+i, data);
  }
  
  EEPROM.commit();
}

