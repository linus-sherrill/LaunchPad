#include "Arduino.h"

#include "Ports.h"
#include "LightControl.h"
#include "CountDownTimer.h"

// -----------------------------------------------------------------------------
//
// Debugging macros
//
#define NewState( S ) \
  currentState = S; \
  Serial.println( "New sate:" #S );

#define SignalState( N, S) \
  Serial.print( N ); \
  Serial.print( ": " ); \
  Serial.println( (S) ? "HIGH" : "LOW"  );

// converts seconds to timer coutns MS
#define TIME_VAL(N) ((N)*1000)

#define ARM_DWELL 4000
#define FUELING_TIME 11000

#define FIRE_TIME 2100  // retracting cylinder
#define FIRE_RECYCLE 4500

// --------------------------------------------------------------------------
void RelayTest()
{
  constexpr int i { 1 };

  do {
    digitalWrite( RelayBase + i, SIGNAL_ON );
    delay(1000);
    digitalWrite( RelayBase + i, SIGNAL_OFF );
    delay(1000);
  } while (1);
}

// ----------------------------------------------------------------------------
inline bool KeyActive()
{
  int key_state = digitalRead( kKeyPort );
  // SignalState( "Key state", key_state  );

  return ( key_state == 0 ); // Active low
}

// ----------------------------------------------------------------------------
inline bool LaunchButtonActive()
{
  int button_state = digitalRead( kLaunchButton );
  // SignalState( "Launch button", button_state );

  return ( button_state == 0 ); // active low
}

// ----------------------------------------------------------------------------
inline bool E_StopButtonActive()
{
  int button_state = digitalRead( kEstopPort );
  // SignalState( "E_Stop button", button_state );

  // Debounce
  if ( button_state == 0 )
  {
    delay( 100 );
    button_state = digitalRead( kEstopPort );
  }

  return ( button_state == 0 ); // active low
}

// --------------------------------------------------------------------
void checkout()
{
check_out:
  Serial.println( "Green Light" );
  do {
    digitalWrite( kGreenLightPort, SIGNAL_ON );
    delay(1000);
    digitalWrite( kGreenLightPort, SIGNAL_OFF );
    delay(1000);
  } while (0);

  Serial.println( "Yellow Light" );
  do {
    digitalWrite( kYellowLightPort, SIGNAL_ON );
    delay(1000);
    digitalWrite( kYellowLightPort, SIGNAL_OFF );
    delay(1000);
  } while (0);

  Serial.println( "Red Light" );
  do {
    digitalWrite( kRedLightPort, SIGNAL_ON );
    delay(1000);
    digitalWrite( kRedLightPort, SIGNAL_OFF );
    delay(1000);
  } while (0);

  Serial.println( "Launch Button Light" );
  do {
    digitalWrite( kLaunchButtonLightPort, SIGNAL_ON );
    delay(1000);
    digitalWrite( kLaunchButtonLightPort, SIGNAL_OFF );
    delay(1000);
  } while (0);

  Serial.println( "Fuel Actuator" );
  do {
    digitalWrite(kFuelPort, SIGNAL_ON );
    delay(1000);
    digitalWrite(kFuelPort, SIGNAL_OFF );
    delay(1000);
  } while (0);

  Serial.println( "Launch Actuator" );
  do {
    digitalWrite(kLaunchPort, SIGNAL_ON );
    delay(1000);
    digitalWrite(kLaunchPort, SIGNAL_OFF  );
    delay(1000);
  } while (0);

  Serial.println( "Blow Down Valve" );
  do {
    digitalWrite(kBlowDownPort, SIGNAL_ON );
    delay(1000);
    digitalWrite(kBlowDownPort, SIGNAL_OFF  );
    delay(1000);
  } while (0);
  // goto check_out;

  Serial.println("turn key\n");
  while (1)
  {
    if (KeyActive())
    {
      Serial.println("Key turn detected");
      break;
    }
  } // end while

  Serial.println("Press fire button\n");
  while (1)
  {
    if (LaunchButtonActive())
    {
      Serial.println("Fire button detected");
      break;
    }
  } // end while

  Serial.println("Press E-Stop button\n");
  while (1)
  {
    if (E_StopButtonActive())
    {
      Serial.println("E-Stop button detected");
      break;
    }
  } // end while
} // end checkout

// -------------------------------------------------------------------------
void LightShow()
{
  digitalWrite( kGreenLightPort, SIGNAL_ON );
  delay(1000);
  digitalWrite( kYellowLightPort, SIGNAL_ON );
  delay(1000);
  digitalWrite( kGreenLightPort, SIGNAL_OFF );
  delay(1000);
  digitalWrite( kRedLightPort, SIGNAL_ON );
  delay(1000);
  digitalWrite( kYellowLightPort, SIGNAL_OFF );
  delay(1000);
  digitalWrite( kRedLightPort, SIGNAL_OFF );
  delay( 1000 );
}

// ============================================================================
void setup()
{
  Serial.begin(19200);
  Serial.println( "LauchPad control program: V1.1 - " __DATE__ " - " __TIME__ "\n");

  // put your setup code here, to run once:
  for (int i = 0; i < 8; ++i )
  {
    // Reset all relays
    pinMode( RelayBase + i, OUTPUT);
    digitalWrite( RelayBase + i, SIGNAL_OFF );
  }

  // input pins
  pinMode( kKeyPort, INPUT_PULLUP );
  pinMode( kLaunchButton, INPUT_PULLUP );
  pinMode( kEstopPort, INPUT_PULLUP);

  // This would be the power on self text light show
  LightShow();

  // initial checkout testing
  // checkout();
  // RelayTest();
}


// --------------------------------------------------------------------------
enum { // States
  ST_IDLE,
  ST_KEY_ACTIVE,
  ST_KEY_SATISFIED,
  ST_READY,
  ST_FUELING,
  ST_FUELING_ACTIVE,
  ST_FIRE,
  ST_FIRING,
  ST_POST_FIRING
};

LightControl LightTower;
CountDownTimer CDTimer;

// Global state variables
int currentState { ST_IDLE };
unsigned long key_timer { 0 };
unsigned long fueling_timer { 0 };
unsigned long firing_timer { 0 };

// ============================================================================
void loop()
{
  LightTower.Cycle(); // update light state
  CDTimer.Cycle(); // update display

  // Test E-Stop button. If pressed, reset the state machine.
  if ( E_StopButtonActive() )
  {
    Serial.println( "E-Stop detected");
    NewState( ST_IDLE );

    // Reset major outputs
    digitalWrite( kFuelPort, SIGNAL_OFF );
    digitalWrite( kLaunchPort, SIGNAL_OFF );
    digitalWrite( kBlowDownPort, SIGNAL_ON );
    digitalWrite( kLaunchButtonLightPort, SIGNAL_OFF );

    // Reset light tower
    LightTower.Yellow();
  }
  else
  {
    digitalWrite( kBlowDownPort, SIGNAL_OFF );
  }

  switch ( currentState )
  {
    default:
    case ST_IDLE: // -------------------------------------------
      {
        LightTower.Green();

        // if key switch is active, go to ST_KEY_ACTIVE
        if ( KeyActive() )
        {
          NewState( ST_KEY_ACTIVE );
          key_timer = millis() + ARM_DWELL; // set timer for 4 seconds
        }
        break;
      }

    case ST_KEY_ACTIVE: // -------------------------------------------
      {
        if ( ! KeyActive() ) // Key released
        {
          // key switch has been reset
          NewState( ST_IDLE );
        }

        if ( millis() > key_timer )
        {
          // Key has been held long enough
          NewState( ST_KEY_SATISFIED );

          // Set yellow flashing light
          LightTower.YellowFlash();

          // turn on fire button light
          digitalWrite( kLaunchButtonLightPort, SIGNAL_ON );
        }
      }
      break;

    case ST_KEY_SATISFIED: // -------------------------------------------
      {
        // Wait for key to be released. When released, advance state.
        if ( ! KeyActive() )
        {
          // key switch has been reset, advance to READY
          NewState( ST_READY );
        }
      }
      break;

    case ST_READY: // -------------------------------------------
      {
        // Wait for button to be pushed.
        if ( LaunchButtonActive() )
        {
          // This is trying to debounce the key.
          // If the key is still down after 50 ms,
          // then consider it a real button press.
          delay( 50 );
          if ( ! LaunchButtonActive() )
          {
            currentState = ST_FUELING;

            // Set yellow light
            LightTower.Yellow();

            // Start countdown timer
            // This needs to be greater than the total delay through all subsequest
            // states until pulling the trigger.
            CDTimer.Reset(18);
          }
        }
      }
      break;

    case ST_FUELING: // -------------------------------------------
      {
        // Turn on fueling relay
        digitalWrite(kFuelPort, SIGNAL_ON );

        // turn off fire button light
        digitalWrite( kLaunchButtonLightPort, SIGNAL_OFF );

        fueling_timer = millis() + FUELING_TIME; // add fuel for N sec
        NewState( ST_FUELING_ACTIVE );
      }
      break;

    case ST_FUELING_ACTIVE: // -------------------------------------------
      {
        // Waiting for fueling timer to expire
        if (millis() > fueling_timer )
        {
          // Turn fueling relay off
          digitalWrite(kFuelPort, SIGNAL_OFF );
          NewState( ST_FIRE );
          LightTower.Red();
          
          // Add delay from when air is turned off and trigger in pulled
          fueling_timer = millis() + 2000; // temp
        }
      }
      break;

    case ST_FIRE: // -------------------------------------------
      {
        // Wait until countdown reaches zero.
        // if ( CDTimer.Remaining() == 0 )
        if (millis() > fueling_timer ) // temp
        {
          digitalWrite( kLaunchPort, SIGNAL_ON );
          NewState( ST_FIRING );
          firing_timer = millis() + FIRE_TIME; // time to pull latch
        }
      }
      break;

    case ST_FIRING: // -------------------------------------------
      {
        // Delay a while until the trigger is pulled
        if ( millis() > firing_timer )
        {
          // Reset actuator
          digitalWrite( kLaunchPort, SIGNAL_OFF );
          NewState( ST_POST_FIRING );
          firing_timer = millis() + FIRE_RECYCLE; // reset time
        }
      }
      break;

    case ST_POST_FIRING: // -------------------------------------------
      {
        if ( millis() > firing_timer )
        {
          NewState( ST_IDLE );
          CDTimer.Blank();
        }
      }
      break;

  } // end switch

} // end loop
