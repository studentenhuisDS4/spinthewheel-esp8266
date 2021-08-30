#ifndef SHARED_H__
#define SHARED_H__

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// Physical settings
#define DATA_PIN D5
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 140
#define FRAMES_PER_SECOND 240 // here you can control the speed. With the Access Point / Web Server the animations run a bit slower.
#define MILLI_AMPS 16000

// AP related
const bool apMode = false;
const char WiFiAPPSK[] = "groen333"; // AP mode password
char *ssid = "DS4_LIVE_trpn";
char *password = "groen333";

/* Server related */
ESP8266WebServer webServer(80);
ESP8266HTTPUpdateServer httpUpdateServer;

// Led array
CRGB leds[NUM_LEDS];

// Ledstrip settings
const uint8_t brightnessCount = 5;
uint8_t brightnessMap[brightnessCount] = {16, 32, 64, 128, 255};
uint8_t brightnessIndex = 0;
uint8_t secondsPerPalette = 10; // demo; 20-120 is better for deployment
// COOLING Less cooling = taller flames.  More cooling = shorter flames. Suggested range 20-100
uint8_t cooling = 49;
// SPARKING: Default 120, suggested range 50-200.
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
uint8_t sparking = 60;
uint8_t speed = 30;
uint8_t autoplay = 0;
uint8_t autoplayDuration = 10;
unsigned long autoPlayTimeout = 0;

/* Palette related */
uint8_t gCurrentPaletteNumber = 0;
uint8_t currentPatternIndex = 0; // Index number of which pattern is current
uint8_t currentPaletteIndex = 0;
CRGBPalette16 gCurrentPalette(CRGB::Black);
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
CRGB solidColor = CRGB::Blue;
extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
CRGBPalette16 gTargetPalette(gGradientPalettes[0]);

#endif //SHARED_H__
