/***************************************************************
 * @file    I2C_Magneto.ino
 * @author  Jonathan Mejorado Lopez
 * @brief   Minimal I2C bring-up for DevLab ICM-20948 +
 *          AK09916 magnetometer readout (updated API).
 * Features
 * - SPI init (UNO / ESP32)
 * - beginSPI() initialization
 * - Manual sensor enable
 * - Read Accel / Gyro / Temp
 *
 * Notes
 * - WHO_AM_I must be 0xEA
 * - Sensors must be explicitly enabled
 * - SPI speed ~1MHz recommended during init
 *
 * Wiring (Arduino UNO SPI)
 * - SCK(SCL)  -> D13
 * - MOSI(SDA) -> D11
 * - MISO(SD0/AD0) -> D12
 * - CS(nCS)   -> D10 (changeable; update CS_PIN)
 * - VCC  -> 3V3
 * - GND  -> GND
 *
 * Wiring (ESP32 VSPI default)
 * - SCK  -> D13
 * - MOSI -> D11
 * - MISO -> D12
 * - CS   -> D10
 *
 * License : MIT
 ***************************************************************/

#include <DevLab_ICM20948.h>

/* ====================== User Config ======================= */
/** @brief Chip-select pin for SPI. */
#define CS_PIN D10

/** @brief SPI clock used during sensor initialization and reads. */

#define SPI_FAST_SPEED 1000000

/** @brief Global ICM-20948 driver instance. */
DevLab_ICM20948 imu;

/** @brief SPI bus object used by the IMU driver. */
SPIClass spi_bus(SPI);


/**
 * @brief Configure I2C, verify the IMU, and initialize the magnetometer.
 *
 * The magnetometer is accessed through the ICM-20948 internal I2C master, so
 * initMag() must succeed before loop() can read magnetic field data.
 */
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println(F("ICM-20948 (SPI) — Magnetometer Example"));

  if (!imu.beginSPI(CS_PIN, spi_bus, SPI_FAST_SPEED)) {
    Serial.println("ERROR: beginSPI() failed");
    while (1) delay(200);
  }

  Serial.println(F("ICM-20948 ready (SPI)."));

  if (!imu.initMag()) {
    Serial.println(F("Mag init failed"));
  } else {
    Serial.println(F("Mag initialized"));
  }
  Serial.println("Ready.");

  Serial.println(F("Magnetometer ready"));
}

/**
 * @brief Read and print magnetometer data in microtesla.
 */
void loop() {
  float mx, my, mz;

  /* Read magnetometer data
   * - Output in microtesla (uT)
   * - Returns true on success
   */
  if (imu.readMag(mx, my, mz)) {
    Serial.print(F("MAG [uT]: "));
    Serial.print(mx, 2); Serial.print(F(", "));
    Serial.print(my, 2); Serial.print(F(", "));
    Serial.println(mz, 2);
  } else {
    Serial.println(F("Mag read failed"));
  }

  Serial.println(F("-----------------------------"));
  delay(500);
}