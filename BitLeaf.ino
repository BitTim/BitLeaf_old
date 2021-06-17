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
  
  Serial.begin(38400);

  pinMode(BTN1PIN, INPUT_PULLUP);
  pinMode(BTN2PIN, INPUT_PULLUP);

  mode = defaultMode;
  randomSeed(analogRead(A3));
}

void loop() {
  int btnMap = getBtnMap();
  Packet packet = receivePacket();
  
  if(btnMap == prevBtnMap) btnMap = 0;
  else prevBtnMap = btnMap;

  if(packet.header[1] == 'S') changePacketSetting(packet);
  
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

void changeMode(int newMode, Adafruit_NeoPixel pixels)
{
  clearLamp();
  mode = newMode;

  pixels.show();
}

// ================================
// Settings
// ================================

void printPacket(Packet packet)
{
  Serial.print("Header: ");
  Serial.print(packet.header[0], HEX);
  Serial.print(" ");
  Serial.println(packet.header[1], HEX);

  Serial.print("Command: ");
  Serial.println(packet.command, HEX);

  Serial.print("Length: ");
  Serial.println(packet.len, HEX);

  Serial.print("Arguments: ");
  for(int i = 0; i < packet.len; i++) Serial.print(String(packet.args[i], HEX) + " ");
  Serial.println();
  Serial.println();
}

void printBuffer(byte* buf, int len)
{
  for(int i = 0; i < len; i++) Serial.print(String(buf[i], HEX) + " ");
  Serial.println();
}

Packet receivePacket()
{
  byte packetBuffer[16] = {0};
  Packet packet;
  int i = 3;

  if(Serial.available() > 2)
  {
    packetBuffer[0] = Serial.read();
    packetBuffer[1] = Serial.read();
    packetBuffer[2] = Serial.read();

    if(packetBuffer[0] != 'L') return packet;

    while((i - 3) < (packetBuffer[2] + 1))
    {
      packetBuffer[i] = Serial.read();
      i++;
    }

    packet = Packet(packetBuffer);
    Serial.print("Packet buffer: ");
    printBuffer(packetBuffer, 16);
    Serial.println("Received packet: \n--------------------------------");
    printPacket(packet);
  }
  
  return packet;
}

void* getSettingFromID(byte id)
{
  Serial.print("Looking up SettingID ");
  Serial.println(id, HEX);
  
  switch(id)
  {
  case CLOCK_12HOUR:
    Serial.println("Changing setting CLOCK_12HOUR");
    return &clock_12hour;
    
  case CLOCK_FILLLIGHT:
    return &clock_fillLightOff;
    
  case CLOCK_FILLOFF:
    return &clock_fillOffBits;
  
  case CLOCK_AUTOBRIGHTNESS:
    return &clock_autobrightness;
    
  case LIGHT_MOTION:
    return &light_slightMotion;
    
  case LIGHT_TEMPERATURE:
    return &light_temperature;
    
  case LIGHT_AUTOBRIGHTNESS:
    return &light_autobrightness;
    
  default:
    return NULL;
  }
}

void changePacketSetting(Packet packet)
{
  if(packet.command == 0x01)
  {
    RtcDateTime dt = RtcDateTime((packet.args[0] << 8) + packet.args[1], packet.args[2], packet.args[3], packet.args[4], packet.args[5], packet.args[6]);
    rtc.SetDateTime(dt);
  }
  else
  {
    void* setting = getSettingFromID(packet.command);
    memcpy(setting, &packet.args, packet.len);
  }
}
