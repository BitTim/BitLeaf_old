//================================
// Definitions
//================================

#include <Adafruit_NeoPixel.h>

#define BTN1PIN               8
#define BTN2PIN               9

#define LONGPRESSTIME         500

#define SENSORPIN             A0

#define LEDPIN                12
#define NUMPIXPERPANEL        6
#define NUMPANELS             10
#define NUMPIXELS             NUMPANELS * NUMPIXPERPANEL

#define BRIGHTNESSSTEPS       8
#define MAXSTATICTIME         10000
#define MAXFADEDOUTTIME       7500
#define MINFADINGTIME         500
#define MAXFADINGTIME         2000

#define BLINKDELAY            250

//================================
// Enums
//================================

enum mainModes
{
  CLOCK,
  LIGHT,
  EFFECTS
};

enum clockModes
{
  CLOCK_CLOCK,
  CLOCK_LIGHT,
  CLOCK_NIGHT,
  CLOCK_SETTINGS,
  CLOCK_CHANGETIME
};

enum lightModes
{
  LIGHT_LIGHT,
  LIGHT_SETTINGS
};

enum lightMotionStages
{
  MOTION_HIGH,
  MOTION_DECAY,
  MOTION_LOW,
  MOTION_RISE
};

enum settingIDs
{
  CLOCK_TIME,
  CLOCK_12HOUR,
  CLOCK_FILLLIGHT,
  CLOCK_FILLOFF,
  CLOCK_AUTOBRIGHTNESS,
  LIGHT_MOTION,
  LIGHT_TEMPERATURE,
  LIGHT_AUTOBRIGHTNESS
};

//================================
// Structures
//================================

struct Col
{
  int r = 0;
  int g = 0;
  int b = 0;

  Col()                    {this->r = 0; this->g = 0; this->b = 0;}
  Col(int a)               {this->r = a; this->g = a; this->b = a;}
  Col(int r, int g, int b) {this->r = r; this->g = g; this->b = b;}
};

struct Packet
{
  byte header[2]= {0};
  byte len = 0;
  byte command = 0;
  byte args[12] = {0};
  
  Packet() {}
  Packet(byte* buf)
  {
    memcpy(&header, buf, 2);
    len = buf[2];
    command = buf[3];
    memcpy(&args, buf + 4, len);
  }
};

//================================
// Constants
//================================

int defaultMode = CLOCK;
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

Col clockShades[3]   = {Col(163, 190, 140), Col(143, 188, 187), Col(136, 192, 208)};
Col clockAMShades[3] = {Col(204, 123,  94), Col(214, 112,  99), Col(191,  97, 106)};
Col clockPMShades[3] = {Col( 94, 129, 172), Col( 97, 166, 194), Col( 92, 181, 184)};

Col trueCol  = Col(163, 190, 140);
Col falseCol = Col(191,  97, 106);
Col miscCol  = Col(129, 161, 193);

float clockNightBrightness = 0.2f;

//================================
// Variables
//================================

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

ThreeWire rtcWire(3, 2, 4); //DAT, CLK, RST
RtcDS1302<ThreeWire> rtc(rtcWire);

int mode = CLOCK;

int  btnPins[2]            = {BTN1PIN, BTN2PIN};
bool btnJustPressed[2]     = {false, false};
bool btnJustLongPressed[2] = {false, false};
int  btnTimer[2]           = {0, 0};

int prevBtnMap = 0;

//================================
// Settings
//================================

int  numClockSettings     = 5; //5th is time
bool clock_12hour         = false;
bool clock_autobrightness = false;
bool clock_fillLightOff   =  true;
bool clock_fillOffBits    = false;

int  numLightSettings     = 3;
bool light_autobrightness = false;
bool light_slightMotion   = true;
int  light_temperature    = 0;
