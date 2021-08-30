#ifndef MODES_H__
#define MODES_H__

#include <FastLED.h>
#include "../Shared.h"

/* FUNCTION PROTOTYPES */
void colorWaves();
void colorwaves(CRGB *ledarray, uint16_t numleds, CRGBPalette16 &palette);
void heatMap(CRGBPalette16 palette, bool up);
void strandTest();
void showSolidColor();
void rainbow();
void rainbowWithGlitter();
void rainbowSolid();
void confetti();
void sinelon();
void bpm();
void juggle();
void fire();
void water();
void pride();
void radialPaletteShift();
void heatMap(CRGBPalette16 palette, bool up);
void addGlitter(uint8_t chanceOfGlitter);

#endif //MODES_H__