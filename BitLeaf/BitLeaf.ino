#include <Adafruit_NeoPixel.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

// ================================
// Constants
// ================================

#define BTN1PIN              8
#define BTN2PIN              9

#define LONGPRESSTIME      500

#define SENSORPIN           A0

#define LEDPIN              12
#define NUMPIXPERPANEL       6
#define NUMPANELS           10
#define NUMPIXELS           NUMPANELS * NUMPIXPERPANEL

#define BRIGHTNESSSTEPS      8

int panelIDs[NUMPANELS][NUMPIXPERPANEL] = {
  { 1,  0, 59, 58,  2,  3},
  {57, 56,  4,  5, 54, 55},
  { 7,  6, 53, 52,  8,  9},
  {51, 50, 10, 11, 12, 13},
  {49, 48, 47, 46, 14, 15},
  {45, 44, 16, 17, 18, 19},
  {43, 42, 41, 40, 20, 21},
  {39, 38, 22, 23, 36, 37},
  {25, 24, 35, 34, 26, 27},
  {33, 32, 28, 29, 30, 31}
};

enum modes
{
  LIGHT,
  CLOCK,
  EFFECTS,
  SETTINGS
};

// ================================
// Structures
// ================================

struct Col
{
  int r = 0;
  int g = 0;
  int b = 0;

  Col()                    {this->r = 0; this->g = 0; this->b = 0;}
  Col(int a)               {this->r = a; this->g = a; this->b = a;}
  Col(int r, int g, int b) {this->r = r; this->g = g; this->b = b;}
};

// ================================
// Variables
// ================================


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

ThreeWire rtcWire(3, 2, 4); //DAT, CLK, RST
RtcDS1302<ThreeWire> rtc(rtcWire);
RtcDateTime dt;

int mode = CLOCK;

int  btnPins[2]            = {BTN1PIN, BTN2PIN};
bool btnJustPressed[2]     = {false, false};
bool btnJustLongPressed[2] = {false, false};
int  btnTimer[2]           = {0, 0};

int prevBtnMap = 0;

Col clockHourCol  = Col(156, 255,  13);
Col clockMinCol   = Col(255, 236,  25);
Col clockSecCol   = Col(255, 186,  13);
Col clockOffCol = Col(  0,   0,   0);

Col clockAMCol  = Col( 51, 213, 255);
Col clockPMCol  = Col(255,   0,  88);

int lampBrightness = 3;

// ================================
// Settings
// ================================

int brightness = 3;
bool twelveHour = true;

int lampOnHour = 17;
int lampOffHour = 21;

// ================================
// Main
// ================================

void setup() {
  rtc.Begin();
  rtc.SetIsRunning(true);
  rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
  
  pixels.begin();
  pixels.setBrightness(brightness);

  pinMode(BTN1PIN, INPUT_PULLUP);
  pinMode(BTN2PIN, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  int btnMap = getBtnMap();
  
  if(btnMap == prevBtnMap) btnMap = 0;
  else prevBtnMap = btnMap;
  
  runMode(btnMap);
  pixels.show();
}

// ================================
// Modes
// ================================

void runMode(int btnMap)
{
  switch(mode)
  {
    case LIGHT:
      modeLight(btnMap);
      break;
  
    case CLOCK:
      modeClock(btnMap);
      break;
  
    case SETTINGS:
      modeSettings(btnMap);
      break;
  
    case EFFECTS:
      modeEffects(btnMap);
      break;
  }
}

void modeLight(int btnMap)
{
  if((btnMap >> 2) & 0x1) changeMode(CLOCK);
  if((btnMap >> 0) & 0x1)
  {
    lampBrightness += 1;
    if(lampBrightness >= BRIGHTNESSSTEPS) lampBrightness = 0;
  }
  
  Col white = Col((256 / BRIGHTNESSSTEPS) * lampBrightness - 1);
  fillLamp(white.r, white.g, white.b);
}

void modeClock(int btnMap)
{
  if((btnMap >> 2) & 0x1) changeMode(LIGHT);
  
  dt = rtc.GetDateTime();

  if(twelveHour)
  {
    int hour = dt.Hour();
    bool pm = false;
    
    if(hour > 11) pm = true;
    else pm = false;
    
    if(hour > 12) hour -= 12;
    if(hour == 0) hour  = 12;
    
    binaryDisplay(hour, 6, 2, pm ? clockPMCol : clockAMCol, clockOffCol);
  }
  else binaryDisplay(dt.Hour(), 6, 2, clockHourCol, clockOffCol);
  
  binaryDisplay(dt.Minute(), 4, 2, clockMinCol, clockOffCol);
  binaryDisplay(dt.Second(), 2, 2, clockSecCol, clockOffCol);

  if(lampOffHour < lampOnHour) lampOffHour += 24;
  if(dt.Hour() > lampOnHour && dt.Hour() < lampOffHour)
  {
    Col white = Col(255);

    setPanel(0, white.r, white.g, white.b);
    setPanel(1, white.r, white.g, white.b);
    setPanel(8, white.r, white.g, white.b);
    setPanel(9, white.r, white.g, white.b);
  }

  if(lampOffHour > 23) lampOffHour -= 24;
  if(dt.Hour() >= lampOffHour)
  {
    Col white = Col(0);

    setPanel(0, white.r, white.g, white.b);
    setPanel(1, white.r, white.g, white.b);
    setPanel(8, white.r, white.g, white.b);
    setPanel(9, white.r, white.g, white.b);
  }
}

void modeSettings(int btnMap)
{
  
}

void modeEffects(int btnMap)
{

}

// ================================
// Util
// ================================

void binaryDisplay(int num, int panelOffset, int numPanels, Col on, Col off)
{
  Col col;
  int resolution = (numPanels * NUMPIXPERPANEL) / 6;
  
  for(int panel = 0; panel < numPanels; panel++)
  {
    for(int digit = 0; digit < 6 / resolution; digit++)
    {
      if(num & 0x1 == 1) col = on;
      else col = off;
      
      for(int i = 0; i < resolution; i++)
      {
        setPixel(panelIDs[panel + panelOffset][digit * resolution + i], col.r, col.g, col.b);
      }

      num = num >> 1;
    }
  }
}

int getBtnMap()
{
  int btnMap = 0b0000;

  for(int i = 0; i < 2; i++)
  {
    btnMap = btnMap << 2;
    
    if(digitalRead(btnPins[i]) == LOW)
    {
      if(!btnJustPressed[i]) {btnJustPressed[i] = true; btnTimer[i] = millis();}
      if((millis() - btnTimer[i] > LONGPRESSTIME) && (btnJustLongPressed[i] == false)) btnMap |= 0b10;
    }
    else
    {
      if(btnJustPressed[i])
      {
        if(btnJustLongPressed[i]) btnJustLongPressed[i] = false;
        else btnMap |= 0b01;

        btnJustPressed[i] = false;
      }
    }
  }

  return btnMap;
}

// ================================
// Helper
// ================================

void changeMode(int newMode)
{
  clearLamp();
  mode = newMode;

  pixels.show();
}

void setPixel(int id, int r, int g, int b)
{
  pixels.setPixelColor(id, pixels.Color(r, g, b));
}

void setPanel(int id, int r, int g, int b)
{
  for(int i = 0; i < NUMPIXPERPANEL; i++)
  {
    setPixel(panelIDs[id][i], r, g, b);
  }
}

void clearLamp()
{
  fillLamp(0, 0, 0);
}

void fillLamp(int r, int g, int b)
{
  for(int i = 0; i < NUMPIXELS; i++)
  {
    setPixel(i, r, g, b);
  }
}
