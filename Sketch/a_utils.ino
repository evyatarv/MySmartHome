
void hexDump (void *addr, int len, int r_size) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    if (len == 0) {
        Serial.printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        Serial.printf("  NEGATIVE LENGTH: %i\n",len);
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
                Serial.printf ("  %s", buff);
                Serial.println();
            }
            // Output the offset.
            Serial.printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        Serial.printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        Serial.printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    Serial.printf ("  %s\n", buff);
    Serial.println();
}

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

