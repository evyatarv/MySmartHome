
//void init_device()
//{
//  Serial.begin(19200);    //uart speed to relays micro-controller
//}


enum activeRelay {
  none,
  first,
  second
};

//shutter movment in sec (time)
#define SHUTTER_SML_FULL_OPEN_CLOSE 18000
#define SHUTTER_SML_CRACKS 2500
#define SHUTTER_SML_HALF ((SHUTTER_FULL_OPEN_CLOSE - SHUTTER_CRACKS) >> 1)

void shutterSendCmd(activeRelay relay);
void shutterMove(bool upDown, int moveTime);

int shutterState = 0;

void shutterInit()
{
  shutterMove(false, SHUTTER_SML_FULL_OPEN_CLOSE);
  shutterState = 0;
}

/* shutterMove
 *  move the shutter up or down
 *  
 *  upDown[IN] - set the direction of the shutter, up - true, down - false
 *  moveTime - set the amount of time to move the shutter
 */
void shutterMove(bool upDown, int moveTime){

  if (upDown){
    shutterSendCmd(first);
  }
  else{
    shutterSendCmd(second);
  }
  
  delay(moveTime);
  shutterState += (moveTime * ((upDown)? 1:-1));
}

/* shutterSendCmd
 *  send command to micro-controller to activate the relay
 * 
 * relay[IN] - which relay to activate: 0 - none of them, 1 - first relay, 2 - second relay
 */
void shutterSendCmd(activeRelay relay){
  
  //first close the relays to be on the safe side that the will not be activate together 
  Serial.write(0xA0);
  Serial.write(0x04);
  Serial.write(0x00);               //00 - all off, 01 - relay 1 on, 02 - relay 2 on, 03 - both on
  Serial.write(0xA1);
  Serial.flush();
  delay(500);                       // wait for a 0.5 seconds, Omron LY2N-J on/off time is 25ms max

  //DO NOT change or remove this check!
  //this is to make sure that both relays will NOT be activate together at all times (even if the enum has changed)
  //it can burn the shutter motor if both relays will work!!!
  if (relay == none)              
    return;
  else if ((relay == first) && ((int)relay == 0x01) ||
           (relay == second) && ((int)relay == 0x02)) { //check that the values are correct, meaning only ONE or TWO
              
              // do the code in the section below
            }
  else
    return;        
  
  //if need to activate one of the relays - active it :)
  Serial.write(0xA0);
  Serial.write(0x04);
  Serial.write((int)relay);        // 00 - all off, 01 - relay 1 on, 02 - relay 2 on, 03 - both on
  Serial.write(0xA1);
  Serial.flush();
  delay(500);                       // wait for a 0.5 second
}




  /*test both on/off every sec
  Serial.write(0xA0);
  Serial.write(0x04);
  Serial.write(0x03); 
  Serial.write(0xA1);
  Serial.flush();
  delay(1000);                       // wait for a second 
  Serial.write(0xA0);
  Serial.write(0x04);
  Serial.write(0x00);               //00 - all off, 01 - relay 1 on, 02 - relay 2 on, 03 - both on
  Serial.write(0xA1);
  Serial.flush();
  delay(1000);                       // wait for a second
  */
