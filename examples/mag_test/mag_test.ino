#include <Wire.h>
#include <DevLab_ICM20948.h>
// -----------------------------------------------------------
// PINES I2C — ESP32C6 NANO Unit Electronics
// -----------------------------------------------------------
#define SDA_PIN   6
#define SCL_PIN   7
#define I2C_FREQ  400000UL
#define ICM_ADDR  0x69

DevLab_ICM20948 imu;

void printLinea()      { Serial.println(F("------------------------------------------------------------")); }
void printDobleLinea() { Serial.println(F("============================================================")); }

const ICM20948_Op_Mode opMode[] = {
  ICM20948_Op_Mode::MODE_POWER_DOWN,
  ICM20948_Op_Mode::MODE_SINGLE_MEASUREMENT,
  ICM20948_Op_Mode::MODE_CONTINUOUS_1,
  ICM20948_Op_Mode::MODE_CONTINUOUS_2,
  ICM20948_Op_Mode::MODE_CONTINUOUS_3,
  ICM20948_Op_Mode::MODE_CONTINUOUS_4,
  ICM20948_Op_Mode::MODE_SELF_TEST
};

const char* etiquetasOpModeCfg[] = {
  "PowerDown","SingleMeasure","Continuo1","Continuo2","Continuo3","Continuo4"
};
const uint8_t N_OPM = 6;

void applySettings(){
 /** Initialize magnetometer */
  if (!imu.initMag()) {
    Serial.println(F("ERROR: initMag() failed"));
    while (1) delay(200);
  }
  for(int i = 2; i < N_OPM; i++){
    imu.setMagOpMode(opMode[i]);
     delay(100);
    Serial.println(F("Magnetometer ready"));
     float mx, my, mz;

    /** Read magnetometer data
    * - Output in microtesla (uT)
    * - Returns true on success
    */
    for(int r = 0; r  < 500 ; r++){
      if (imu.readMag(mx, my, mz)) {
        Serial.printf("[%d]",r);
        Serial.print(";");
        Serial.print(etiquetasOpModeCfg[i]);
        Serial.print(";");
        Serial.print(mx, 2); Serial.print(F("; "));
        Serial.print(my, 2); Serial.print(F("; "));
        Serial.println(mz, 2);
      } else {
        Serial.println(F("Mag read failed"));
      }
    }
    Serial.println(F("-----------------------------"));
    delay(500);
  }

}
void setup() {
  // put your setup code here, to run once:
 Serial.begin(115200);
  delay(200);
  Serial.println(F("ICM-20948 (I2C) — Magnetometer Example"));
  
  Wire.begin(SDA_PIN,SCL_PIN);
  /** Initialize IMU */
  if (!imu.beginI2C(ICM_ADDR, Wire, 400000)) {
    Serial.println(F("ERROR: beginI2C() failed."));
    while (1) delay(200);
  }

  Serial.println(F("ICM-20948 ready (I2C)."));

  /** Verify device */
  uint8_t who;
  if (!imu.readWhoAmI(who) || who != 0xEA) {
    Serial.println(F("ERROR: WHO_AM_I mismatch"));
    while (1) delay(200);
  }

  Serial.print(F("WHO_AM_I = 0x"));
  Serial.println(who, HEX);

  /** Enable required sensors (mag uses internal I2C master)
   * - accel/gyro not strictly required but safe to keep ON
   */
  if (!imu.setSensors(true, true, false)) {
    Serial.println(F("setSensors failed."));
  }

  Serial.println(F("  Sensor listo.\n"));
  Serial.println(F("  Sensor PLANO y QUIETO sobre la mesa."));
  Serial.println(F("  Iniciando en 5 segundos..."));
  for (int i = 5; i > 0; i--) {
    Serial.printf("  %d...\n", i);
    delay(1000);
  }

  applySettings();

}

void loop() {
  // put your main code here, to run repeatedly:

}
