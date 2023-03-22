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

  // start the sensor and check for error
  if (ec.begin() != true)
  {
    Serial.println("Error: the sensor isn't connected");
    while (1);
  }
}

void loop()
{
  // take an EC measurement
  ec.measureEC();
  switch (ec.status)
  {
    case ec.STATUS_NO_PROBE_RANGE:
      Serial.println("Error: probe not connected or solution out of range");
      break;

    case ec.STATUS_SYSTEM_ERROR:
      Serial.println("Error: module not functioning properly");
      break;

    case ec.STATUS_CONFIG_ERROR:
      Serial.println("Error: parameters to measureEC, or calibration data incorrect");
      break;

    case ec.STATUS_NO_ERROR:
      // display the results if no error
      Serial.println((String) ec.mS + " mS/cm");
      break;
  }
}