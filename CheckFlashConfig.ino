
/*
  ESP8266 CheckFlashConfig by Markus Sattler
  This sketch tests if the EEPROM settings of the IDE match to the Hardware
*/

#include <FastLED.h>
FASTLED_USING_NAMESPACE

extern "C"
{
#include "user_interface.h"
}

#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <FS.h>
#include <EEPROM.h>

void setup(void)
{
    Serial.begin(115200);
}

void loop()
{

    uint32_t realSize = ESP.getFlashChipRealSize();
    uint32_t ideSize = ESP.getFlashChipSize();
    FlashMode_t ideMode = ESP.getFlashChipMode();

    Serial.printf("Flash real id:   %08X\n", ESP.getFlashChipId());
    Serial.printf("Flash real size: %u bytes\n\n", realSize);

    Serial.printf("Flash ide  size: %u bytes\n", ideSize);
    Serial.printf("Flash ide speed: %u Hz\n", ESP.getFlashChipSpeed());
    Serial.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

    if (ideSize != realSize)
    {
        Serial.println("Flash Chip configuration wrong!\n");
    }
    else
    {
        Serial.println("Flash Chip configuration ok.\n");
    }

    delay(5000);
}