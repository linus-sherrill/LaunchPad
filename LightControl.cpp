
#include "Arduino.h"

#include "Ports.h"
#include "LightControl.h"

// -------------------------------------------------------------
LightControl::
LightControl() // CTOR
{
  Dark();
  currentState = SIGNAL_OFF;
}

// -------------------------------------------------------------

void LightControl::
Green()
{
  Dark();
  currentPort = kGreenLightPort;
  currentState = SIGNAL_ON;
  Cycle(); // turn light on
}

// -------------------------------------------------------------
void LightControl::
GreenFlash( unsigned int period ) // period in ms
{
  Dark();
  halfCycle = period / 2;
  targetTime = millis() + halfCycle;
  currentPort = kGreenLightPort;
  currentState = SIGNAL_ON;
  Cycle(); // turn light on
}

// -------------------------------------------------------------
void LightControl::
Yellow()
{
  Dark();
  currentPort = kYellowLightPort;
  currentState = SIGNAL_ON;
  Cycle(); // turn light on
}

// -------------------------------------------------------------
void LightControl::
YellowFlash( unsigned int period ) // period in ms
{
  Dark();
  halfCycle = period / 2;
  targetTime = millis() + halfCycle;
  currentPort = kYellowLightPort;
  currentState = SIGNAL_ON;
  Cycle(); // turn light on
}

// -------------------------------------------------------------
void LightControl::
Red()
{
  Dark();
  currentPort = kRedLightPort;
  currentState = SIGNAL_ON;
  Cycle(); // turn light on
}
// -------------------------------------------------------------
void LightControl::
RedFlash( unsigned int period ) // period in ms
{
  Dark();
  halfCycle = period / 2;
  targetTime = millis() + halfCycle;
  currentPort = kRedLightPort;
  currentState = SIGNAL_ON;
  Cycle(); // turn light on
}

// -------------------------------------------------------------
void LightControl::
Dark()
{
  targetTime = 0;
  currentState = false;
  digitalWrite( kGreenLightPort, SIGNAL_OFF );
  digitalWrite( kYellowLightPort, SIGNAL_OFF );
  digitalWrite( kRedLightPort, SIGNAL_OFF );
}

// -------------------------------------------------------------
void LightControl::
Cycle()
{
  digitalWrite(currentPort, currentState);

  // If we have exceeded our half interval, invert the
  // state of the light
  if ((targetTime > 0) && (millis() > targetTime))
  {
    targetTime = millis() + halfCycle;
    if (currentState == SIGNAL_OFF)
    {
      currentState = SIGNAL_ON;
    }
    else
    {
      currentState = SIGNAL_OFF;
    }

    digitalWrite(currentPort, currentState);
  }
}
