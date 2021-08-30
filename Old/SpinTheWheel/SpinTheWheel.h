#ifndef SPIN_THE_WHEEL_H__
#define SPIN_THE_WHEEL_H__

#include <ESP8266WiFi.h>
#include <FS.h>
#include <EEPROM.h>
#include <FastLED.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include "ledstrip/Twinkles.h"
#include "ledstrip/TwinkleFOX.h"
#include "ledstrip/GradientPalettes.h"
#include "server/FSBrowser.h"
#include "server/Field.h"
#include "server/Fields.h"
#include "Shared.h"
#include "ledstrip/Palettes.h"

FASTLED_USING_NAMESPACE
extern "C"
{
#include "user_interface.h"
}

/* DEFINITIONS AND CONFIG */
//#define FASTLED_ALLOW_INTERRUPTS 1
//#define INTERRUPT_THRESHOLD 1
#define FASTLED_INTERRUPT_RETRY_COUNT 0

#endif