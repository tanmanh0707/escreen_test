#include "common.h"

void SD_Init();

void setup()
{
  Serial.begin(115200);

  SD_Init();
  SCREEN_Init();

  log_i("EScreen Test Started!");
}

void loop()
{
  SCREEN_Update();
}


