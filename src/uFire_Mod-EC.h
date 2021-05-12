#ifndef Mod_EC_H
#define Mod_EC_H

#include <Arduino.h>
#include <Wire.h>

#define UFIRE_MOD_EC 0x0a
#define MEASURE_EC_TASK 80      /*!< Command to start an EC measure */
#define MEASURE_TEMP_TASK 40    /*!< Command to measure temperature */
#define CALIBRATE_LOW_TASK 20   /*!< Command to calibrate the probe */
#define CALIBRATE_MID_TASK 10   /*!< Command to calibrate the probe */
#define CALIBRATE_HIGH_TASK 8   /*!< Command to calibrate the high point of the probe */
#define CALIBRATE_SINGLE_TASK 4 /*!< Command to calibrate the high point of the probe */
#define I2C_TASK 2              /*!< Command to change the i2c address */

#define EC_EC_MEASUREMENT_TIME 750
#define EC_TEMP_MEASURE_TIME 750

#define HW_VERSION_REGISTER 0               /*!< hardware version register */
#define FW_VERSION_REGISTER 1               /*!< firmware version  register */
#define TASK_REGISTER 2                     /*!< task register */
#define STATUS_REGISTER 3                   /*!< status register */
#define MS_REGISTER 4                       /*!< mS register */
#define PSU_REGISTER 8                      /*!< PSU register */
#define TEMP_C_REGISTER 12                  /*!< temperature in C register */
#define CALIBRATE_REFLOW_REGISTER 16        /*!< reference low register */
#define CALIBRATE_READLOW_REGISTER 20       /*!< read high register */
#define CALIBRATE_REFMID_REGISTER 24        /*!< reference mid register */
#define CALIBRATE_READMID_REGISTER 28       /*!< reading mid register */
#define CALIBRATE_REFHIGH_REGISTER 32       /*!< reference high register */
#define CALIBRATE_READHIGH_REGISTER 36      /*!< reading high register */
#define CALIBRATE_SINGLE_OFFSET_REGISTER 40 /*!< calibration temperature register */
#define COEFFICIENT_REGISTER 44             /*!< temperature coefficient register */
#define CONSTANT_REGISTER 48                /*!< temperature constant register */
#define K_REGISTER 52                       /*!< Probe cell constant register */
#define KPA_REGISTER 56                     /*!< kPa register */
#define DENSITY_REGISTER 60                 /*!< density register */

namespace uFire
{
  namespace Mod_EC
  {
    class i2c
    {
    public:
      const float &S = __S;
      const float &mS = _mS;
      const float &uS = _uS;
      const float &PSU = _PSU;
      const float &density = _density;
      const int &PPM_500 = _PPM_500;
      const int &PPM_640 = _PPM_640;
      const int &PPM_700 = _PPM_700;
      const float &tempC = _tempC;
      const float &tempF = _tempF;
      const float &calibrationLowReading = _calibrationLowReading;
      const float &calibrationLowReference = _calibrationLowReference;
      const float &calibrationMidReading = _calibrationMidReading;
      const float &calibrationMidReference = _calibrationMidReference;
      const float &calibrationHighReading = _calibrationHighReading;
      const float &calibrationHighReference = _calibrationHighReference;
      const float &calibrationSingleOffset = _calibrationSingleOffset;
      const int &hwVersion = _hwVersion;
      const int &fwVersion = _fwVersion;
      const int &status = _status;

      typedef enum EC_Error_e
      {
        STATUS_NO_ERROR,
        STATUS_NO_PROBE_RANGE,
        STATUS_SYSTEM_ERROR,
        STATUS_CONFIG_ERROR
      } EC_Error_t;

      bool begin(TwoWire &wirePort = Wire, uint8_t address = UFIRE_MOD_EC);
      bool connected();
      uint8_t calibrateLow(float solutionEC, float tempC = 25.0, float tempCoef = 0.019, float tempConst = 25.0, float k = 1.0, bool blocking = true);
      uint8_t calibrateHigh(float solutionEC, float tempC = 25.0, float tempCoef = 0.019, float tempConst = 25.0, float k = 1.0, bool blocking = true);
      uint8_t calibrateMid(float solutionEC, float tempC = 25.0, float tempCoef = 0.019, float tempConst = 25.0, float k = 1.0, bool blocking = true);
      uint8_t calibrateSingle(float solutionEC, float tempC = 25.0, float tempCoef = 0.019, float tempConst = 25.0, float k = 1.0, bool blocking = true);
      void getDeviceInfo();
      float measureEC(float tempC = 25.0, float tempCoef = 0.019, float tempConst = 25.0, float k = 1.0, float kPa = 0, bool blocking = true);
      float measureSeawater(float tempC = 25.0, float tempCoef = 0.021, float tempConst = 25.0, float k = 10.0, float kPa = 0, bool blocking = true);
      float measureFreshwater(float tempC = 25.0, float tempCoef = 0.019, float tempConst = 25.0, float k = 1.0, bool blocking = true);
      float measurePurewater(float tempC = 25.0, float tempCoef = 0.019, float tempConst = 25.0, float k = 0.1, bool blocking = true);
      float measureTemp(bool blocking = true);
      void reset();
      void setDeviceInfo(float calibrationLowReading, float calibrationLowReference, float calibrationMidReading, float calibrationMidReference, float calibrationHighReading, float calibrationHighReference, float calibrationSingleOffset);
      void setI2CAddress(uint8_t i2cAddress);
      void update();

    private:
      uint8_t _address;
      TwoWire *_i2cPort;
      static float __S;
      static float _mS;
      static float _uS;
      static float _PSU;
      static int _PPM_500;
      static int _PPM_640;
      static int _PPM_700;
      static float _tempC;
      static float _tempF;
      static float _density;
      static float _calibrationLowReading;
      static float _calibrationLowReference;
      static float _calibrationMidReading;
      static float _calibrationMidReference;
      static float _calibrationHighReading;
      static float _calibrationHighReference;
      static float _calibrationSingleOffset;
      static int _hwVersion;
      static int _fwVersion;
      static int _i2c_address;
      static int _status;

      float _mS_to_mS25(float mS, float tempC, float tempCoef);
      void _updateRegisters();
      void _send_command(uint8_t command);
      void _write_4_bytes(uint8_t reg, float f);
      void _write_byte(uint8_t reg, uint8_t val);
      float _read_4_bytes(uint8_t reg);
      uint8_t _read_byte(uint8_t reg);
    };
  } // namespace Mod_EC
} // namespace uFire
#endif // ifndef Mod_EC_H