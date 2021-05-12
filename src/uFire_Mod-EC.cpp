#include "uFire_Mod-EC.h"

float uFire::Mod_EC::i2c::__S = -1;
float uFire::Mod_EC::i2c::_mS = -1;
float uFire::Mod_EC::i2c::_uS = -1;
float uFire::Mod_EC::i2c::_PSU = -1;
float uFire::Mod_EC::i2c::_tempC = -1;
float uFire::Mod_EC::i2c::_tempF = -1;
int uFire::Mod_EC::i2c::_PPM_500 = -1;
int uFire::Mod_EC::i2c::_PPM_640 = -1;
int uFire::Mod_EC::i2c::_PPM_700 = -1;
float uFire::Mod_EC::i2c::_calibrationLowReading = -1;
float uFire::Mod_EC::i2c::_calibrationLowReference = -1;
float uFire::Mod_EC::i2c::_calibrationMidReading = -1;
float uFire::Mod_EC::i2c::_calibrationMidReference = -1;
float uFire::Mod_EC::i2c::_calibrationHighReading = -1;
float uFire::Mod_EC::i2c::_calibrationHighReference = -1;
float uFire::Mod_EC::i2c::_calibrationSingleOffset = -1;
int uFire::Mod_EC::i2c::_hwVersion = -1;
int uFire::Mod_EC::i2c::_fwVersion = -1;
int uFire::Mod_EC::i2c::_i2c_address = -1;
int uFire::Mod_EC::i2c::_status = -1;
float uFire::Mod_EC::i2c::_density = -1;

namespace uFire
{
  namespace Mod_EC
  {
    // Initializes library
    bool i2c::begin(TwoWire &wirePort, uint8_t address)
    {
      _address = address;
      _i2cPort = &wirePort;

      return connected();
    }

    // Performs a high-point calibration. Used in conjuction with calibrateMid or calibrateLow.
    uint8_t i2c::calibrateHigh(float solutionEC, float tempC, float tempCoef, float tempConst, float k, bool blocking)
    {
      _write_4_bytes(MS_REGISTER, solutionEC);
      _write_4_bytes(TEMP_C_REGISTER, tempC);
      _write_4_bytes(CONSTANT_REGISTER, tempConst);
      _write_4_bytes(COEFFICIENT_REGISTER, tempCoef);
      _write_4_bytes(K_REGISTER, k);

      _send_command(CALIBRATE_HIGH_TASK);
      if (blocking)
        delay(EC_EC_MEASUREMENT_TIME);

      _updateRegisters();
      return status;
    }

    // Performs a low-point calibration. Used in conjuction with calibrateMid or calibrateHigh.
    uint8_t i2c::calibrateLow(float solutionEC, float tempC, float tempCoef, float tempConst, float k, bool blocking)
    {
      _write_4_bytes(MS_REGISTER, solutionEC);
      _write_4_bytes(TEMP_C_REGISTER, tempC);
      _write_4_bytes(CONSTANT_REGISTER, tempConst);
      _write_4_bytes(COEFFICIENT_REGISTER, tempCoef);
      _write_4_bytes(K_REGISTER, k);

      _send_command(CALIBRATE_LOW_TASK);
      if (blocking)
        delay(EC_EC_MEASUREMENT_TIME);

      _updateRegisters();
      return status;
    }

    // Performs a mid-point calibration. Used in conjuction with calibrateLow or calibrateHigh.
    uint8_t i2c::calibrateMid(float solutionEC, float tempC, float tempCoef, float tempConst, float k, bool blocking)
    {
      _write_4_bytes(MS_REGISTER, solutionEC);
      _write_4_bytes(TEMP_C_REGISTER, tempC);
      _write_4_bytes(CONSTANT_REGISTER, tempConst);
      _write_4_bytes(COEFFICIENT_REGISTER, tempCoef);
      _write_4_bytes(K_REGISTER, k);

      _send_command(CALIBRATE_MID_TASK);
      if (blocking)
        delay(EC_EC_MEASUREMENT_TIME);

      _updateRegisters();
      return status;
    }

    // Performs a single-point calibration.
    uint8_t i2c::calibrateSingle(float solutionEC, float tempC, float tempCoef, float tempConst, float k, bool blocking)
    {
      _write_4_bytes(MS_REGISTER, solutionEC);
      _write_4_bytes(TEMP_C_REGISTER, tempC);
      _write_4_bytes(CONSTANT_REGISTER, tempConst);
      _write_4_bytes(COEFFICIENT_REGISTER, tempCoef);
      _write_4_bytes(K_REGISTER, k);

      _send_command(CALIBRATE_SINGLE_TASK);
      if (blocking)
        delay(EC_EC_MEASUREMENT_TIME);

      getDeviceInfo();
      return status;
    }

    // Returns true or false if the sensor is connected.
    bool i2c::connected()
    {
      _i2cPort->beginTransmission(_address);
      uint8_t retval = _i2cPort->endTransmission();

      if (retval)
      {
        return false;
      }
      else
      {
        return true;
      }
    }

    // Retrieves all the system information.
    void i2c::getDeviceInfo()
    {
      _calibrationLowReading = _read_4_bytes(CALIBRATE_READLOW_REGISTER);
      _calibrationLowReference = _read_4_bytes(CALIBRATE_REFLOW_REGISTER);
      _calibrationMidReading = _read_4_bytes(CALIBRATE_READMID_REGISTER);
      _calibrationMidReference = _read_4_bytes(CALIBRATE_REFMID_REGISTER);
      _calibrationHighReading = _read_4_bytes(CALIBRATE_READHIGH_REGISTER);
      _calibrationHighReference = _read_4_bytes(CALIBRATE_REFHIGH_REGISTER);
      _calibrationSingleOffset = _read_4_bytes(CALIBRATE_SINGLE_OFFSET_REGISTER);
      _hwVersion = _read_byte(HW_VERSION_REGISTER);
      _fwVersion = _read_byte(FW_VERSION_REGISTER);
      _status = _read_byte(STATUS_REGISTER);
    }

    // Measures EC.
    float i2c::measureEC(float tempC, float tempCoef, float tempConst, float k, float kPa, bool blocking)
    {
      _write_4_bytes(TEMP_C_REGISTER, tempC);
      _write_4_bytes(CONSTANT_REGISTER, tempConst);
      _write_4_bytes(COEFFICIENT_REGISTER, tempCoef);
      _write_4_bytes(K_REGISTER, k);
      _write_4_bytes(KPA_REGISTER, kPa);

      _send_command(MEASURE_EC_TASK);
      if (blocking)
        delay(EC_EC_MEASUREMENT_TIME);

      _updateRegisters();

      return mS;
    }

    // Default parameters to measure seawater.
    float i2c::measureSeawater(float tempC, float tempCoef, float tempConst, float k, float kPa, bool blocking)
    {
      measureEC(tempC, tempCoef, tempConst, k, kPa, true);
      return PSU;
    }

    // Default parameters to measure freshwater.
    float i2c::measureFreshwater(float tempC, float tempCoef, float tempConst, float k, bool blocking)
    {
      return measureEC(tempC, tempCoef, tempConst, k, 0, true);
    }

    // Default parameters to measure pure water.
    float i2c::measurePurewater(float tempC, float tempCoef, float tempConst, float k, bool blocking)
    {
      measureEC(tempC, tempCoef, tempConst, k, 0, true);
      return uS;
    }

    // Measures temperature using an optionally connected DS18B20 sensor
    float i2c::measureTemp(bool blocking)
    {
      _send_command(MEASURE_TEMP_TASK);
      if (blocking)
        delay(EC_TEMP_MEASURE_TIME);

      _updateRegisters();

      return tempC;
    }

    // Resets all system calibration information.
    void i2c::reset()
    {
      _write_4_bytes(CALIBRATE_REFHIGH_REGISTER, NAN);
      _write_4_bytes(CALIBRATE_REFLOW_REGISTER, NAN);
      _write_4_bytes(CALIBRATE_READHIGH_REGISTER, NAN);
      _write_4_bytes(CALIBRATE_READLOW_REGISTER, NAN);
      _write_4_bytes(CALIBRATE_REFMID_REGISTER, NAN);
      _write_4_bytes(CALIBRATE_READMID_REGISTER, NAN);
      _write_4_bytes(CALIBRATE_SINGLE_OFFSET_REGISTER, NAN);
    }

    // Writes system calibration information.
    void i2c::setDeviceInfo(float calibrationLowReading, float calibrationLowReference, float calibrationMidReading, float calibrationMidReference, float calibrationHighReading, float calibrationHighReference, float calibrationSingleOffset)
    {
      _write_4_bytes(CALIBRATE_READLOW_REGISTER, calibrationLowReading);
      _write_4_bytes(CALIBRATE_REFLOW_REGISTER, calibrationLowReference);
      _write_4_bytes(CALIBRATE_READMID_REGISTER, calibrationMidReading);
      _write_4_bytes(CALIBRATE_REFMID_REGISTER, calibrationMidReference);
      _write_4_bytes(CALIBRATE_READHIGH_REGISTER, calibrationHighReading);
      _write_4_bytes(CALIBRATE_REFHIGH_REGISTER, calibrationHighReference);
      _write_4_bytes(CALIBRATE_SINGLE_OFFSET_REGISTER, calibrationSingleOffset);
    }

    // Changes the I2C address.
    void i2c::setI2CAddress(uint8_t i2cAddress)
    {
      _write_4_bytes(MS_REGISTER, i2cAddress);
      _send_command(I2C_TASK);
      _address = i2cAddress;
    }

    // If measureEC (or related) was called with blocking = true, this retrieves the latest pH measurement.
    void i2c::update()
    {
      _updateRegisters();
    }

    float i2c::_mS_to_mS25(float mS, float tempC, float tempCoef)
    {
      return mS / (1 - (tempCoef * (tempC - 25)));
    }

    void i2c::_updateRegisters()
    {
      _status = _read_byte(STATUS_REGISTER);
      _mS = _read_4_bytes(MS_REGISTER);
      _PSU = _read_4_bytes(PSU_REGISTER);
      _density = _read_4_bytes(DENSITY_REGISTER);
      _tempC = _read_4_bytes(TEMP_C_REGISTER);

      if (_status == 0)
      {
        _PPM_500 = _mS * 500;
        _PPM_640 = _mS * 640;
        _PPM_700 = _mS * 700;
        _uS = _mS * 1000;
        __S = _mS / 1000;
        _tempF = ((tempC * 9) / 5) + 32;
        getDeviceInfo();
      }
      else
      {
        _mS = 0;
        _PPM_500 = 0;
        _PPM_640 = 0;
        _PPM_700 = 0;
        _uS = 0;
        __S = 0;
        _tempF = -127;
        _tempC = -127;
        _PSU = -127;
      }
    }

    void i2c::_send_command(uint8_t command)
    {
      _i2cPort->beginTransmission(_address);
      _i2cPort->write(TASK_REGISTER);
      _i2cPort->write(command);
      _i2cPort->endTransmission();
    }

    void i2c::_write_4_bytes(uint8_t reg, float f)
    {
      uint8_t b[5];
      float f_val = f;

      b[0] = reg;
      b[1] = *((uint8_t *)&f_val);
      b[2] = *((uint8_t *)&f_val + 1);
      b[3] = *((uint8_t *)&f_val + 2);
      b[4] = *((uint8_t *)&f_val + 3);
      _i2cPort->beginTransmission(_address);
      _i2cPort->write(b, 5);
      _i2cPort->endTransmission();
      //delay(10);
    }

    float i2c::_read_4_bytes(uint8_t reg)
    {
      float retval;

      _i2cPort->beginTransmission(_address);
      _i2cPort->write(reg);
      _i2cPort->endTransmission();
      _i2cPort->requestFrom(_address, (uint8_t)4);
      *((uint8_t *)&retval) = _i2cPort->read();
      *((uint8_t *)&retval + 1) = _i2cPort->read();
      *((uint8_t *)&retval + 2) = _i2cPort->read();
      *((uint8_t *)&retval + 3) = _i2cPort->read();
      return retval;
    }

    void i2c::_write_byte(uint8_t reg, uint8_t val)
    {
      uint8_t b[5];

      b[0] = reg;
      b[1] = val;
      _i2cPort->beginTransmission(_address);
      _i2cPort->write(b, 2);
      _i2cPort->endTransmission();
    }

    uint8_t i2c::_read_byte(uint8_t reg)
    {
      uint8_t retval;

      _i2cPort->beginTransmission(_address);
      _i2cPort->write(reg);
      _i2cPort->endTransmission();
      _i2cPort->requestFrom(_address, (uint8_t)1);
      retval = _i2cPort->read();

      return retval;
    }
  } // namespace Mod_EC
} // namespace uFire