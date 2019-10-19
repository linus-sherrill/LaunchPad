#include "CountDownTimer.h"

// ---------------------------------------------------------------
CountDownTimer::CountDownTimer()
{
  // Initialize the display
  m_matrix.begin(0x70);
}


// ---------------------------------------------------------------
void
CountDownTimer::Reset( int secs)
{
  m_currentCount = secs;
  m_targetTime = millis() + 1000; // a second in the future
}

// ---------------------------------------------------------------
void
CountDownTimer::Blank()
{
  m_matrix.println(' ');
  m_matrix.writeDisplay();
}

// ---------------------------------------------------------------
void
CountDownTimer::Cycle()
{
  if (millis() > m_targetTime)
  {
    if ( m_currentCount > 0 )
    {
      --m_currentCount;
      m_targetTime = millis() + 1000; // a second in the future
    }

    DisplayTime();
  }
}

// ---------------------------------------------------------------
int
CountDownTimer::Remaining()
{
  return m_currentCount;
}

// ---------------------------------------------------------------
void
CountDownTimer::DisplayTime()
{
  // Send m_currentCount to display device
  m_matrix.println(m_currentCount);
  m_matrix.writeDisplay();
}
