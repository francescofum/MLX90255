
/*                                                  Entia Embedded Code Challenge
   -----------------------------------------------------------------------------------------------------------------------------------------------------------
   
This is a library for the MLX90255 linear optical array by Melexis.
   This library provides a few of functions for intialising the sensor as well as reading data from it. 
   Sensor Datasheet: https://www.melexis.com/en/documents/documentation/datasheets/datasheet-mlx90255-bc

   Functions provided:
			MLX90255(uint8_t SI, uint8_t CLK, uint8_t AO, float clockspeed =500000);
			Description: Creates an object of the MLX90255 library.
			Arguments:   SI - any digital pin, CLK - any digital pin, AO - any analogue pin, 
				     clockspeed - anything between 1Hz-500kHz defaults to 500kHz.
			
			void init();
			Description: Initialises the sensor, must be called prior to reading any values from the sensor. 
			Arguments:   None

			void readSensor(uint16_t *readings);
			Description: Reads the light intensity values on 128 pixels of the sensor and stores them.
			Arguments:   int *readings - pointer to a 128 element array of type int (16bits or same as ADC resolution).

   			void setSpeed(float clockspeed);
			Description: Allows the user to change the frequency of the clock during runtime.
			Arguments:   float frequency between 1Hz-500kHz

  
   Author: Francesco Fumagalli (francesco.fum@gmail.com)

*/



#include "Arduino.h"
#include "MLX90255.h"


  MLX90255::MLX90255(uint8_t SI, uint8_t CLK, uint8_t AO, float clockspeed) {

      pinMode(SI, OUTPUT);    // Set the two pins as outputs.
      pinMode(CLK, OUTPUT);
      pinMode(AO, INPUT);     // Set ADC pin as input.

      digitalWrite(SI, LOW);  // Logic low to SI and high to CLK
      digitalWrite(CLK, HIGH);// thi is because SI needs to go HIGH beofore CLK goes high for a second time

      _SI = SI;
      _CLK = CLK;
      _AO = AO;

      analogReadResolution(16);  //Teensy can go up to 16, although you may want to ignore the 3LSB due to noise affecting them.

      if (clockspeed > 500000 || clockspeed <= 0){    // Max clockspeed is 500kHz, this is the default clockspeed.
      _clockPeriod = 0.5 / 500000;                    // If the user selects a clock that's not valid (>500kHz or <0Hz).

      }

      else {   // Let user pick his own clockspeed otherwise.
      _clockPeriod = 0.5 / clockspeed;
      }

  }

  void MLX90255::init() {
      /*
         This function should be called at startup. After power-up first integration is probably wrong.
         Call this function to initialise digital levels on the chip.
         This is done by setting SI and 133 clk signals (pg.5 footnote 0 in datasheet)
         After calling this function any subsequent readings should be correct.
      */

      digitalWrite(_CLK, LOW);
      digitalWrite(_SI, HIGH);       // Start signal should be set before clk goes high.
      delayMicroseconds(_clockPeriod);
      digitalWrite(_CLK, HIGH);
      delayMicroseconds(_clockPeriod);

      // Loops 133 times to complete setup.
      for (int i = 0; i < 133; i++) {

        digitalWrite(_CLK, LOW);
        delayMicroseconds(_clockPeriod);
        digitalWrite(_CLK, HIGH);
        delayMicroseconds(_clockPeriod);
      }
 
 }

    void MLX90255::readSensor(uint16_t *readings) {
      /*
          This function reads the 128 pixel bits and stores them in an array.
          First dummy bits and last 2 dummy bits are ignored.
          The SI is set high twice, once to get the values and a second time to read them. 
          Arguments: pointer to an array to store the values.
      */


      digitalWrite(_CLK, LOW);             // Start clock.
      digitalWrite(_SI, HIGH);             // Start signal should be set before clk goes high.
      delayMicroseconds(_clockPeriod);
      digitalWrite(_CLK, HIGH);
      delayMicroseconds(_clockPeriod);
      digitalWrite(_SI, LOW);              // SI must go low before rising edge of next clock pulse.

      digitalWrite(_CLK, LOW);            
      delayMicroseconds(_clockPeriod);
      digitalWrite(_CLK, HIGH);
      delayMicroseconds(_clockPeriod);

      for (int i = 0; i < 128; i++) {
        
        digitalWrite(_CLK, LOW);
        delayMicroseconds(_clockPeriod);
        digitalWrite(_CLK, HIGH);
        delayMicroseconds(_clockPeriod);
        
      }
      
      // Two additional dummy bits which are also discarded.
      digitalWrite(_CLK, LOW);    
      delayMicroseconds(_clockPeriod);
      digitalWrite(_CLK, HIGH);
      delayMicroseconds(_clockPeriod);
      digitalWrite(_CLK, LOW);    
      delayMicroseconds(_clockPeriod);
      digitalWrite(_CLK, HIGH);
      delayMicroseconds(_clockPeriod);

      // 133rd clock pulse to reset the shift register.
      digitalWrite(_CLK, LOW);    
      delayMicroseconds(_clockPeriod);
      digitalWrite(_CLK, HIGH);
      delayMicroseconds(_clockPeriod);
      // Delay a further 10us for effective sample and hold function.
      delayMicroseconds(10);              


      /*The second time this runs it will return the measurements obtained from the previous
       *iteration.If the values aren't read continuously then the integration
       *time might become too large and results might be different to what expected (maxed out due to integration).
       *This is why two reads are always performed, one for the sensor to get the data and one to store the values.
       */

      digitalWrite(_CLK, LOW);
      digitalWrite(_SI, HIGH);      	   // Start signal should be set before clk goes high.
      delayMicroseconds(_clockPeriod);
      digitalWrite(_CLK, HIGH);
      digitalWrite(_SI, LOW);              // SI must go low before rising edge of next clock pulse.
      delayMicroseconds(_clockPeriod);
      // Discard the first value as it's a dummy value.
      digitalWrite(_CLK, LOW);            
      delayMicroseconds(_clockPeriod);
      digitalWrite(_CLK, HIGH);
      delayMicroseconds(_clockPeriod);
      // Discard the second value as it's a dummy value.

      for (int i = 0; i < 128; i++) {         // Read the 128 pixel values.

        digitalWrite(_CLK, LOW);
        delayMicroseconds(_clockPeriod);
        digitalWrite(_CLK, HIGH);
        delayMicroseconds(_clockPeriod);
        *(readings + i) = analogRead(_AO);    // Read the value from analogue out
                                              // and store in array.
      }

      // Two additional dummy bits which are also discarded.
      digitalWrite(_CLK, LOW);    
      delayMicroseconds(_clockPeriod);
      digitalWrite(_CLK, HIGH);
      delayMicroseconds(_clockPeriod);
      digitalWrite(_CLK, LOW);    
      delayMicroseconds(_clockPeriod);
      digitalWrite(_CLK, HIGH);
      delayMicroseconds(_clockPeriod);
      
      // 133rd clock pulse to reset the shift register.
      digitalWrite(_CLK, LOW);                
      delayMicroseconds(_clockPeriod);
      digitalWrite(_CLK, HIGH);
      delayMicroseconds(_clockPeriod);   
}

      void MLX90255::setSpeed(float clockspeed){
  
      if (clockspeed > 500000 || clockspeed <= 0){    // Max clockspeed is 500kHz, this is the default clockspeed.
      _clockPeriod = 0.5 / 500000;                    // If the user selects a clock that's not valid (>500kHz or <0Hz).

      }

      else { // let user pick his own clockspeed otherwise.
      _clockPeriod = 0.5 / clockspeed;
      }
  
}


