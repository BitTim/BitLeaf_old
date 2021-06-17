//================================
// Clock Mode
//================================

int  subMode           = CLOCK_CLOCK;
int  prevSubMode       = CLOCK_CLOCK;
int  subModeBeforeTime = CLOCK_CLOCK;

RtcDateTime dt;
int  currSetting  = 0;
int  settingTimer = 0;
bool blinkOn      = false;

int settingTime[6] = {0};
int maxSettingTime[6] = {2100, 12, 31, 23, 59, 59};
int minSettingTime[6] = {2000,  1,  1,  0,  0,  0}; 
int currSettingTime = 0;

// Functions
void changeSubMode(int newSubMode)
{
  clearLamp();
  prevSubMode = subMode;
  subMode = newSubMode;

  pixels.show();
}

void changeSetting(int id)
{
  switch(id)
  {
  case CLOCK_TIME:
    settingTime[0] = dt.Year();
    settingTime[1] = dt.Month();
    settingTime[2] = dt.Day();
    settingTime[3] = dt.Hour();
    settingTime[4] = dt.Minute();
    settingTime[5] = dt.Second();

    subModeBeforeTime = prevSubMode;
    changeSubMode(CLOCK_CHANGETIME);
    break;
    
  case CLOCK_12HOUR:
    clock_12hour = !clock_12hour;
    break;
    
  case CLOCK_FILLLIGHT:
    clock_fillLightOff = !clock_fillLightOff;
    break;
    
  case CLOCK_FILLOFF:
    clock_fillOffBits = !clock_fillOffBits;
    break;
    
  case CLOCK_AUTOBRIGHTNESS:
    clock_autobrightness = !clock_autobrightness;
    break;
  }
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
  if(clock_12hour)
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

void changeTime(int btnMap)
{ 
  if((btnMap >> 0) & 0x1) settingTime[currSettingTime] += 1; if(settingTime[currSettingTime] > maxSettingTime[currSettingTime]) settingTime[currSettingTime] = minSettingTime[currSettingTime];
  if((btnMap >> 1) & 0x1)
  {
    dt = RtcDateTime(settingTime[0], settingTime[1], settingTime[2], settingTime[3], settingTime[4], settingTime[5]);
    rtc.SetDateTime(dt);
    delay(10);
    changeSubMode(CLOCK_SETTINGS);
  }
  if((btnMap >> 2) & 0x1) currSettingTime += 1; if(currSettingTime >= 6) currSettingTime = 0;
  
  Col yearCol  = clockAMShades[0];
  Col monthCol = clockAMShades[1];
  Col dayCol   = clockAMShades[2];

  Col hourCol  = clockPMShades[0];
  Col minCol   = clockPMShades[1];
  Col secCol   = clockPMShades[2];

  Col offCol   = Col(  0,   0,   0);

  clearLamp();

  if(currSettingTime == 0)
  {
    binaryDisplay(settingTime[0], 3, 4, yearCol, offCol, 12);
  }
  else if(currSettingTime == 1 || currSettingTime == 2)
  {
    binaryDisplay(settingTime[1], 5, 2, monthCol, offCol);
    binaryDisplay(settingTime[2], 3, 2, dayCol, offCol);
  }
  else
  {
    binaryDisplay(settingTime[3], 6, 2, hourCol, offCol);
    binaryDisplay(settingTime[4], 4, 2, minCol , offCol);
    binaryDisplay(settingTime[5], 2, 2, secCol , offCol);
  }
}

void modeClockClock(int btnMap)
{
  if((btnMap >> 0) & 0x1) changeSubMode(CLOCK_LIGHT);

  Col white = Col(236, 239, 244);
  Col off   = Col(  0,   0,   0);

  setLampColor(off);

  if(clock_fillOffBits) off = changeBrightness(white, 0.25);
  displayClock(off, 1.0);
}

void modeClockLight(int btnMap)
{
  if((btnMap >> 0) & 0x1) changeSubMode(CLOCK_NIGHT);

  Col white = Col(236, 239, 244);
  Col off   = Col(  0,   0,   0);

  setLampColor(white);

  if(clock_fillLightOff) off = changeBrightness(white, 0.25);
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
  int cTime = millis();
  if(prevSubMode == CLOCK_CHANGETIME) prevSubMode = subModeBeforeTime;
  
  if((btnMap >> 3) & 0x1) changeSubMode(prevSubMode);
  if((btnMap >> 2) & 0x1)
  {
    currSetting++;
    if(currSetting >= numClockSettings) currSetting = 0;
    settingTimer = cTime;
    blinkOn = false;
  }
  if((btnMap >> 0) & 0x1)
  {
    changeSetting(currSetting);
    settingTimer = cTime;
    blinkOn = false;
  }

  for(int i = 0; i < numClockSettings; i++)
  {
    switch(i)
    {
    case CLOCK_12HOUR:
      if(clock_12hour) setPanel(i, trueCol);
      else setPanel(i, falseCol);
      break;

    case CLOCK_FILLLIGHT:
      if(clock_fillLightOff) setPanel(i, trueCol);
      else setPanel(i, falseCol);
      break;

    case CLOCK_FILLOFF:
      if(clock_fillOffBits) setPanel(i, trueCol);
      else setPanel(i, falseCol);
      break;

    case CLOCK_AUTOBRIGHTNESS:
      if(clock_autobrightness) setPanel(i, trueCol);
      else setPanel(i, falseCol);
      break;

    default:
      setPanel(i, miscCol);
      break;
    }
  }

  if(cTime >= settingTimer + BLINKDELAY)
  {
    settingTimer = cTime;
    blinkOn = !blinkOn;
  }

  if(!blinkOn) setPanel(currSetting, Col(0));
}

void modeClock(int btnMap)
{
  if(((btnMap >> 2) & 0x1) && subMode != CLOCK_SETTINGS && subMode != CLOCK_CHANGETIME) changeMode(LIGHT);
  if(((btnMap >> 1) & 0x1) && subMode != CLOCK_SETTINGS && subMode != CLOCK_CHANGETIME) changeSubMode(CLOCK_SETTINGS);

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

    case CLOCK_CHANGETIME:
      changeTime(btnMap);
      break;
  }
}
