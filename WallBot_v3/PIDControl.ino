// Copyright (c) 2014, Solder Spot
// All rights reserved. 
// See license.txt 

//----------------------------------------
// Config
//----------------------------------------

#define Kp    1000L
#define Ki    1000L
#define Kd    0L

//----------------------------------------
// Data
//----------------------------------------

int16_t _pid_lastErr;
int16_t _pid_sumErrs;

//----------------------------------------
//
//----------------------------------------

void resetPID()
{
  _pid_lastErr = 0;
  _pid_sumErrs = 0;
  adjustLMotor = adjustRMotor = 0;
  updateMotors();
  clear_ticks();
}

//----------------------------------------
//
//----------------------------------------

void driveStraight()
{
  int16_t lticks, rticks, diff;
  int32_t delta;
  uint16_t ms;


  get_ticks_since_last( &lticks, &rticks, &ms);

  if ( ms > 0 )
  {
    // we assume wheels are turning in the same direction
    int16_t dir = ( lticks < 0 || rticks < 0) ? -1 : 1;

    // make the values positive
    lticks *= dir;
    rticks *= dir;

    diff = lticks - rticks;

    // we want the difference to be 0

    if ( !diff)
    {
      // that was easy
      return;
    }

    // track the intergral 
    _pid_sumErrs += diff;

    // get the differential
    delta = (int32_t) (diff - _pid_lastErr);

    int16_t P = (int16_t) ((Kp*((int32_t)diff) + Ki*((int32_t)_pid_sumErrs) + (Kd*delta*1000L)/ms)/1000L);

   _pid_lastErr = diff;

    // a positive error means the left motor is 
    // turning more than the right so adjust 
    // each motor accordingly
    int adjust = (P/2)*dir;
    adjustLMotor = -adjust;
    adjustRMotor = adjust;
    #if PID_INFO
    Serial.print("DIFF = ");
    Serial.print(diff);
    Serial.print(" PID = ");
    Serial.print(P);
    Serial.print(" ADJ = ");
    Serial.println(adjust);
    #endif
    updateMotors();
  }
}

