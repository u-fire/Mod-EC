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

  // start the sensor after Wire.begin()
  ec.begin();
}

void loop()
{
  // get the temperature of the solution
  ec.measureTemp();

  // take an EC measurement
  ec.measureEC(ec.tempC);

  // display the results
  Serial.println((String) ec.mS + " mS/cm");
  Serial.println((String) ec.tempC + " °C");
  Serial.println();

  delay(1000);
}