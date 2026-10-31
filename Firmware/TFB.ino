/******************************************************************************
 * PROJECT: The Tropical Fruit Bowl
 * AUTHOR:  TheTank87
 * BOARD:   Seeed Studio XIAO ESP32-C3
 *
 * DESCRIPTION:
 * Firmware that loads the readings from my sensors. Includes text and numerical readings as well as a graph of the data.
 *
 *
 * You need these libraries for this to work :
 * 1. Adafruit GFX Library
 * 2. Adafruit SSD1306
 * 3. Adafruit MPU6050
 * 4. Adafruit Unified Sensor
 * 5. DHT sensor library (by Adafruit)
 * 6. ScioSense ENS160
 * 7. Adafruit AHTX0
 ******************************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>
#include <DHT.h>

// ============================================================================
// Hardware Pin Defs 
// ============================================================================
#define PIN_ENC1    D1   // ENC1
#define PIN_ENC2    D2   // ENC2
#define PIN_SLIDER  D3   // Slide
#define PIN_I2C_SDA D4   // SDA
#define PIN_I2C_SCL D5   // SCL
#define PIN_DHT     D6   // DATA
#define PIN_SW2     D7   // GPIO2
#define PIN_SW1     D8   // GPIO1 

// ============================================================================
// Display and Color customization settings
// ============================================================================
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_I2C_ADDR 0x3C


// Note: J1 on the PCB is a 128x64 I2C OLED (SSD1306). On monochrome/bi-color
// SSD1306 displays, non-zero colors map to lit pixels White, Yellow, Blue.
// If you swap J1 for a full-color I2C/SPI RGB OLED, these 16-bit RGB565 hex
#define COLOR_BLACK   0x0000  // SSD1306_BLACK (0)
#define COLOR_WHITE   0xFFFF  // SSD1306_WHITE (1) - Used for Cursor & Text
#define COLOR_YELLOW  0xFFE0  // Highlight border color when hovering over buttons
#define COLOR_BLUE    0x001F  // More colors below
#define COLOR_GREEN   0x07E0  // 
#define COLOR_RED     0xF800  // 
#define COLOR_DARKGREY 0x2104 // 

// ---> App Background color changer <---
// For SSD1306 OLED: Use COLOR_BLACK (0) for dark mode or COLOR_WHITE (1) for light mode.
// For RGB Displays: Use any 16-bit RGB565 color (e.g., COLOR_BLUE, 0x1082, etc.).
uint16_t appBackgroundColor = COLOR_BLACK;

// Foreground/UI Element Colors
uint16_t appTextColor        = COLOR_WHITE;
uint16_t cursorColor         = COLOR_WHITE;
uint16_t hoverBorderColor    = COLOR_YELLOW;
uint16_t defaultBorderColor  = COLOR_WHITE;
uint16_t graphLineColor      = COLOR_WHITE;


void setAppBackgroundColor(uint16_t newBgColor) {
  appBackgroundColor = newBgColor;
  
  if (appBackgroundColor == COLOR_WHITE) {
    appTextColor       = COLOR_BLACK;
    defaultBorderColor = COLOR_BLACK;
    graphLineColor     = COLOR_BLACK;
  } else {
    appTextColor       = COLOR_WHITE;
    defaultBorderColor = COLOR_WHITE;
    graphLineColor     = COLOR_WHITE;
  }
}

// ============================================================================
// Sensor and Object Displays (Actual)
// ============================================================================
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MPU6050 mpu;
Adafruit_AHTX0   aht21;

#define DHTTYPE DHT11
DHT dht(PIN_DHT, DHTTYPE);

// ENS160 I2C Register Definitions (Standard I2C address 0x53 or 0x52)
#define ENS160_ADDR       0x53
#define ENS160_REG_OPMODE 0x10
#define ENS160_REG_DATA_AQI  0x21
#define ENS160_REG_DATA_TVOC 0x22
#define ENS160_REG_DATA_ECO2 0x24

// Flags to track if hardware sensors were detected on boot, to prevents lockups
bool mpuFound   = false;
bool ahtFound   = false;
bool ensFound   = false;

// ============================================================================
// State machine and UI Architecture
// ============================================================================
enum AppState {
  STATE_OFF,          // Board OFF
  STATE_INTRO,        // Intro screen
  STATE_MAIN_MENU,    // Main Menu
  STATE_SENSOR_1,     // DHT11 Window
  STATE_SENSOR_2,     // ENS160 + AHT21 Window (The new component I added!)
  STATE_SENSOR_3      // MPU6050 (gyroscope to move cursor)
};

AppState currentState = STATE_OFF;
bool exitPopupOpen    = false; // True when Switch 1 is pressed a second time

// UI Button Structure for hit-testing with the Gyroscope Cursor
struct UIButton {
  int16_t x;
  int16_t y;
  int16_t w;
  int16_t h;
  const char* label;
};

// Main Menu Buttons
UIButton mainButtons[3] = {
  {  4, 42, 36, 18, "S1" },  // DHT11 Window
  { 46, 42, 36, 18, "S2" },  // ENS160+AHT21 Window
  { 88, 42, 36, 18, "S3" }   // MPU6050 Window
};

// Sensor bindow back button (Top-right corner)
UIButton backButton = { 92, 2, 34, 12, "Back" };

// Exit popup "Yes" and "No" buttons
UIButton popupYesButton = { 24, 38, 32, 14, "Yes" };
UIButton popupNoButton  = { 72, 38, 32, 14, "No"  };

// ============================================================================
// Gryoscope cursor functions, and the readings for the sensors
// ============================================================================
// Cursor position, starts in the center of the screen
float cursorX = SCREEN_WIDTH / 2.0f;
float cursorY = SCREEN_HEIGHT / 2.0f;
const int CURSOR_RADIUS = 3;
const float GYRO_SENSITIVITY = 1.8f; // Adjust cursor speed when tilting PCB

#define MAX_GRAPH_POINTS 60
float sensor1History[MAX_GRAPH_POINTS] = {0}; // DHT11 Temp (in C)
float sensor2History[MAX_GRAPH_POINTS] = {0}; // ENS160 eCO2 (ppm)
float sensor3History[MAX_GRAPH_POINTS] = {0}; // MPU6050 Tilt/Accel (m/s^2)
int historyIndex = 0;

// Live numerical sensor readings
float dhtTemp = 25.0f, dhtHum = 50.0f;
uint16_t ensECO2 = 400, ensTVOC = 0;
float ahtTemp = 25.0f, ahtHum = 50.0f;
float mpuTiltMag = 9.8f, mpuGyroX = 0.0f, mpuGyroY = 0.0f;

// Graph sizing controlled by slider and rotary encoder
volatile int encoderXLength = 70; 
const int MIN_GRAPH_X_LEN   = 25;
const int MAX_GRAPH_X_LEN   = 116;

int graphHeight     = 24;        
const int BASE_GRAPH_H = 22;     
const int MAX_GRAPH_H  = 44;     

// Timing trackers, update graph every 250ms
unsigned long lastSensorReadTime = 0;
const unsigned long SENSOR_READ_INTERVAL = 250; 
unsigned long lastGyroUpdateTime = 0;

// Button debounce variables for Switch 1 and Switch 2
bool lastSw1State = HIGH;
bool lastSw2State = HIGH;
unsigned long lastSw1Debounce = 0;
unsigned long lastSw2Debounce = 0;
const unsigned long DEBOUNCE_DELAY = 180;

// ============================================================================
// Making sure components don't do weird stuff in the wrong menu.
// ============================================================================
// Reads quadrature signals from ENC1 and ENC2.
// Using the rotary encoder in the main menu does NOTHING.
volatile int lastEncoded = 0;

void IRAM_ATTR updateRotaryEncoder() {
  int MSB = digitalRead(PIN_ENC1);
  int LSB = digitalRead(PIN_ENC2);

  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  
  bool inSensorWindow = (currentState == STATE_SENSOR_1 ||
                         currentState == STATE_SENSOR_2 ||
                         currentState == STATE_SENSOR_3);

  if (inSensorWindow && !exitPopupOpen) {
    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
      encoderXLength += 2; 
    }
    if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
      encoderXLength -= 2; 
    }

    
    if (encoderXLength < MIN_GRAPH_X_LEN) encoderXLength = MIN_GRAPH_X_LEN;
    if (encoderXLength > MAX_GRAPH_X_LEN) encoderXLength = MAX_GRAPH_X_LEN;
  }

  lastEncoded = encoded;
}

// ============================================================================
// ENS160 I2C functions
// ============================================================================
bool initENS160() {
  Wire.beginTransmission(ENS160_ADDR);
  if (Wire.endTransmission() != 0) return false;

  // Set operating mode to standard gas sensing
  Wire.beginTransmission(ENS160_ADDR);
  Wire.write(ENS160_REG_OPMODE);
  Wire.write(0x02);
  return (Wire.endTransmission() == 0);
}

void readENS160(uint16_t &eco2Out, uint16_t &tvocOut) {
  if (!ensFound) return;

  // Read TVOC (2 bytes at 0x22)
  Wire.beginTransmission(ENS160_ADDR);
  Wire.write(ENS160_REG_DATA_TVOC);
  if (Wire.endTransmission(false) == 0 && Wire.requestFrom(ENS160_ADDR, 2) == 2) {
    uint8_t lsb = Wire.read();
    uint8_t msb = Wire.read();
    tvocOut = (msb << 8) | lsb;
  }

  // Read eCO2 (2 bytes at 0x24)
  Wire.beginTransmission(ENS160_ADDR);
  Wire.write(ENS160_REG_DATA_ECO2);
  if (Wire.endTransmission(false) == 0 && Wire.requestFrom(ENS160_ADDR, 2) == 2) {
    uint8_t lsb = Wire.read();
    uint8_t msb = Wire.read();
    eco2Out = (msb << 8) | lsb;
  }
}

// ============================================================================
// UI draw functions
// ============================================================================
// Checks if the white cursor circle is currently hovering over a UIButton
bool isCursorOverButton(const struct UIButton &btn) {

  return (cursorX >= btn.x && cursorX <= (btn.x + btn.w) &&

          cursorY >= btn.y && cursorY <= (btn.y + btn.h));

}

void drawUIButton(const struct UIButton &btn) {

  bool hovered = isCursorOverButton(btn);

  display.fillRect(btn.x, btn.y, btn.w, btn.h, appBackgroundColor);

  if (hovered) {


    display.drawRect(btn.x, btn.y, btn.w, btn.h, hoverBorderColor);

    int16_t innerX = btn.x + 1;

    int16_t innerY = btn.y + 1;

    int16_t innerW = btn.w - 2;

    int16_t innerH = btn.h - 2;

    display.drawRect(innerX, innerY, innerW, innerH, hoverBorderColor);

  } else {

    display.drawRect(btn.x, btn.y, btn.w, btn.h, defaultBorderColor);

  }

  int16_t x1 = 0, y1 = 0;

  uint16_t tw = 0, th = 0;

  display.setTextSize(1);

  display.setTextColor(appTextColor);

  display.getTextBounds(btn.label, 0, 0, &x1, &y1, &tw, &th);

  int16_t textX = btn.x + ((int16_t)btn.w - (int16_t)tw) / 2;

  int16_t textY = btn.y + ((int16_t)btn.h - (int16_t)th) / 2;

  display.setCursor(textX, textY);

  display.print(btn.label);

}

void drawGyroCursor() {

  int cx = (int)cursorX;

  int cy = (int)cursorY;

  display.fillCircle(cx, cy, CURSOR_RADIUS, cursorColor);

  display.drawCircle(cx, cy, CURSOR_RADIUS + 1, appBackgroundColor);

}
// ============================================================================
// Intro screen 
// ============================================================================
// Intro text
void playIntroAndFadeToApp() {
  display.ssd1306_command(SSD1306_DISPLAYON);

  // Restore full brightness
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(0xFF);

  display.clearDisplay();
  display.fillScreen(COLOR_BLACK);
  display.setTextColor(COLOR_WHITE);
  display.setTextSize(1);

  // Decorative border
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_WHITE);

  
  display.setCursor(10, 12);
  display.println(F("The Tropical Fruit"));
  display.setCursor(52, 24);
  display.println(F("Bowl"));
  display.setCursor(16, 40);
  display.println(F("by The Diat"));
  display.setCursor(37, 50);
  display.println(F("Formation"));
  display.display();

  
  delay(2000);

  
  for (int contrast = 255; contrast >= 0; contrast -= 15) {
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command((uint8_t)contrast);
    delay(35);
  }

  for (int step = 0; step < 8; step++) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
      for (int x = (y + step) % 4; x < SCREEN_WIDTH; x += 4) {
        display.drawPixel(x, y, COLOR_BLACK);
      }
    }
    display.display();
    delay(40);
  }

  display.clearDisplay();
  display.display();

  
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(0xFF);
  delay(200);

  cursorX = SCREEN_WIDTH / 2.0f;
  cursorY = SCREEN_HEIGHT / 2.0f;
  currentState = STATE_MAIN_MENU;
}

// ============================================================================
// Collecting the data
// ============================================================================
void updateSensorsAndHistory() {
  unsigned long now = millis();
  if (now - lastSensorReadTime < SENSOR_READ_INTERVAL) return;
  lastSensorReadTime = now;

  // 1. Read DHT11
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t)) dhtTemp = t;
  if (!isnan(h)) dhtHum  = h;

  // 2. Read ENS160 + AHT21
  if (ahtFound) {
    sensors_event_t humidity, temp;
    aht21.getEvent(&humidity, &temp);
    ahtTemp = temp.temperature;
    ahtHum  = humidity.relative_humidity;
  }
  if (ensFound) {
    readENS160(ensECO2, ensTVOC);
  }

  // 3. Push latest readings into circular graph buffers
  sensor1History[historyIndex] = dhtTemp;
  sensor2History[historyIndex] = (float)ensECO2;
  sensor3History[historyIndex] = mpuTiltMag;

  historyIndex = (historyIndex + 1) % MAX_GRAPH_POINTS;
}

// Updates the gyroscope/accelerometer cursor position based on PCB tilt
void updateGyroCursor() {
  unsigned long now = millis();
  if (now - lastGyroUpdateTime < 20) return;
  lastGyroUpdateTime = now;

  if (mpuFound) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Store overall acceleration/tilt magnitude for Sensor 3 reading window
    mpuTiltMag = sqrt(a.acceleration.x * a.acceleration.x +
                      a.acceleration.y * a.acceleration.y +
                      a.acceleration.z * a.acceleration.z);
    mpuGyroX = g.gyro.x;
    mpuGyroY = g.gyro.y;

    
    float tiltX = a.acceleration.x; 
    float tiltY = a.acceleration.y; 

   
    if (abs(tiltX) > 0.6f) {
      cursorX += (tiltX * 0.35f * GYRO_SENSITIVITY);
    }
    if (abs(tiltY) > 0.6f) {
      cursorY += (tiltY * 0.35f * GYRO_SENSITIVITY);
    }
  }

  
  if (cursorX < CURSOR_RADIUS) cursorX = CURSOR_RADIUS;
  if (cursorX > SCREEN_WIDTH - 1 - CURSOR_RADIUS) cursorX = SCREEN_WIDTH - 1 - CURSOR_RADIUS;
  if (cursorY < CURSOR_RADIUS) cursorY = CURSOR_RADIUS;
  if (cursorY > SCREEN_HEIGHT - 1 - CURSOR_RADIUS) cursorY = SCREEN_HEIGHT - 1 - CURSOR_RADIUS;
}

// Reads the slide potentiometer to enlarge/restore the graph size.
void updateSliderGraphSize() {
  if (currentState == STATE_MAIN_MENU || exitPopupOpen) {
    return; 
  }

  
  int rawSlider = analogRead(PIN_SLIDER);

  
  graphHeight = map(rawSlider, 0, 4095, BASE_GRAPH_H, MAX_GRAPH_H);
  graphHeight = constrain(graphHeight, BASE_GRAPH_H, MAX_GRAPH_H);
}

// ============================================================================
// Screen Rendering
// ============================================================================

// Renders the Main Screen
void renderMainMenu() {
  display.fillScreen(appBackgroundColor);

  // Header Title
  display.setTextSize(1);
  display.setTextColor(appTextColor);
  display.setCursor(25, 8);
  display.println(F("View Readings"));
  display.drawFastHLine(14, 18, 100, defaultBorderColor);

  // Subtitle instruction
  display.setCursor(12, 25);
  display.println(F("Tilt PCB + Press SW2"));

  // 3 Select buttons
  for (int i = 0; i < 3; i++) {
    drawUIButton(mainButtons[i]);
  }
}

// Renders a Sensor Window with numerical text readings, dynamic graph, and Back button
void renderSensorWindow(uint8_t sensorNumber) {
  display.fillScreen(appBackgroundColor);
  display.setTextSize(1);
  display.setTextColor(appTextColor);

  float* activeHistory = nullptr;
  float minVal = 0.0f, maxVal = 100.0f;

  // Print numerical text readings at the top-left
  display.setCursor(2, 2);
  if (sensorNumber == 1) {
    // DHT11 Temperature & Humidity
    display.print(F("DHT:"));
    display.print(dhtTemp, 1);
    display.print(F("C "));
    display.print(dhtHum, 0);
    display.print(F("%"));
    activeHistory = sensor1History;
    minVal = 10.0f;
    maxVal = 45.0f;
  } else if (sensorNumber == 2) {
    // ENS160 Air Quality + AHT21
    display.print(F("CO2:"));
    display.print(ensECO2);
    display.print(F("p T:"));
    display.print(ahtTemp, 0);
    display.print(F("C"));
    activeHistory = sensor2History;
    minVal = 350.0f;
    maxVal = 1500.0f;
  } else {
    // MPU6050 Gyro/Accel Readings
    display.print(F("IMU:"));
    display.print(mpuTiltMag, 1);
    display.print(F("m/s2"));
    activeHistory = sensor3History;
    minVal = 0.0f;
    maxVal = 20.0f;
  }

  // Draw the "Back" button in the top-right corner
  drawUIButton(backButton);

  
  int graphX = 6;
  int graphW = encoderXLength;
  int graphH = graphHeight;
  int graphY = SCREEN_HEIGHT - graphH - 3; 

  
  display.drawRect(graphX, graphY, graphW, graphH, defaultBorderColor);

  
  int numPointsToPlot = min(MAX_GRAPH_POINTS, graphW - 2);
  if (numPointsToPlot > 2 && activeHistory != nullptr) {
    for (int i = 0; i < numPointsToPlot - 1; i++) {
      int idx1 = (historyIndex - numPointsToPlot + i + MAX_GRAPH_POINTS) % MAX_GRAPH_POINTS;
      int idx2 = (historyIndex - numPointsToPlot + i + 1 + MAX_GRAPH_POINTS) % MAX_GRAPH_POINTS;

      float v1 = constrain(activeHistory[idx1], minVal, maxVal);
      float v2 = constrain(activeHistory[idx2], minVal, maxVal);

      
      int x1 = graphX + 1 + map(i, 0, numPointsToPlot - 1, 0, graphW - 3);
      int x2 = graphX + 1 + map(i + 1, 0, numPointsToPlot - 1, 0, graphW - 3);

      
      int y1 = (graphY + graphH - 2) - (int)((v1 - minVal) / (maxVal - minVal) * (graphH - 4));
      int y2 = (graphY + graphH - 2) - (int)((v2 - minVal) / (maxVal - minVal) * (graphH - 4));

      display.drawLine(x1, y1, x2, y2, graphLineColor);
    }
  }
}

// Renders exit screen
void renderExitPopup() {
  
  int popX = 10, popY = 10, popW = 108, popH = 46;
  display.fillRect(popX, popY, popW, popH, appBackgroundColor);
  display.drawRect(popX, popY, popW, popH, defaultBorderColor);
  display.drawRect(popX + 2, popY + 2, popW - 4, popH - 4, defaultBorderColor);

  display.setTextSize(1);
  display.setTextColor(appTextColor);
  display.setCursor(18, 16);
  display.println(F("Do you want to"));
  display.setCursor(48, 26);
  display.println(F("exit?"));

  // Draws the yes and no buttons
  drawUIButton(popupYesButton);
  drawUIButton(popupNoButton);
}

// ============================================================================
// Low profile switch input handlings
// ============================================================================
void handleSwitches() {
  unsigned long now = millis();

  
  bool sw1Reading = digitalRead(PIN_SW1);
  if (sw1Reading == LOW && lastSw1State == HIGH && (now - lastSw1Debounce > DEBOUNCE_DELAY)) {
    lastSw1Debounce = now;

    if (currentState == STATE_OFF) {
      
      currentState = STATE_INTRO;
    } else if (currentState != STATE_INTRO) {
      
      exitPopupOpen = true;
    }
  }
  lastSw1State = sw1Reading;

  
  bool sw2Reading = digitalRead(PIN_SW2);
  if (sw2Reading == LOW && lastSw2State == HIGH && (now - lastSw2Debounce > DEBOUNCE_DELAY)) {
    lastSw2Debounce = now;

    if (currentState == STATE_OFF || currentState == STATE_INTRO) {
      lastSw2State = sw2Reading;
      return;
    }

    // yes or no buttons
    if (exitPopupOpen) {
      if (isCursorOverButton(popupYesButton)) {
        
        exitPopupOpen = false;
      } else if (isCursorOverButton(popupNoButton)) {
        
        exitPopupOpen = false;
        currentState  = STATE_OFF;
        display.clearDisplay();
        display.display();
      }
      lastSw2State = sw2Reading;
      return;
    }

    
    if (currentState == STATE_MAIN_MENU) {
      if (isCursorOverButton(mainButtons[0])) {
        currentState = STATE_SENSOR_1;
      } else if (isCursorOverButton(mainButtons[1])) {
        currentState = STATE_SENSOR_2;
      } else if (isCursorOverButton(mainButtons[2])) {
        currentState = STATE_SENSOR_3;
      }
    }
    
    else if (currentState == STATE_SENSOR_1 ||
             currentState == STATE_SENSOR_2 ||
             currentState == STATE_SENSOR_3) {
      if (isCursorOverButton(backButton)) {
        currentState = STATE_MAIN_MENU;
      }
    }
  }
  lastSw2State = sw2Reading;
}

// ============================================================================
// Main Loop
// ============================================================================
void setup() {
  Serial.begin(115200);

  
  pinMode(PIN_SW1, INPUT_PULLUP);
  pinMode(PIN_SW2, INPUT_PULLUP);
  pinMode(PIN_ENC1, INPUT_PULLUP);
  pinMode(PIN_ENC2, INPUT_PULLUP);
  pinMode(PIN_SLIDER, INPUT);

  
  attachInterrupt(digitalPinToInterrupt(PIN_ENC1), updateRotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC2), updateRotaryEncoder, CHANGE);

  // Initialize I2C Bus on XIAO D4
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

  // Initialize 128x64 SSD1306 OLED Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
    Serial.println(F("SSD1306 OLED allocation failed!"));
  }
  display.clearDisplay();
  display.display(); // Keep screen blank until Switch 1 is pressed the first time

  // Initialize DHT11 Sensor
  dht.begin();

  // Initialize MPU6050 
  if (mpu.begin()) {
    mpuFound = true;
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  } else {
    Serial.println(F("MPU6050 not detected on I2C."));
  }

  // Initialize AHT21 + ENS160 Sensor 
  if (aht21.begin()) {
    ahtFound = true;
  }
  ensFound = initENS160();

  
  setAppBackgroundColor(appBackgroundColor);
}

void loop() {
  
  handleSwitches();

  
  if (currentState == STATE_OFF) {
    delay(20);
    return;
  }

  
  if (currentState == STATE_INTRO) {
    playIntroAndFadeToApp();
    return;
  }

  
  updateGyroCursor();
  updateSensorsAndHistory();
  updateSliderGraphSize();

  // Render the active window
  if (currentState == STATE_MAIN_MENU) {
    renderMainMenu();
  } else if (currentState == STATE_SENSOR_1) {
    renderSensorWindow(1);
  } else if (currentState == STATE_SENSOR_2) {
    renderSensorWindow(2);
  } else if (currentState == STATE_SENSOR_3) {
    renderSensorWindow(3);
  }

  
  if (exitPopupOpen) {
    renderExitPopup();
  }

  
  drawGyroCursor();

  display.display();
}
