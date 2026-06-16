#ifndef ICM20948_DEVLAB_H
#define ICM20948_DEVLAB_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "ICM20948_regs.h"

#include "7Semi_Interface.h"
#include "7Semi_I2C_Interface.h"
#include "7Semi_SPI_Interface.h"
#include "BusIO_7Semi.h"

enum class ICM20948_Op_Mode : uint8_t
{
  MODE_POWER_DOWN = 0x00,
  MODE_SINGLE_MEASUREMENT = 0x01,
  MODE_CONTINUOUS_1 = 0x02,
  MODE_CONTINUOUS_2 = 0x04,
  MODE_CONTINUOUS_3 = 0x06,
  MODE_CONTINUOUS_4 = 0x08,
  MODE_SELF_TEST = 0x10
};

enum class ICM20948_Clock_Source : uint8_t
{
  INTERNAL_20MHZ_0 = 0x00, // Internal oscillator
  AUTO_PLL_1 = 0x01,       // Auto select (PLL if ready)
  AUTO_PLL_2 = 0x02,
  AUTO_PLL_3 = 0x03,
  AUTO_PLL_4 = 0x04,
  AUTO_PLL_5 = 0x05,
  INTERNAL_20MHZ_6 = 0x06, // Internal oscillator
  STOP_CLOCK = 0x07        // Stops clock, timing generator reset
};

enum class ICM20948_Gyro_DLPF : uint8_t
{
  DLPF_196HZ = 0x00, // BW ≈ 196.6 Hz, NBW ≈ 229.8 Hz
  DLPF_151HZ = 0x01, // BW ≈ 151.8 Hz, NBW ≈ 187.6 Hz
  DLPF_119HZ = 0x02, // BW ≈ 119.5 Hz, NBW ≈ 154.3 Hz
  DLPF_51HZ = 0x03,  // BW ≈ 51.2 Hz,  NBW ≈ 73.3 Hz
  DLPF_23HZ = 0x04,  // BW ≈ 23.9 Hz,  NBW ≈ 35.9 Hz
  DLPF_11HZ = 0x05,  // BW ≈ 11.6 Hz,  NBW ≈ 17.8 Hz
  DLPF_5HZ = 0x06,   // BW ≈ 5.7 Hz,   NBW ≈ 8.9 Hz
  DLPF_361HZ = 0x07  // BW ≈ 361.4 Hz, NBW ≈ 376.5 Hz
};

enum class ICM20948_Gyro_Average : uint8_t
{
  AVG_1 = 0x00,      // 1x Average (no averaging)
  AVG_2 = 0x01,      // 2x Average
  AVG_4 = 0x02,      // 4x Average
  AVG_8 = 0x03,      // 8x Average 
  AVG_16 = 0x04,     // Average 16 samples
  AVG_32 = 0x05,      // Average 32 samples
  AVG_64 = 0x06,     // Average 64 samples
  AVG_128 = 0x07     // Average 128 samples
};
enum class ICM20948_Gyro_FullScale : uint8_t
{
  DPS_250 = 0x00,  // ±250 dps
  DPS_500 = 0x01,  // ±500 dps
  DPS_1000 = 0x02, // ±1000 dps
  DPS_2000 = 0x03  // ±2000 dps
};

enum class ICM20948_Accel_FullScale : uint8_t
{
  G_2 = 0x00, // ±2g
  G_4 = 0x01, // ±4g
  G_8 = 0x02, // ±8g
  G_16 = 0x03 // ±16g
};

enum class ICM20948_Accel_Average : uint8_t
{
  AVG_1_OR_4 = 0x00, // Depends on ACCEL_FCHOICE
  AVG_8 = 0x01,      // Average 8 samples
  AVG_16 = 0x02,     // Average 16 samples
  AVG_32 = 0x03      // Average 32 samples
};

enum class ICM20948_Accel_DLPFCFG : uint8_t
{
  DLPF0_246HZ = 0x00, // BW ≈ 246 Hz, NBW ≈ 265 Hz
  DLPF_246HZ = 0x01, // BW ≈ 246 Hz, NBW ≈ 265 Hz
  DLPF_111HZ = 0x02, // BW ≈ 111 Hz, NBW ≈ 136 Hz
  DLPF_50HZ = 0x03,  // BW ≈ 50 Hz,  NBW ≈ 68.8 Hz
  DLPF_24HZ = 0x04,  // BW ≈ 24 Hz,  NBW ≈ 34.4 Hz
  DLPF_12HZ = 0x05,  // BW ≈ 12 Hz,  NBW ≈ 17 Hz
  DLPF_6HZ = 0x06,   // BW ≈ 6 Hz,   NBW ≈ 8.3 Hz
  DLPF_473HZ = 0x07, // BW ≈ 473 Hz, NBW ≈ 499 Hz
};

/**
 * ICM20948_IntPinConfig
 *
 * - Physical configuration of the INT pin (INT_PIN_CFG register, BANK 0)
 * - Controls electrical behavior and clear condition of the interrupt line
 * - Pass to intInit() to apply settings
 */
struct ICM20948_IntPinConfig
{
  uint8_t activeLevel    : 1;  // bit 7 | 0 = active high,       1 = active low
  uint8_t driveMode      : 1;  // bit 6 | 0 = push-pull,         1 = open-drain
  uint8_t latchMode      : 1;  // bit 5 | 0 = pulse 50µs,        1 = latch held
  uint8_t clearMode      : 1;  // bit 4 | 0 = read INT_STATUS,   1 = any read
  uint8_t fsyncActLevel  : 1;  // bit 3 | 0 = FSYNC active high, 1 = FSYNC active low
  uint8_t fsyncIntEn     : 1;  // bit 2 | 0 = FSYNC disabled,    1 = FSYNC as interrupt
  uint8_t bypassEn       : 1;  // bit 1 | 0 = normal,            1 = I2C bypass mode
};

/**
 * ICM20948_IntEnableConfig
 *
 * - Selects which interrupt sources are routed to the INT pin
 * - Covers INT_ENABLE (0x10), INT_ENABLE_1 (0x11), INT_ENABLE_2 (0x12), INT_ENABLE_3 (0x13)
 * - FIFO overflow and watermark fields are per-channel arrays [0]–[4]
 * - Pass to intEnableConfig() to write all four registers in one call
 */
struct ICM20948_IntEnableConfig
{
  // --- INT_ENABLE (0x10) ---
  uint8_t wofEn        : 1;  // Wake on FSYNC
  uint8_t womIntEn     : 1;  // Wake on motion
  uint8_t pllRdyEn     : 1;  // PLL ready
  uint8_t dmpInt1En    : 1;  // DMP interrupt
  uint8_t i2cMstIntEn  : 1;  // I2C master interrupt

  // --- INT_ENABLE_1 (0x11) ---
  uint8_t rawDataRdyEn : 1;  // Raw data ready

  // --- INT_ENABLE_2 (0x12) --- canal por canal
  uint8_t fifoOvfEn[5];      // [0]–[4]: 1 = overflow interrupt ON para ese canal

  // --- INT_ENABLE_3 (0x13) --- canal por canal
  uint8_t fifoWmEn[5];       // [0]–[4]: 1 = watermark interrupt ON para ese canal
};

/**
 * ICM20948_IntStatus
 *
 * - Holds the parsed state of the four interrupt status registers
 * - Covers INT_STATUS (0x19), INT_STATUS_1 (0x1A), INT_STATUS_2 (0x1B), INT_STATUS_3 (0x1C)
 * - Populated by checkIntStatus() via a single 4-byte burst read
 * - Reading these registers clears the interrupt flags (when clearMode = 0 in IntPinConfig)
 */
struct ICM20948_IntStatus
{
  // --- INT_STATUS (0x19) ---
  uint8_t womInt     : 1;  // Wake on motion ocurrió
  uint8_t pllRdyInt  : 1;  // PLL listo
  uint8_t dmpInt1    : 1;  // DMP generó interrupción
  uint8_t i2cMstInt  : 1;  // I2C master generó interrupción

  // --- INT_STATUS_1 (0x1A) ---
  uint8_t rawDataRdy : 1;  // Datos de sensores listos para leer

  // --- INT_STATUS_2 (0x1B) ---
  uint8_t fifoOvf[5];      // [0]–[4]: FIFO overflow por canal

  // --- INT_STATUS_3 (0x1C) ---
  uint8_t fifoWm[5];       // [0]–[4]: FIFO watermark por canal
};

/**
 * Class
 * - Manages ICM-20948 over I2C (SPI path disabled in this cut)
 * - Caches scale factors for LSB->physical conversions
 */
class DevLab_ICM20948
{
public:

  DevLab_ICM20948();

  /**
   * beginI2C
   *
   * - Initialize ICM20948 using I2C interface
   * - Sets up communication layer (Interface + BusIO)
   * - Verifies device identity (WHO_AM_I)
   * - Configures basic power and clock settings
   *
   * Returns:
   * - true  → Device initialized successfully
   * - false → Communication or configuration failed
   */
  bool beginI2C(uint8_t address = 0x69, TwoWire &i2cPort = Wire, uint32_t i2cSpeed = 400000);

  bool beginSPI(uint8_t csPin, SPIClass &spiPort = SPI, uint32_t spiSpeed = 1000000);

  /**
   * readWhoAmI
   *
   * - Read WHO_AM_I register (device ID)
   * - Used during initialization to verify ICM20948
   * - Expected value: 0xEA
   *
   * Returns:
   * - true  → Read successful
   * - false → Operation failed
   */
  bool readWhoAmI(uint8_t &whoAmI);

  /**
   * selectBank
   *
   * - Select USER BANK (0–3)
   * - Used to access different register groups inside ICM20948
   *
   * Returns:
   * - true  → Bank switch successful
   * - false → Write failed
   */
  bool selectBank(uint8_t bank);

  /**
   * softReset
   *
   * - Perform software reset of ICM20948
   * - Resets all internal registers to default state
   *
   * Flow:
   * - Select USER BANK 0
   * - Set DEVICE_RESET bit
   * - Wait for device to restart
   *
   * Returns:
   * - true  → Reset successful
   * - false → Operation failed
   */
  bool softReset();

  /**
   * sleep
   *
   * - Enable or disable sleep mode
   * - Maintains clock source (CLKSEL = 1)
   *
   * Flow:
   * - Select USER BANK 0
   * - Set or clear SLEEP bit
   *
   * Returns:
   * - true  → Operation successful
   * - false → Operation failed
   */
  bool sleep(bool en);

  /**
   * applyBasicDefaults
   *
   * - Apply basic sensor configuration
   * - Configure power, filters, ranges, and sampling rates
   *
   * Returns:
   * - true  → Configuration successful
   * - false → Any register write failed
   */
  bool applyBasicDefaults();

  /**
   * readAccel
   *
   * - Read accelerometer data (X, Y, Z)
   * - Convert raw values to g using LSB scale
   *
   * Returns:
   * - true  → Read successful
   * - false → Read failed
   */
  bool readAccel(float &x, float &y, float &z);

  /**
   * readGyro
   *
   * - Read gyroscope data (X, Y, Z)
   * - Convert raw values to dps using LSB scale
   *
   * Returns:
   * - true  → Read successful
   * - false → Read failed
   */
  bool readGyro(float &x, float &y, float &z);

  /**
   * readTemperature
   *
   * - Read temperature sensor
   * - Convert raw values to °C
   *
   * Returns:
   * - true  → Read successful
   * - false → Read failed
   */
  bool readTemperature(float &temperature);

  /**
   * readMag
   *
   * - Read magnetometer data via internal I2C master
   * - Data comes from EXT_SLV_SENS_DATA registers
   * - Convert raw values to µT
   *
   * Returns:
   * - true  → Read successful
   * - false → Read failed
   */
  bool readMag(float &x, float &y, float &z);

  /**
   * initMag
   *
   * - Initialize AK09916 magnetometer using internal I2C master
   * - Verifies WHO_AM_I and enables continuous mode
   * - Configures SLV0 for automatic data read
   *
   * Returns:
   * - true  → Initialization successful
   * - false → Operation failed
   */
  bool initMag();

  /**
   * setMagOpMode
   *
   * - Set magnetometer operation mode
   *
   * Returns:
   * - true  → Mode set successfully
   * - false → Write failed
   */
  bool setMagOpMode(ICM20948_Op_Mode opMode);

  /**
   * setLowPower
   *
   * - Enable or disable low power mode
   * - Controls LP_EN bit in PWR_MGMT_1
   *
   * Returns:
   * - true  → Operation successful
   * - false → Operation failed
   */
  bool setLowPower(bool enable);

  /**
   * getLowPower
   *
   * - Read low power mode status
   * - Returns state of LP_EN bit
   *
   * Returns:
   * - true  → Read successful
   * - false → Operation failed
   */
  bool getLowPower(bool &enable);

  /**
   * setClock
   *
   * - Set clock source (CLKSEL [2:0])
   * - Selects internal clock for sensor operation
   *
   * Returns:
   * - true  → Operation successful
   * - false → Operation failed
   */
  bool setClock(ICM20948_Clock_Source clock);

  /**
   * getClock
   *
   * - Read current clock source (CLKSEL [2:0])
   *
   * Returns:
   * - true  → Read successful
   * - false → Operation failed
   */
  bool getClock(uint8_t &clock);

  /**
   * setGyroSampleRate
   *
   * - Set gyroscope sample rate
   * - Uses internal divider (SRD) based on 1100 Hz base rate
   *
   * Returns:
   * - true  → Operation successful
   * - false → Invalid input or write failed
   */
  bool setGyroSampleRate(float sampleRate);

  /**
   * getGyroSampleRate
   *
   * - Get current gyroscope sample rate
   * - Computes value from divider register
   *
   * Returns:
   * - true  → Read successful
   * - false → Operation failed
   */
  bool getGyroSampleRate(float &sampleRate);
  bool setGyroDivRate(uint8_t divisor);
  /**
   * setDLPF
   *
   * - Configure gyroscope digital low-pass filter (DLPF)
   * - Option to bypass filter (full bandwidth)
   *
   * Returns:
   * - true  → Operation successful
   * - false → Operation failed
   */
  bool setDLPF(ICM20948_Gyro_DLPF dlpf, bool bypass);

  /**
   * getDLPF
   *
   * - Read gyroscope DLPF configuration
   * - Returns filter setting and bypass state
   *
   * Returns:
   * - true  → Read successful
   * - false → Operation failed
   */
  bool getDLPF(uint8_t &dlpf, bool &bypass);

  /**
   * setGyroScale
   *
   * - Set gyroscope full-scale range (FS_SEL)
   * - Controls sensitivity (dps range)
   *
   * Flow:
   * - Validate bus pointer
   * - Select USER BANK 2
   * - Write FS_SEL bits [2:1]
   *
   * Returns:
   * - true  → Operation successful
   * - false → Operation failed
   */
  bool setGyroScale(ICM20948_Gyro_FullScale fullScale);
  bool setGyroAveraging(ICM20948_Gyro_Average avg);
  /**
   * getGyroScale
   *
   * - Get current gyroscope full-scale range (FS_SEL
   *
   * Returns:
   * - true  → Read successful
   * - false → Operation failed
   */
  bool getGyroScale(uint8_t &fullScale);

  /**
   * selfTestGyro
   *
   * - Enable or disable gyroscope self-test per axis
   *
   * Returns:
   * - true  → Operation successful
   * - false → Operation failed
   */
  bool selfTestGyro(bool x, bool y, bool z);

  bool setAccelDivRate(uint16_t divisor);
  /**
   * setAccelSampleRate
   *
   * - Set accelerometer output data rate (ODR)
   * - Uses 1125 Hz base rate with 12-bit divider
   *
   * Returns:
   * - true  → Operation successful
   * - false → Operation failed
   */
  bool setAccelSampleRate(uint16_t sampleRate);

  /**
   * getAccelSampleRate
   *
   * - Get accelerometer output data rate (ODR)
   * - Computes value from divider registers
   *
   * Returns:
   * - true  → Read successful
   * - false → Operation failed
   */
  bool getAccelSampleRate(float &sampleRate);

  /**
   * selfTestAccel
   *
   * - Enable or disable accelerometer self-test per axis
   *
   * Returns:
   * - true  → Operation successful
   * - false → Operation failed
   */
  bool selfTestAccel(bool x, bool y, bool z);

  /**
   * setAccelScale
   *
   * - Set accelerometer full-scale range (FS_SEL)
   * - Controls measurement range (±2g, ±4g, ±8g, ±16g)
   *
   * Returns:
   * - true  → Operation successful
   * - false → Operation failed
   */
  bool setAccelScale(ICM20948_Accel_FullScale fullScale);

  /**
   * getAccelScale
   *
   * - Get current accelerometer full-scale range (FS_SEL)
   *
   * Returns:
   * - true  → Read successful
   * - false → Operation failed
   */
  bool getAccelScale(uint8_t &fullScale);

  /**
   * setAccelDLPF
   *
   * - Configure accelerometer digital low-pass filter (DLPF)
   * - Option to bypass filter (full bandwidth)
   *
   * Flow:
   * - Validate bus pointer
   * - Select USER BANK 2
   * - Set or clear bypass bit
   * - Configure DLPF bits if not bypassed
   *
   * Returns:
   * - true  → Operation successful
   * - false → Operation failed
   */
  bool setAccelDLPF(uint8_t dlpf, bool bypass);

  /**
   * getAccelDLPF
   *
   * - Read accelerometer DLPF configuration
   * - Returns filter setting and bypass state
   *
   * Flow:
   * - Validate bus pointer
   * - Select USER BANK 2
   * - Read ACCEL_CONFIG register
   * - Extract bypass and DLPF bits
   *
   * Returns:
   * - true  → Read successful
   * - false → Operation failed
   */
  bool getAccelDLPF(uint8_t &dlpf, bool &bypass);

  /**
   * setAccelAveraging
   *
   * - Configure accelerometer averaging / decimation (DEC3)
   * - Controls internal averaging of accel samples
   *
   * Flow:
   * - Validate bus pointer
   * - Select USER BANK 2
   * - Write DEC3 bits [1:0]
   *
   * Returns:
   * - true  → Operation successful
   * - false → Operation failed
   */
  bool setAccelAveraging(ICM20948_Accel_Average avg);

  /**
   * getAccelAveraging
   *
   * - Read accelerometer averaging / decimation setting (DEC3)
   *
   * Flow:
   * - Validate bus pointer
   * - Select USER BANK 2
   * - Read DEC3 bits [1:0]
   *
   * Returns:
   * - true  → Read successful
   * - false → Operation failed
   */
  bool getAccelAveraging(uint8_t &avg);

  /**
   * setSensors
   *
   * - Enable or disable accel, gyro, and temperature sensor
   * - Controls power gating via PWR_MGMT_2 and TEMP_DIS
   *
   * Flow:
   * - Validate bus pointer
   * - Select USER BANK 0
   * - Build PWR_MGMT_2 mask
   * - Configure temperature enable/disable
   *
   * Returns:
   * - true  → Operation successful
   * - false → Operation failed
   */
  bool setSensors(bool accel_on, bool gyro_on, bool temp_on);

  /**
   * getSensors
   *
   * - Read accel, gyro, and temperature enable state
   *
   * Flow:
   * - Validate bus pointer
   * - Select USER BANK 0
   * - Read PWR_MGMT_2 register
   * - Read TEMP_DIS bit
   * - Decode enable states
   *
   * Returns:
   * - true  → Read successful
   * - false → Operation failed
   */
  bool getSensors(bool &accel_on, bool &gyro_on, bool &temp_on);

  /**
   * setGyroOffset
   *
   * - Set gyroscope offset values for X, Y, Z axes
   * - Writes offset registers in USER BANK 2
   *
   * Flow:
   * - Validate bus pointer
   * - Select USER BANK 2
   * - Pack offsets into byte array
   * - Write to offset registers
   *
   * Returns:
   * - true  → Operation successful
   * - false → Operation failed
   */
  bool setGyroOffset(uint16_t offsetX, uint16_t offsetY, uint16_t offsetZ);

  /**
   * getGyroOffset
   *
   * - Read gyroscope offset values for X, Y, Z axes
   * - Reads offset registers from USER BANK 2
   *
   * Flow:
   * - Validate bus pointer
   * - Select USER BANK 2
   * - Read offset registers
   * - Convert to signed values
   *
   * Returns:
   * - true  → Read successful
   * - false → Operation failed
   */
  bool getGyroOffset(int16_t &offsetX, int16_t &offsetY, int16_t &offsetZ);

  /**
   * setAccelOffset
   *
   * - Set accelerometer offset values for X, Y, Z axes
   * - Writes offset registers in USER BANK 1
   *
   * Flow:
   * - Validate bus pointer
   * - Select USER BANK 1
   * - Pack offsets into byte array
   * - Write to offset registers
   *
   * Returns:
   * - true  → Operation successful
   * - false → Operation failed
   */
  bool setAccelOffset(int16_t offsetX, int16_t offsetY, int16_t offsetZ);

  /**
   * getAccelOffset
   *
   * - Read accelerometer offset values for X, Y, Z axes
   * - Reads offset registers from USER BANK 1
   *
   * Flow:
   * - Validate bus pointer
   * - Select USER BANK 1
   * - Read offset registers
   * - Convert to signed values
   *
   * Returns:
   * - true  → Read successful
   * - false → Operation failed
   */
  bool getAccelOffset(int16_t &offsetX, int16_t &offsetY, int16_t &offsetZ);

  /**
   * intInit
   *
   * - Configure the physical behavior of the INT pin (INT_PIN_CFG register, BANK 0)
   * - Sets active level, drive mode (push-pull / open-drain), latch vs pulse mode,
   *   clear condition, FSYNC level, FSYNC interrupt enable, and I2C bypass
   *
   * Parameters:
   * - cfg: ICM20948_IntPinConfig struct with the desired pin settings
   *
   * Returns:
   * - true  → Configuration written successfully
   * - false → Operation failed
   */
  bool intInit(const ICM20948_IntPinConfig &cfg);

  /**
   * intEnableConfig
   *
   * - Enable or disable individual interrupt sources routed to the INT pin
   * - Writes INT_ENABLE (0x10), INT_ENABLE_1 (0x11), INT_ENABLE_2 (0x12),
   *   and INT_ENABLE_3 (0x13) in BANK 0
   * - Must call intInit() first to configure the pin electrical behavior
   *
   * Parameters:
   * - cfg: ICM20948_IntEnableConfig struct with the desired interrupt sources enabled
   *
   * Returns:
   * - true  → All four registers written successfully
   * - false → Operation failed
   */
  bool intEnableConfig(const ICM20948_IntEnableConfig &cfg);

  /**
   * checkIntStatus
   *
   * - Read and parse all four interrupt status registers in a single burst read
   * - Covers INT_STATUS (0x19), INT_STATUS_1 (0x1A), INT_STATUS_2 (0x1B),
   *   INT_STATUS_3 (0x1C) from BANK 0
   * - Reading clears the interrupt flags when clearMode = 0 in ICM20948_IntPinConfig
   * - Typically called inside the INT pin ISR or polled in the main loop
   *
   * Parameters:
   * - status: ICM20948_IntStatus struct populated with the current interrupt flags
   *
   * Returns:
   * - true  → Status read successfully
   * - false → Operation failed
   */
  bool checkIntStatus(ICM20948_IntStatus &status);

  /**
   * auxMasterEnable
   *
   * - Enable the ICM20948 internal I2C master for auxiliary bus
   * - Clears BYPASS_EN so the aux bus is controlled by the ICM
   * - Sets I2C_MST_EN in USER_CTRL
   * - Configures auxiliary master clock frequency
   *
   * Parameters:
   * - clkFreq: clock divider value (e.g. 0x07 ≈ 345.6 kHz, 0x0D ≈ 400 kHz)
   *
   * Returns:
   * - true  → Master enabled successfully
   * - false → Operation failed
   */
  bool auxMasterEnable(uint8_t clkFreq);

  /**
   * auxWriteByte
   *
   * - Write a single byte to a register of an auxiliary I2C slave via SLV4
   * - Uses one-shot SLV4 transaction: polls SLV4_DONE, checks for NACK
   * - Suitable for configuration writes (not continuous streaming)
   *
   * Parameters:
   * - slaveAddr: 7-bit I2C address of the auxiliary slave
   * - reg:       target register address on the slave
   * - data:      byte to write
   *
   * Returns:
   * - true  → Write acknowledged by slave
   * - false → Timeout or NACK
   */
  bool auxWriteByte(uint8_t slaveAddr, uint8_t reg, uint8_t data);

  /**
   * auxReadByte
   *
   * - Read a single byte from a register of an auxiliary I2C slave via SLV4
   * - Generates a combined write-then-read transaction (reg byte + repeated START)
   * - Suitable for slaves that follow standard I2C register-read protocol
   *
   * Note: slaves that need separate write/read transactions (e.g. command-response
   * firmware) should use auxWriteCommand + auxReadResponse instead.
   *
   * Parameters:
   * - slaveAddr: 7-bit I2C address of the auxiliary slave
   * - reg:       register address to read from
   * - data:      output byte received from slave
   *
   * Returns:
   * - true  → Read successful
   * - false → Timeout or NACK
   */
  bool auxReadByte(uint8_t slaveAddr, uint8_t reg, uint8_t &data);

  /**
   * auxWriteCommand
   *
   * - Send a single command byte to an auxiliary I2C slave via SLV4
   * - Uses REG_DIS: generates [START, addr+W, cmd_byte, STOP] with no register prefix
   * - Designed for slaves that use a command-response protocol (no register addressing)
   *
   * Parameters:
   * - slaveAddr: 7-bit I2C address of the auxiliary slave
   * - cmd:       command byte to send
   *
   * Returns:
   * - true  → Command acknowledged by slave
   * - false → Timeout or NACK
   */
  bool auxWriteCommand(uint8_t slaveAddr, uint8_t cmd);

  /**
   * auxConfigSlave
   *
   * - Configure SLV0 for automatic periodic reads from an auxiliary I2C slave
   * - The ICM20948 master will read numBytes starting at reg on every ODR cycle
   * - Data is deposited into EXT_SLV_SENS_DATA_00 and subsequent registers
   * - Call once during initialization; read results with auxReadSensorData
   *
   * Parameters:
   * - slaveAddr: 7-bit I2C address of the auxiliary slave
   * - reg:       starting register address to read from on the slave
   * - numBytes:  number of bytes to read per cycle (1–15)
   *
   * Returns:
   * - true  → SLV0 configured successfully
   * - false → Operation failed
   */
  bool auxConfigSlave(uint8_t slaveAddr, uint8_t reg, uint8_t numBytes);

  /**
   * auxReadSensorData
   *
   * - Read bytes from EXT_SLV_SENS_DATA registers (BANK 0)
   * - Returns data collected by the ICM20948 master from SLV0–SLV3 on the last cycle
   * - Must call auxConfigSlave first to set up the automatic read
   *
   * Parameters:
   * - buf: output buffer to store the received bytes
   * - len: number of bytes to read (must match numBytes configured in auxConfigSlave)
   *
   * Returns:
   * - true  → Read successful
   * - false → Bus error
   */
  bool auxReadSensorData(uint8_t *buf, uint8_t len);

  /**
   * auxReadResponse
   *
   * - Read a single response byte from an auxiliary I2C slave via SLV4
   * - Generates a pure read transaction: [START, addr+R, 1 byte, STOP]
   * - No register byte is sent (REG_DIS); slave must already be ready to transmit
   * - Use after auxWriteCommand to complete a command-response exchange with slaves
   *   that require separate write and read transactions (e.g. PY32F0 firmware slaves)
   *
   * Parameters:
   * - slaveAddr: 7-bit I2C address of the auxiliary slave
   * - data:      output byte received from slave
   *
   * Returns:
   * - true  → Response received successfully
   * - false → Timeout or NACK
   */
  bool auxReadResponse(uint8_t slaveAddr, uint8_t &data);

  /**
   * auxRead12bit
   *
   * - Read a 12-bit value previously configured via auxConfigSlave (SLV0, numBytes = 2)
   * - Assumes little-endian packing: first byte = LSB, second byte = MSB
   * - Result is masked to 12 bits (0-4095)
   *
   * Returns:
   * - true  → Read successful
   * - false → Read failed
   */
  bool auxRead12bit(uint16_t &raw);
private:
  I2C_Interface i2c;
  SPI_Interface spi;

  Interface_7Semi *iface = nullptr;

  BusIO_7Semi<Interface_7Semi> *bus = nullptr;

  float mg_per_lsb = 16384.0f;        // LSB/g at ±16g
  float degree_per_second = 131.072f; // LSB/dps at ±2000 dps

  bool writeSlave4(uint8_t reg, uint8_t value); 
  bool readSlave4(uint8_t reg, uint8_t &value);
};


#endif /* ICM20948_DEVLAB_H */