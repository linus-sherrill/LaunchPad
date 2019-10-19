#include "Arduino.h"

// Port assignment

constexpr int RelayBase{ 5 }; // D5

constexpr int kGreenLightPort{ RelayBase + 5 };
constexpr int kYellowLightPort{ RelayBase + 6 } ;
constexpr int kRedLightPort{ RelayBase + 7 };

constexpr int kLaunchButtonLightPort{ RelayBase + 4 };
constexpr int kBlowDownPort{ RelayBase + 3 };

constexpr int kFuelPort{ RelayBase + 2 }; // black
constexpr int kLaunchPort{ RelayBase + 1 }; // brown

// inputs - active low
#define kKeyPort 2
#define kLaunchButton 3
#define kEstopPort 4

#define SIGNAL_OFF HIGH
#define SIGNAL_ON  LOW
