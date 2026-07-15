#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <RadioLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ============================================================
// Heltec WiFi LoRa 32 V3 — SX1262 pins
// ============================================================

constexpr int LORA_NSS  = 8;
constexpr int LORA_DIO1 = 14;
constexpr int LORA_RST  = 12;
constexpr int LORA_BUSY = 13;

constexpr int LORA_SCK  = 9;
constexpr int LORA_MISO = 11;
constexpr int LORA_MOSI = 10;

// ============================================================
// Heltec WiFi LoRa 32 V3 — OLED pins
// ============================================================

constexpr int OLED_SDA  = 17;
constexpr int OLED_SCL  = 18;
constexpr int OLED_RST  = 21;
constexpr int VEXT_CTRL = 36;

constexpr int SCREEN_WIDTH  = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr int OLED_ADDRESS  = 0x3C;

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RST
);

// ============================================================
// Training-emitter settings
// ============================================================

constexpr float FREQUENCY_MHZ = 915.000;
constexpr int8_t TX_POWER_DBM = 2;

constexpr unsigned int UNIT_MS = 150;

SX1262 radio = new Module(
  LORA_NSS,
  LORA_DIO1,
  LORA_RST,
  LORA_BUSY
);

unsigned long patternCount = 0;
char currentLetter = '-';
bool carrierActive = false;

// ============================================================
// OLED functions
// ============================================================

void updateDisplay() {
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("FOX-01");

  display.setTextSize(1);

  display.setCursor(0, 22);
  display.print("Freq: ");
  display.print(FREQUENCY_MHZ, 3);
  display.println(" MHz");

  display.setCursor(0, 34);
  display.print("Power: ");
  display.print(TX_POWER_DBM);
  display.println(" dBm");

  display.setCursor(0, 46);
  display.print("Letter: ");
  display.print(currentLetter);

  display.setCursor(74, 46);
  display.print(carrierActive ? "TX" : "--");

  display.setCursor(0, 57);
  display.print("Pattern: ");
  display.print(patternCount);

  display.display();
}

void showStartupScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(18, 8);
  display.println("FOX-01");

  display.setTextSize(1);
  display.setCursor(20, 34);
  display.println("915 MHz Emitter");

  display.setCursor(31, 50);
  display.println("Starting...");

  display.display();
}

void showError(const char* message, int16_t errorCode) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("ERROR");

  display.setTextSize(1);
  display.setCursor(0, 22);
  display.println(message);

  display.setCursor(0, 48);
  display.print("Code: ");
  display.println(errorCode);

  display.display();
}

// ============================================================
// Error handling
// ============================================================

void haltWithError(const char* message, int16_t state) {
  Serial.print(message);
  Serial.print(" Error code: ");
  Serial.println(state);

  showError(message, state);

  while (true) {
    delay(1000);
  }
}

// ============================================================
// Carrier control
// ============================================================

void carrierOn() {
  int16_t state = radio.transmitDirect();

  if (state != RADIOLIB_ERR_NONE) {
    haltWithError("Carrier start failed", state);
  }

  carrierActive = true;
  updateDisplay();
}

void carrierOff() {
  int16_t state = radio.standby();

  if (state != RADIOLIB_ERR_NONE) {
    haltWithError("Carrier stop failed", state);
  }

  carrierActive = false;
  updateDisplay();
}

// ============================================================
// Morse timing
// ============================================================

void dit() {
  carrierOn();
  delay(UNIT_MS);

  carrierOff();
  delay(UNIT_MS);
}

void dah() {
  carrierOn();
  delay(UNIT_MS * 3);

  carrierOff();
  delay(UNIT_MS);
}

void letterSpace() {
  // The final dit/dah already included one unit of silence.
  delay(UNIT_MS * 2);
}

void wordSpace() {
  // The final dit/dah already included one unit of silence.
  delay(UNIT_MS * 6);
}

// ============================================================
// FOX pattern: ..-. --- -..-
// ============================================================

void sendFOX() {
  // F: ..-.
  currentLetter = 'F';
  updateDisplay();

  dit();
  dit();
  dah();
  dit();

  letterSpace();

  // O: ---
  currentLetter = 'O';
  updateDisplay();

  dah();
  dah();
  dah();

  letterSpace();

  // X: -..-
  currentLetter = 'X';
  updateDisplay();

  dah();
  dit();
  dit();
  dah();

  currentLetter = '-';
  wordSpace();
}

// ============================================================
// Setup
// ============================================================

void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println();
  Serial.println("Heltec V3 FOX-01 training emitter");

  // ----------------------------------------------------------
  // Enable OLED/peripheral power
  // ----------------------------------------------------------

  pinMode(VEXT_CTRL, OUTPUT);

  // On Heltec V3, LOW enables Vext.
  digitalWrite(VEXT_CTRL, LOW);
  delay(100);

  // ----------------------------------------------------------
  // Initialize OLED
  // ----------------------------------------------------------

  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  delay(20);

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS,
        false,
        false
      )) {
    Serial.println("OLED initialization failed.");

    while (true) {
      delay(1000);
    }
  }

  showStartupScreen();
  delay(1500);

  // ----------------------------------------------------------
  // Initialize SX1262
  // ----------------------------------------------------------

  Serial.println("Initializing SPI...");

  SPI.begin(
    LORA_SCK,
    LORA_MISO,
    LORA_MOSI,
    LORA_NSS
  );

  Serial.println("Initializing SX1262...");

  int16_t state = radio.begin();

  if (state != RADIOLIB_ERR_NONE) {
    haltWithError("Radio init failed", state);
  }

  state = radio.setFrequency(FREQUENCY_MHZ);

  if (state != RADIOLIB_ERR_NONE) {
    haltWithError("Frequency failed", state);
  }

  state = radio.setOutputPower(TX_POWER_DBM);

  if (state != RADIOLIB_ERR_NONE) {
    haltWithError("Power setup failed", state);
  }

  state = radio.standby();

  if (state != RADIOLIB_ERR_NONE) {
    haltWithError("Standby failed", state);
  }

  Serial.println("Radio ready.");
  Serial.print("Frequency: ");
  Serial.print(FREQUENCY_MHZ, 3);
  Serial.println(" MHz");

  Serial.print("Power: ");
  Serial.print(TX_POWER_DBM);
  Serial.println(" dBm");

  Serial.println("Transmitting repeating FOX pattern.");

  updateDisplay();
  delay(1000);
}

// ============================================================
// Main loop
// ============================================================

void loop() {
  patternCount++;

  Serial.print("FOX pattern ");
  Serial.println(patternCount);

  sendFOX();
  updateDisplay();
}