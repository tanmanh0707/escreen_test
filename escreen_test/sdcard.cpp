#include "common.h"
#include <SPI.h>
#include <SD.h>

void LocalPrintSDInfo()
{
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    log_e("Unknown card type");
  } else {
    log_i("SD Card Type: ");
    if (cardType == CARD_MMC) {
      log_i("MMC");
    } else if (cardType == CARD_SD) {
      log_i("SDSC");
    } else if (cardType == CARD_SDHC) {
      log_i("SDHC");
    } else {
      log_i("UNKNOWN");
    }
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  log_i("SD Card Size: %llu MB", cardSize);
}

void SD_Init()
{
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  if (!SD.begin(SD_CS, SPI, 20000000)) {
    log_e("SD Card Mount Failed!");
  }
  else {
    log_i("Initialisation SD card done!");
    LocalPrintSDInfo();
  }
}