//================================
// Definitions
//================================

#define BTN1PIN              8
#define BTN2PIN              9

#define LONGPRESSTIME      500

#define SENSORPIN           A0

#define LEDPIN              12
#define NUMPIXPERPANEL       6
#define NUMPANELS           10
#define NUMPIXELS           NUMPANELS * NUMPIXPERPANEL

#define BRIGHTNESSSTEPS      8

//================================
// Constants
//================================

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

//================================
// Enums
//================================

enum modes
{
  LIGHT,
  CLOCK,
  EFFECTS,
  SETTINGS
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