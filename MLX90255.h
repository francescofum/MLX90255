
#ifndef MLX90255_h
#define MLX90255_h

#include "Arduino.h"	


class MLX90255 {

  public:
    MLX90255(uint8_t SI, uint8_t CLK, uint8_t AO, float clockspeed = 500000);
    void init();
    void readSensor(uint16_t *readings);
    void setSpeed(float clockspeed);

  private:
    uint8_t _SI;
    uint8_t _CLK;
    uint8_t _AO;  
    uint8_t _clockPeriod;
};
#endif
