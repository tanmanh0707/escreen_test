#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <LovyanGFX.h>
#include <lvgl.h>
#include <libs/gif/lv_gif.h>
#include "configs.h"
#include "images.h"

void SD_Init(void);

void SCREEN_Init(void);
void SCREEN_Update(void);
void SCREEN_SetAddrWindow(int32_t x, int32_t y, int32_t w, int32_t h);
void SCREEN_PushPixelsDMA(const uint16_t* pixels, uint32_t len, bool swap = false);