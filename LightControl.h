

// -----------------------------------------------------------------
class LightControl
{
public:
  LightControl();

  void Green();
  void GreenFlash( unsigned int period = 1000 ); // period in ms
  void Yellow();
  void YellowFlash( unsigned int period = 1000 ); // period in ms
  void Red();
  void RedFlash( unsigned int period = 1000 ); // period in ms
  
  void Dark(); // turn all lights off
  void Cycle(); // periodic call to implement flashing

private:
  int currentPort;
  bool currentState;
  unsigned long halfCycle; // in ms
  unsigned long targetTime;  // in ms
};
