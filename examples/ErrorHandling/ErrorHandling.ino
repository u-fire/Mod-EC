/*!
   ufire.dev for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   Mod-EC hardware version 1, firmware 1
*/

#include <uFire_Mod-EC.h>
uFire::Mod_EC::i2c ec;

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  if (ec.begin() != true)
  {
    Serial.println("Error: the sensor isn't connected");
    while (1);
  }
}

void loop()
{
  // get the temperature of the solution
  ec.measureTemp();

  // check for errors
  if (ec.status)
  {
    Serial.println("Error:");
    switch (ec.status)
    {
      case ec.STATUS_NO_PROBE_RANGE:
        Serial.println("  temperature sensor not connected");
        break;

      case ec.STATUS_SYSTEM_ERROR:
        Serial.println("  temperature sensor not connected or damaged");
        break;
    }
  }

  // take an EC measurement
  ec.measureEC(ec.tempC);
  switch (ec.status)
  {
    case ec.STATUS_NO_PROBE_RANGE:
      Serial.println("  probe not connected or solution out of range");
      break;

    case ec.STATUS_SYSTEM_ERROR:
      Serial.println("  module not functioning properly");
      break;

    case ec.STATUS_CONFIG_ERROR:
      Serial.println("  parameters to measureEC, or calibration data likely incorrect");
      break;

    case ec.STATUS_NO_ERROR:
      // display the results if no error
      Serial.println((String) ec.mS + " mS/cm");
      Serial.println((String) ec.tempC + " °C");
      Serial.println();
      break;
  }
}