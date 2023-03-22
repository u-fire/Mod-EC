/*!
   microfire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@microfire.co to get in touch with someone

   Mod-EC hardware version 2, firmware 1
*/
#include <Microfire_Mod-EC.h>
Microfire::Mod_EC::i2c ec;

void setup()
{
  // start Serial and I2C
  Serial.begin(9600);
  Wire.begin();

  // start the sensor
  ec.begin();
}

void loop()
{
// take an EC measurement
  ec.measureEC();

  // display the results
  Serial.println((String) ec.mS + " mS/cm");

  delay(1000);
}