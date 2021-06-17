//================================
// Display Handler
//================================

void setPixel(int id, Col col)
{
  pixels.setPixelColor(id, pixels.gamma32(pixels.Color(col.r, col.g, col.b)));
}

void setPanel(int id, Col col)
{
  for(int i = 0; i < NUMPIXPERPANEL; i++)
  {
    setPixel(panelIDs[id][i], col);
  }
}

void fillLamp(Col col)
{
  for(int i = 0; i < NUMPIXELS; i++)
  {
    setPixel(i, col);
  }
}

void clearLamp()
{
  fillLamp(Col(0));
}

void changeMode(int newMode)
{
  clearLamp();
  mode = newMode;

  pixels.show();
}

Col changeBrightness(Col col, float brightness)
{
  Col ret = col;

  ret.r = round(ret.r * brightness);
  ret.g = round(ret.g * brightness);
  ret.b = round(ret.b * brightness);

  return ret;
}

Col downsampleColor(Col col, int colorDepth)
{
  Col ret;

  ret.r = ((colorDepth - 1) * 255 / col.r) * (255 / (colorDepth - 1));
  ret.g = ((colorDepth - 1) * 255 / col.g) * (255 / (colorDepth - 1));
  ret.b = ((colorDepth - 1) * 255 / col.b) * (255 / (colorDepth - 1));

  return ret;
}

// ================================
// Displays
// ================================

void progressDisplay(int num, Col col)
{
  if(num >= NUMPANELS) num = NUMPANELS - 1;
  num++;

  for(int i = 0; i < num; i++)
  {
    setPanel(i, col);
  }
}

void binaryDisplay(int num, int panelOffset, int numPanels, Col on, Col off, int numBits = 6)
{
  Col col;
  int resolution = (numPanels * NUMPIXPERPANEL) / numBits;
  
  for(int panel = 0; panel < numPanels; panel++)
  {
    for(int digit = 0; digit < 6 / resolution; digit++)
    {
      if(num & 0x1 == 1) col = on;
      else col = off;
      
      for(int i = 0; i < resolution; i++)
      {
        setPixel(panelIDs[panel + panelOffset][digit * resolution + i], col);
      }

      num = num >> 1;
    }
  }
}
