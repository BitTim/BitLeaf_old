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
  CLOCK_SETTINGS
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
  byte header[2];
  byte length;
  byte command;
  byte ags[12];
  
  Packet()             {header = {0}; length =0; command = 0; args = {0};}
  Packet(byte* buffer)
  {
    memcpy(*header, *buffer, 2);
    length = buffer[2];
    command = buffer[3];
    memcpy(*args, *buffer + 4, 12);
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
