#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

// --------------------------------------------------------------------------
class CountDownTimer
{
  public:
    CountDownTimer();

    void Reset( int secs);
    void Blank();
    void Cycle();

    int Remaining();

  private:
    void DisplayTime();

    int m_currentCount {0};
    unsigned long m_targetTime;  // in ms

    Adafruit_7segment m_matrix;
};
