/*
   microfire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@microfire.co to get in touch with someone

   Mod-EC hardware version 2, firmware 1
   
   This allows you to run various functions in a command-line like interface.
   Enter:
   `config` to see the configuration of the device.
   'reset' to reset all the configuration stored in the device

   Measure EC:
     ec <solution temp [25]> <temp coefficient [0.019]> <temp constant [25.0]> <probe constant [1.0]>
      `ec` - EC measurement with default parameters
      `ec t` - EC measurement using result of measureTemp() for solution temperature

   Single Point Calibration:
    sin <calibration solution in mS> <solution temp [25]> <temp coefficient [0.019]> <temp constant [25.0]> <probe constant [1.0]>
      `sin 1.413 t` - Calibrate at 1.413 mS using measureTemp() for solution temp

   Dual Point Calibration:
    low <calibration solution in mS> <solution temp [25]> <temp coefficient [0.019]> <temp constant [25.0]> <probe constant [1.0]>
      `low 0.7 t` - Calibrate at 0.7 mS using measureTemp() for solution temp

    high <calibration solution in mS> <solution temp [25]> <temp coefficient [0.019]> <temp constant [25.0]> <probe constant [1.0]>
      `high 12.88 t` - Calibrate at 12.88 mS using measureTemp() for solution temp

   Triple Point Calibration:
    low <calibration solution in mS> <solution temp [25]> <temp coefficient [0.019]> <temp constant [25.0]> <probe constant [1.0]>
      `low 0.1 t` - Calibrate at 0.1 mS using measureTemp() for solution temp
    mid <calibration solution in mS> <solution temp [25]> <temp coefficient [0.019]> <temp constant [25.0]> <probe constant [1.0]>
      `mid 1.0 t` - Calibrate at 1.0 mS using measureTemp() for solution temp
    high <calibration solution in mS> <solution temp [25]> <temp coefficient [0.019]> <temp constant [25.0]> <probe constant [1.0]>
      `high 10.0 t` - Calibrate at 10.0 mS using measureTemp() for solution temp

   Measure Temperature:
    temp

   Change the I2C address:
    i2c 0x0E
*/
#include <Microfire_Mod-EC.h>
#include <OneWire.h>              // Click here to install the library: http://librarymanager/All#OneWire
#include <DallasTemperature.h>    // Click here to install the library: http://librarymanager/All#DallasTemperature

// what pin is the temperature sensor's signal pin connected to?
#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature temp(&oneWire);
Microfire::Mod_EC::i2c ec;

String buffer, cmd, p1, p2, p3, p4, p5;
float temp_c, temp_f, temp_coef, temp_constant, k, density;
const int fw_compatible = 1;
const int hw_compatible = 2;

void config()
{
  ec.getDeviceInfo();
  Serial.println((String) "Microfire Mod-EC Sensor: " + (ec.connected() ? "connected" : "*disconnected*"));
  if (!ec.connected()) return;
  if ((ec.fwVersion != fw_compatible) || (ec.hwVersion != hw_compatible))
  {
    Serial.println("*This version of shell was designed for a different hardware revision or firmware version*");
  }
  Serial.println("calibration:");
  Serial.print("    low reference / read: ");
  if (ec.calibrationLowReference == ec.calibrationLowReference ? Serial.print(ec.calibrationLowReference, 3) : Serial.print("-"));
  Serial.print(" /  ");
  if (ec.calibrationLowReading == ec.calibrationLowReading ? Serial.println(ec.calibrationLowReading, 3) : Serial.println("-"));
  Serial.print("    middle reference / read: ");
  if (ec.calibrationMidReference == ec.calibrationMidReference ? Serial.print(ec.calibrationMidReference, 3) : Serial.print("-"));
  Serial.print(" / ");
  if (ec.calibrationMidReading == ec.calibrationMidReading ? Serial.println(ec.calibrationMidReading, 3) : Serial.println("-"));
  Serial.print("    high reference / read: ");
  if (ec.calibrationHighReference == ec.calibrationHighReference ? Serial.print(ec.calibrationHighReference, 3) : Serial.print("-"));
  Serial.print(" / ");
  if (ec.calibrationHighReading == ec.calibrationHighReading ? Serial.println(ec.calibrationHighReading, 3) : Serial.println("-"));
  Serial.print("    single point: ");
  if (ec.calibrationSingleOffset == ec.calibrationSingleOffset ? Serial.println(ec.calibrationSingleOffset, 3) : Serial.println("-"));
  Serial.print("hardware:firmware version: ");
  Serial.print(ec.hwVersion);
  Serial.print(":");
  Serial.println(ec.fwVersion);
}

void config_reset()
{
  ec.reset();
  config();
}

void temperature()
{
  temp.requestTemperatures();
  temp_c = temp.getTempCByIndex(0);
  temp_f = ((temp_c * 9) / 5) + 32;

  Serial.print("C|F: ");
  Serial.print(temp_c);
  Serial.print(" | ");
  Serial.println(temp_f);

}

void i2c()
{
  uint8_t i2c_address;
  char * p;

  if (p1.length())
  {
    i2c_address = strtoul(p1.c_str(), & p, 16);
    Serial.println(i2c_address);
    if (i2c_address == 0)
    {
      Serial.println("Error: malformed I2C address");
    }
    else if ((i2c_address <= 0x07) || (i2c_address > 0x7f))
    {
      Serial.println("Error: I2C address not in valid range");
    }
    else
    {
      ec.setI2CAddress(i2c_address);
    }
  }
}

void measure_ec()
{
  if (p1.length() ? temp_c = p1.toFloat() : temp_c = 25.0);
  if (p2.length() ? temp_coef = p2.toFloat() : temp_coef = 0.019);
  if (p3.length() ? temp_constant = p3.toFloat() : temp_constant = 25.0);
  if (p4.length() ? k = p4.toFloat() : k = 1.0);

  while (Serial.available() == 0)
  {
    if (p1 == "t")
    {
      temp.requestTemperatures();
      temp_c = temp.getTempCByIndex(0);
    }
    ec.measureEC(temp_c, temp_coef, temp_constant, k, 0, true);

    switch (ec.status)
    {
      case ec.STATUS_NO_PROBE_RANGE:
        Serial.println("Error: No probe or solution out of range");
        break;
      case ec.STATUS_SYSTEM_ERROR:
        Serial.println("Error: Module not functioning properly");
        break;
      case ec.STATUS_CONFIG_ERROR:
        Serial.println("Error: Check parameters or calibration");
        break;
      case ec.STATUS_NO_ERROR:
        Serial.print(ec.mS);
        Serial.println((String)" mS/cm @ " + temp_c + "°C");
        break;
    }
    delay(1000);
  }
}

void low()
{
  if (p2.length() ? temp_c = p2.toFloat() : temp_c = 25.0);
  if (p3.length() ? temp_coef = p3.toFloat() : temp_coef = 0.019);
  if (p4.length() ? temp_constant = p4.toFloat() : temp_constant = 25.0);
  if (p5.length() ? k = p5.toFloat() : k = 1.0);

  if (p2 == "t")
  {
    temp.requestTemperatures();
    temp_c = temp.getTempCByIndex(0);
  }
  ec.calibrateLow(p1.toFloat(), temp_c, temp_coef, temp_constant, k, true);
  switch (ec.status)
  {
    case ec.STATUS_NO_PROBE_RANGE:
      Serial.println("Error: No probe or solution out of range");
      break;
    case ec.STATUS_SYSTEM_ERROR:
      Serial.println("Error: Module not functioning properly");
      break;
    case ec.STATUS_CONFIG_ERROR:
      Serial.println("Error: Check parameters or calibration");
      break;
    case ec.STATUS_NO_ERROR:
      config();
      break;
  }
}

void mid()
{
  if (p2.length() ? temp_c = p2.toFloat() : temp_c = 25.0);
  if (p3.length() ? temp_coef = p3.toFloat() : temp_coef = 0.019);
  if (p4.length() ? temp_constant = p4.toFloat() : temp_constant = 25.0);
  if (p5.length() ? k = p5.toFloat() : k = 1.0);

  if (p2 == "t")
  {
    temp.requestTemperatures();
    temp_c = temp.getTempCByIndex(0);
  }
  ec.calibrateMid(p1.toFloat(), temp_c, temp_coef, temp_constant, k, true);
  switch (ec.status)
  {
    case ec.STATUS_NO_PROBE_RANGE:
      Serial.println("Error: No probe or solution out of range");
      break;
    case ec.STATUS_SYSTEM_ERROR:
      Serial.println("Error: Module not functioning properly");
      break;
    case ec.STATUS_CONFIG_ERROR:
      Serial.println("Error: Check parameters or calibration");
      break;
    case ec.STATUS_NO_ERROR:
      config();
      break;
  }
}

void high()
{
  if (p2.length() ? temp_c = p2.toFloat() : temp_c = 25.0);
  if (p3.length() ? temp_coef = p3.toFloat() : temp_coef = 0.019);
  if (p4.length() ? temp_constant = p4.toFloat() : temp_constant = 25.0);
  if (p5.length() ? k = p5.toFloat() : k = 1.0);

  if (p2 == "t")
  {
    temp.requestTemperatures();
    temp_c = temp.getTempCByIndex(0);
  }
  ec.calibrateHigh(p1.toFloat(), temp_c, temp_coef, temp_constant, k, true);
  switch (ec.status)
  {
    case ec.STATUS_NO_PROBE_RANGE:
      Serial.println("Error: No probe or solution out of range");
      break;
    case ec.STATUS_SYSTEM_ERROR:
      Serial.println("Error: Module not functioning properly");
      break;
    case ec.STATUS_CONFIG_ERROR:
      Serial.println("Error: Check parameters or calibration");
      break;
    case ec.STATUS_NO_ERROR:
      config();
      break;
  }
}

void sin()
{
  if (p2.length() ? temp_c = p2.toFloat() : temp_c = 25.0);
  if (p3.length() ? temp_coef = p3.toFloat() : temp_coef = 0.019);
  if (p4.length() ? temp_constant = p4.toFloat() : temp_constant = 25.0);
  if (p5.length() ? k = p5.toFloat() : k = 1.0);

  if (p2 == "t")
  {
    temp.requestTemperatures();
    temp_c = temp.getTempCByIndex(0);
  }
  ec.calibrateSingle(p1.toFloat(), temp_c, temp_coef, temp_constant, k, true);
  switch (ec.status)
  {
    case ec.STATUS_NO_PROBE_RANGE:
      Serial.println("Error: No probe or solution out of rangeect");
      break;
    case ec.STATUS_SYSTEM_ERROR:
      Serial.println("Error: Module not functioning properly");
      break;
    case ec.STATUS_CONFIG_ERROR:
      Serial.println("Error: Check parameters or calibration");
      break;
    case ec.STATUS_NO_ERROR:
      config();
      break;
  }
}

void help()
{
  Serial.println(F("List of available commands, parameters separated by spaces, `sin 1.413 22.1`, bracketed numbers are defaults if none provided"));
  Serial.println(F("config -or- c : no parameters : Displays all configuration and system information."));
  Serial.println(F("ec            : temp_C[25.0], temp_Coef[0.019], temp_Constant[25.0], K[1.0] : Starts an EC measurement."));
  Serial.println(F("high          : calibration_mS, temp_C[25.0], temp_Coef[0.019], temp_Constant[25.0], K[1.0] : High-point calibration."));
  Serial.println(F("i2c           : I2C_address : Changes the module's I2C address."));
  Serial.println(F("low           : calibration_mS, temp_C[25.0], temp_Coef[0.019], temp_Constant[25.0], K[1.0] : Low-point calibration."));
  Serial.println(F("mid           : calibration_mS, temp_C[25.0], temp_Coef[0.019], temp_Constant[25.0], K[1.0] : Mid-point calibration."));
  Serial.println(F("reset -or- r  : no parameters : Returns all configuration information to default values."));
  Serial.println(F("sin           : calibration_mS, temp_C[25.0], temp_Coef[0.019], temp_Constant[25.0], K[1.0] : Single-point calibration."));
  Serial.println(F("temp -or- t   : no parameters : Starts a temperature measurement"));

}
void cmd_run()
{
  if ((cmd == "config") || (cmd == "c")) config();
  if ((cmd == "reset") || (cmd == "r")) config_reset();
  if ((cmd == "temp") || (cmd == "t")) temperature();
  if (cmd == "low") low();
  if (cmd == "mid") mid();
  if (cmd == "high") high();
  if (cmd == "i2c") i2c();
  if (cmd == "ec") measure_ec();
  if (cmd == "sin") sin();
  if ((cmd == "help") || (cmd == "h")) help();
}

void cli_process()
{
  while (Serial.available())
  {
    char c = Serial.read();
    switch (c)
    {
      case '\n':
        Serial.println();
        cmd = buffer.substring(0, buffer.indexOf(" ", 0));
        cmd.trim();
        buffer.remove(0, buffer.indexOf(" ", 0));
        buffer.trim();
        p1 = buffer.substring(0, buffer.indexOf(" ", 0));

        buffer.remove(0, buffer.indexOf(" ", 0));
        buffer.trim();
        p2 = buffer.substring(0, buffer.indexOf(" ", 0));

        buffer.remove(0, buffer.indexOf(" ", 0));
        buffer.trim();
        p3 = buffer.substring(0, buffer.indexOf(" ", 0));

        buffer.remove(0, buffer.indexOf(" ", 0));
        buffer.trim();
        p4 = buffer.substring(0, buffer.indexOf(" ", 0));

        buffer.remove(0, buffer.indexOf(" ", 0));
        buffer.trim();
        p5 = buffer.substring(0, buffer.indexOf(" ", 0));

        cmd_run();

        Serial.print("> ");
        buffer = "";
        break;

      case '\b': // backspace
        buffer.remove(buffer.length() - 1);
        Serial.print("\b \b");
        break;

      default: // everything else
        buffer += c;
        Serial.print(c);
    }
  }
}

void setup()
{
  Wire.begin();
  ec.begin();
  temp.begin();
  Serial.begin(9600);
  Serial.println("Type `help` for a list of commands");
  config();
  Serial.print("> ");
}

void loop()
{
  cli_process();
}