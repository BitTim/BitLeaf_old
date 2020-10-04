//================================
// Main Code
//================================

#include <Adafruit_NeoPixel.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

#include "definitions.h"
#include "displayHandler.h"
#include "clock.h"
#include "light.h"

// ================================
// Main
// ================================

void setup() {
  rtc.Begin();
  rtc.SetIsRunning(true);
  
  pixels.begin();
  pixels.setBrightness(255);
  
  Serial.begin(9600);

  pinMode(BTN1PIN, INPUT_PULLUP);
  pinMode(BTN2PIN, INPUT_PULLUP);

  mode = defaultMode;
  randomSeed(analogRead(A3));
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
  
    case EFFECTS:
      modeEffects(btnMap);
      break;
  }
}

void modeSettings(int btnMap)
{
  if((btnMap >> 2) & 0x1) changeMode(defaultMode);
}

void modeEffects(int btnMap)
{

}

// ================================
// Util
// ================================

int getBtnMap()
{
  int btnMap = 0b0000;

  for(int i = 0; i < 2; i++)
  {
    btnMap = btnMap << 2;
    
    if(digitalRead(btnPins[i]) == LOW)
    {
      if(!btnJustPressed[i]) {btnJustPressed[i] = true; btnTimer[i] = millis();}
      int cTime = millis();
      
      if((cTime - btnTimer[i] > LONGPRESSTIME) && (btnJustLongPressed[i] == false))
      {btnJustLongPressed[i] = true;
        btnMap |= 0b10;
      }
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

Packet receivePacket()
{
  byte packetBuffer[16];
  Packet packet;
  int i = 0;

  while(Serial.available())
  {
    packetBuffer[i++] = Serial.read();
  }
  
  if(packetBuffer[0] == 'L') packet = Packet(packetBuffer);
  return packet;
}

void changeMode(int newMode, Adafruit_NeoPixel pixels)
{
  clearLamp();
  mode = newMode;

  pixels.show();
}

void* getSettingFromID(byte id)
{
  switch(id)
  {
  case SETTING_12HOUR:
    break;
  }
}