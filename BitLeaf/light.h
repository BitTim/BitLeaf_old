//================================
// Light Mode
//================================

int  lampBrightness = BRIGHTNESSSTEPS / 2;

bool  timerActive   = false;
int   timerLength   = 750;
int   timer         = 0;

int   motionTimer   = 0;
int   motionPanel   = 0;
int   motionStage   = 0;
float motionBright  = 1;
int   motionFade    = 0;
int   motionLow     = 0;
int   motionDelay   = 100;

float m             = 0;

//Settings
bool lightAsutoBrightness   = false;
bool slightMotion     = true;
int  lightTemperature = 0;

//Funtions
void modeLight(int btnMap)
{
  if((btnMap >> 2) & 0x1) changeMode(CLOCK);
  if((btnMap >> 3) & 0x1) changeSubMode(LIGHT_SETTINGS);
  if((btnMap >> 0) & 0x1)
  {
    lampBrightness += 1;
    if(lampBrightness >= BRIGHTNESSSTEPS) lampBrightness = 0;

    clearLamp();

    timerActive = true;
    timer = millis();
  }
  
  Col white = changeBrightness(Col(255), (lampBrightness + 1) * float(1.0f / BRIGHTNESSSTEPS));

  if(timerActive)
  {
    progressDisplay(lampBrightness, white);
    if(millis() - timer >= timerLength) timerActive = false;
  }
  else if(slightMotion)
  {
    fillLamp(white);
    int cTime = millis();
   
    switch(motionStage)
    {    
      case MOTION_HIGH:
        if(cTime >= motionTimer + motionDelay)
        {
          motionTimer = cTime;
          motionPanel = random(0, NUMPANELS);
          motionFade  = random(MINFADINGTIME, MAXFADINGTIME + 1);
          motionDelay = random(0, MAXSTATICTIME + 1);
          motionLow   = random(0, MAXFADEDOUTTIME + 1);

          m = 0.5 / motionFade;
          motionStage = MOTION_DECAY;
        }
        break;
        
      case MOTION_DECAY:
        motionBright = -m * (cTime - motionTimer) + 1;
        setPanel(motionPanel, changeBrightness(white, motionBright));

        if(motionBright <= 0.5)
        {
          motionTimer = cTime;
          motionStage = MOTION_LOW;
        }
        break;
      
      case MOTION_LOW:
        setPanel(motionPanel, changeBrightness(white, 0.5));
      
        if(cTime >= motionTimer + motionLow)
        {
          motionTimer = cTime;
          motionStage = MOTION_RISE;
        }
        break;
      
      case MOTION_RISE:
        motionBright = m * (cTime - motionTimer) + 0.5;
        setPanel(motionPanel, changeBrightness(white, motionBright));

        if(motionBright >= 1)
        {
          motionBright = 1;
          motionTimer = cTime;
          motionStage = MOTION_HIGH;
        }
        break;
    }
  }
  else fillLamp(white);
}
