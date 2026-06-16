#include <Wire.h>
#include <DevLab_ICM20948.h>

#define SDA_PIN   6
#define SCL_PIN   7
#define I2C_FREQ  400000UL
#define ICM_ADDR  0x69
#define PIN_INT   D7   // ← ajusta a tu pin real

DevLab_ICM20948 imu;

void printLinea()      { Serial.println(F("------------------------------------------------------------")); }
void printDobleLinea() { Serial.println(F("============================================================")); }

ICM20948_IntPinConfig pinCfg = {
  .activeLevel   = 1,
  .driveMode     = 0,
  .latchMode     = 0,
  .clearMode     = 1,
  .fsyncActLevel = 0,
  .fsyncIntEn    = 0,
  .bypassEn      = 0
};

ICM20948_IntEnableConfig intEn = {
  .rawDataRdyEn = 1   // ← inicialización directa
};

volatile uint32_t isrCount = 0;
void IRAM_ATTR isrHandler() {
  isrCount++;
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!imu.beginI2C(ICM_ADDR, Wire, I2C_FREQ)) {
    Serial.println(F("ERROR: beginI2C() failed."));
    while (1) delay(200);
  }
  Serial.println(F("ICM-20948 ready."));

  uint8_t who;
  if (!imu.readWhoAmI(who) || who != 0xEA) {
    Serial.println(F("ERROR: WHO_AM_I mismatch"));
    while (1) delay(200);
  }
  Serial.printf("WHO_AM_I = 0x%02X\n", who);

  if(!imu.intInit(pinCfg)){
    Serial.println(F("ERROR: Initialization interruption failed"));
    while(1) delay(200);
  };           // ← nombre correcto
  if(!imu.intEnableConfig(intEn)){
    Serial.println(F("ERROR: Initialization interruption failed"));
    while(1) delay(200);
  }

  attachInterrupt(digitalPinToInterrupt(PIN_INT), isrHandler, FALLING);
  Serial.println(F("Interrupt configured. Waiting..."));
}

void loop() {
  static uint32_t lastCount = 0;

  if (isrCount != lastCount) {
    lastCount = isrCount;
    Serial.printf(">>> ISR disparada! Total: %lu\n", isrCount);
  }
}