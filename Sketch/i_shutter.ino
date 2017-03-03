
<<<<<<< HEAD
//shutter movment in sec (time)
#define SHUTTER_SML_FULL_OPEN_CLOSE 18000
#define SHUTTER_SML_CRACKS 2500
#define SHUTTER_SML_HALF ((SHUTTER_FULL_OPEN_CLOSE - SHUTTER_CRACKS) >> 1)

void shutterSendCmd(int relay);
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
    sonoff_dual_relay(first_relay, 1);
  }
  else{
    sonoff_dual_relay(second_relay, 1);
  }
  
  delay(moveTime);
  shutterState += (moveTime * ((upDown)? 1:-1));
}
=======
>>>>>>> 2625d562d2bc8e3ffa3fe42743278e56af33d850
