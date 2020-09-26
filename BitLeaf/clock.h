//================================
// Clock Mode
//================================

int  subMode     = CLOCK_CLOCK;
int  prevSubMode = CLOCK_CLOCK;

RtcDateTime dt;

// Settings
bool twelveHour     = false;
bool clockAutoBrightness = false;
bool fillLightOff   =  true;
bool fillOffBits    = false;

// Functions
void changeSubMode(int newSubMode)
{
  clearLamp();
  prevSubMode = subMode;
  subMode = newSubMode;

  pixels.show();
}

void setLampColor(Col col)
{
  setPanel(0, col);
  setPanel(1, col);
  setPanel(8, col);
  setPanel(9, col);
}

void displayClock(Col offCol, float brightness)
{
  dt = rtc.GetDateTime();

  Col hourCol = clockShades[0];
  Col minCol  = clockShades[1];
  Col secCol  = clockShades[2];

  int hour = dt.Hour();
  if(twelveHour)
  {
    bool pm = false;
    
    if(hour > 11) pm = true;
    else pm = false;
    
    if(hour > 12) hour -= 12;
    if(hour == 0) hour  = 12;

    if(pm)
    {
      hourCol = clockPMShades[0];
      minCol  = clockPMShades[1];
      secCol  = clockPMShades[2];
    }
    else
    {
      hourCol = clockAMShades[0];
      minCol  = clockAMShades[1];
      secCol  = clockAMShades[2];
    }
  }

  hourCol = changeBrightness(hourCol, brightness);
  minCol  = changeBrightness(minCol,  brightness);
  secCol  = changeBrightness(secCol,  brightness);

  binaryDisplay(hour       , 6, 2, hourCol, offCol);
  binaryDisplay(dt.Minute(), 4, 2, minCol , offCol);
  binaryDisplay(dt.Second(), 2, 2, secCol , offCol);
}

void modeClockClock(int btnMap)
{
  if((btnMap >> 0) & 0x1) changeSubMode(CLOCK_LIGHT);

  Col white = Col(236, 239, 244);
  Col off   = Col(  0,   0,   0);

  setLampColor(off);

  if(fillOffBits) off = changeBrightness(white, 0.25);
  displayClock(off, 1.0);
}

void modeClockLight(int btnMap)
{
  if((btnMap >> 0) & 0x1) changeSubMode(CLOCK_NIGHT);

  Col white = Col(236, 239, 244);
  Col off   = Col(  0,   0,   0);

  setLampColor(white);

  if(fillLightOff) off = changeBrightness(white, 0.25);
  displayClock(off, 1.0);
}

void modeClockNight(int btnMap)
{
  if((btnMap >> 0) & 0x1) changeSubMode(CLOCK_CLOCK);

  Col white = Col(236, 239, 244);
  Col off   = Col(  0,   0,   0);

  setLampColor(off);
  displayClock(off, clockNightBrightness);
}

void modeClockSettings(int btnMap)
{
  if((btnMap >> 0) & 0x1) changeSubMode(prevSubMode);
}

void modeClock(int btnMap)
{
  if((btnMap >> 2) & 0x1) changeMode(LIGHT);
  if((btnMap >> 1) & 0x1) changeSubMode(CLOCK_SETTINGS);

  switch(subMode)
  {
    case CLOCK_CLOCK:
      modeClockClock(btnMap);
      break;

    case CLOCK_LIGHT:
      modeClockLight(btnMap);
      break;

    case CLOCK_NIGHT:
      modeClockNight(btnMap);
      break;

    case CLOCK_SETTINGS:
      modeClockSettings(btnMap);
      break;
  }
}
