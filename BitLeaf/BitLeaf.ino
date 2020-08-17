#include <Adafruit_NeoPixel.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

// ================================
// Constants
// ================================

#define BTN1PIN              7
#define BTN2PIN              8

#define LONGPRESSTIME      250

#define SENSORPIN           A0

#define LEDPIN               5
#define NUMPIXPERPANEL       6
#define NUMPANELS           10
#define NUMPIXELS           NUMPANELS * NUMPIXPERPANEL

const int panelIDs[] = {
  { 0,  1,  2,  3, 58, 59},
  { 4,  5, 54, 55, 56, 57},
  { 6,  7,  8,  9, 52, 53},
  {10, 11, 48, 49, 50, 51},
  {12, 13, 14, 15, 46, 47},
  {16, 17, 42, 43, 44, 45},
  {18, 19, 20, 21, 40, 41},
  {22, 23, 36, 37, 38, 39},
  {24, 25, 26, 27, 34, 35},
  {28, 29, 30, 31, 32, 33}
};

const int hourPanels[2] =   {6, 7};
const int minutePanels[2] = {4, 5};
const int secondPanels[2] = {2, 3};

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

  Col()        {this->r = 0; this->g = 0; this->b = 0;}
  Col(r, g, b) {this->r = r; this->g = g; this->b = b;}
};

// ================================
// Variables
// ================================


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_RGB + NEO_KHZ800);

ThreeWire rtcWire(3, 2, 4); //DAT, CLK, RST
RtcDS1302<ThreeWire> rtc(rtcWire);
RtcDateTime dt;

bool btn1State = false;
bool btn2State = false;

bool btn1LongState = false;
bool btnLong2State = false;

int btn1Timer = 0;
int btn2Timer = 0;

int mode = LIGHT;

int binHour = 0;
int binMinute = 0;
int binSecond = 0;

// ================================
// Settings
// ================================

int brightness = 100;
bool twelveHour = false;

Col clockCol = Col(255, 255, 255);

// ================================
// Main
// ================================

void setup() {
  rtc.Begin();
  rtc.SetIsRunning(true);

  pixels.begin();
}

void loop() {
  switch(mode)
  {
  case LIGHT:
    modeLight();
    break;
  case CLOCK:
    modeClock();
    break;
  case SETTINGS:
    modeSettings();
    break;
  case EFFECTS:
    modeEffects();
    break;
  }
  
  pixels.show();
}

// ================================
// Modes
// ================================

void modeLight()
{
  if(digitalRead(BTN1PIN) == HIGH)
  {
    if(btn1State == false)
    {
      btn1State = true;
      btn1Timer = millis();
    }
    
    if((millis() - btn1Timer > LONGPRESSTIME) && (btn1longState == false))
    {
      btn1LongState = true;
      //Longpress
    }
  }
  else
  {
    if(btn1State == true)
    {
      if(btn1LongState == true)
      {
        btn1LongState = false;
      }
      else
      {
        //Short Press
      }
      
      btn1State = false;
    }
  }
}

void modeClock()
{
  dt = rtc.GetDateTime();
  
  binaryDisplay(dt.Hour()  , hourPanels  , 2, 3);
  binaryDisplay(dt.Minute(), MinutePanels, 2, 3);
  binaryDisplay(dt.Second(), SecondPanels, 2, 3);
  
  
}

void modeSettings()
{
  
}

void modeEffects()
{
  
}

// ================================
// Util
// ================================

void binaryDisplay(int num, int *panels, int numPanels, int res)
{
  for(int panel = 0; panel < numPanels; panel++)
  {
    for(int pixel = 0; pixel < res; pixel++)
    {
      if(num & 0x1 == 1) Col col = clockCol;
      else Col col = Col();
      
      for(int i = 0; i < NUMPIXPERPANEL / res; i++)
      {
        setPixel(panelIDs[panels[panel]][pixel + i], col.r, col.g, col.b);
      }

      num = num >> 1;
    }
  }
}

void setPixel(id, r, g, b)
{
  pixels.setPixelColor(id, pixels.Color(r, g, b));
}

void setPanel(id, r, g, b)
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

void fillLamp(r, g, b)
{
  for(int i = 0; i < NUMPIXELS; i++)
  {
    setPixel(i, r, g, b);
  }
}
