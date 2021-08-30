#include "Modes.h"

void strandTest()
{
    static uint8_t i = 0;

    EVERY_N_SECONDS(1)
    {
        i++;
        if (i >= NUM_LEDS)
            i = 0;
    }

    fill_solid(leds, NUM_LEDS, CRGB::Black);

    leds[i] = solidColor;
}

void showSolidColor()
{
    fill_solid(leds, NUM_LEDS, solidColor);
}

// Patterns from FastLED example DemoReel100: https://github.com/FastLED/FastLED/blob/master/examples/DemoReel100/DemoReel100.ino
void rainbow()
{
    // FastLED's built-in rainbow generator
    fill_rainbow(leds, NUM_LEDS, gHue, 255 / NUM_LEDS);
}

void rainbowWithGlitter()
{
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    addGlitter(80);
}

void rainbowSolid()
{
    fill_solid(leds, NUM_LEDS, CHSV(gHue, 255, 255));
}

void confetti()
{
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy(leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    // leds[pos] += CHSV( gHue + random8(64), 200, 255);
    leds[pos] += ColorFromPalette(palettes[currentPaletteIndex], gHue + random8(64));
}

void sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy(leds, NUM_LEDS, 20);
    int pos = beatsin16(speed, 0, NUM_LEDS);
    static int prevpos = 0;
    CRGB color = ColorFromPalette(palettes[currentPaletteIndex], gHue, 255);
    if (pos < prevpos)
    {
        fill_solid(leds + pos, (prevpos - pos) + 1, color);
    }
    else
    {
        fill_solid(leds + prevpos, (pos - prevpos) + 1, color);
    }
    prevpos = pos;
}

void bpm()
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t beat = beatsin8(speed, 64, 255);
    CRGBPalette16 palette = palettes[currentPaletteIndex];
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
}

void juggle()
{
    static uint8_t numdots = 4;                // Number of dots in use.
    static uint8_t faderate = 2;               // How long should the trails be. Very low value = longer trails.
    static uint8_t hueinc = 255 / numdots - 1; // Incremental change in hue between each dot.
    static uint8_t thishue = 0;                // Starting hue.
    static uint8_t curhue = 0;                 // The current hue
    static uint8_t thissat = 255;              // Saturation of the colour.
    static uint8_t thisbright = 255;           // How bright should the LED/display be.
    static uint8_t basebeat = 5;               // Higher = faster movement.

    static uint8_t lastSecond = 99;              // Static variable, means it's only defined once. This is our 'debounce' variable.
    uint8_t secondHand = (millis() / 1000) % 30; // IMPORTANT!!! Change '30' to a different value to change duration of the loop.

    if (lastSecond != secondHand)
    { // Debounce to make sure we're not repeating an assignment.
        lastSecond = secondHand;
        switch (secondHand)
        {
        case 0:
            numdots = 1;
            basebeat = 20;
            hueinc = 16;
            faderate = 2;
            thishue = 0;
            break; // You can change values here, one at a time , or altogether.
        case 10:
            numdots = 4;
            basebeat = 10;
            hueinc = 16;
            faderate = 8;
            thishue = 128;
            break;
        case 20:
            numdots = 8;
            basebeat = 3;
            hueinc = 0;
            faderate = 8;
            thishue = random8();
            break; // Only gets called once, and not continuously for the next several seconds. Therefore, no rainbows.
        case 30:
            break;
        }
    }

    // Several colored dots, weaving in and out of sync with each other
    curhue = thishue; // Reset the hue values.
    fadeToBlackBy(leds, NUM_LEDS, faderate);
    for (int i = 0; i < numdots; i++)
    {
        //beat16 is a FastLED 3.1 function
        leds[beatsin16(basebeat + i + numdots, 0, NUM_LEDS)] += CHSV(gHue + curhue, thissat, thisbright);
        curhue += hueinc;
    }
}

void fire()
{
    heatMap(HeatColors_p, true);
}

void water()
{
    heatMap(IceColors_p, false);
}

// Pride2015 by Mark Kriegsman: https://gist.github.com/kriegsman/964de772d64c502760e5
// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.

extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];
extern const uint8_t gGradientPaletteCount;

void pride()
{
    static uint16_t sPseudotime = 0;
    static uint16_t sLastMillis = 0;
    static uint16_t sHue16 = 0;

    uint8_t sat8 = beatsin88(87, 220, 250);
    uint8_t brightdepth = beatsin88(341, 96, 224);
    uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
    uint8_t msmultiplier = beatsin88(147, 23, 60);

    uint16_t hue16 = sHue16; //gHue * 256;
    uint16_t hueinc16 = beatsin88(113, 1, 3000);

    uint16_t ms = millis();
    uint16_t deltams = ms - sLastMillis;
    sLastMillis = ms;
    sPseudotime += deltams * msmultiplier;
    sHue16 += deltams * beatsin88(400, 5, 9);
    uint16_t brightnesstheta16 = sPseudotime;

    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
        hue16 += hueinc16;
        uint8_t hue8 = hue16 / 256;

        brightnesstheta16 += brightnessthetainc16;
        uint16_t b16 = sin16(brightnesstheta16) + 32768;

        uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
        uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
        bri8 += (255 - brightdepth);

        CRGB newcolor = CHSV(hue8, sat8, bri8);

        uint16_t pixelnumber = i;
        pixelnumber = (NUM_LEDS - 1) - pixelnumber;

        nblend(leds[pixelnumber], newcolor, 64);
    }
}

void radialPaletteShift()
{
    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
        // leds[i] = ColorFromPalette( gCurrentPalette, gHue + sin8(i*16), brightness);
        leds[i] = ColorFromPalette(gCurrentPalette, i + gHue, 255, LINEARBLEND);
    }
}

// based on FastLED example Fire2012WithPalette: https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino
void heatMap(CRGBPalette16 palette, bool up)
{
    fill_solid(leds, NUM_LEDS, CRGB::Black);

    // Add entropy to random number generator; we use a lot of it.
    random16_add_entropy(random(256));

    // Array of temperature readings at each simulation cell
    static byte heat[NUM_LEDS];

    byte colorindex;

    // Step 1.  Cool down every cell a little
    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
        heat[i] = qsub8(heat[i], random8(0, ((cooling * 10) / NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (uint16_t k = NUM_LEDS - 1; k >= 2; k--)
    {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < sparking)
    {
        int y = random8(7);
        heat[y] = qadd8(heat[y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (uint16_t j = 0; j < NUM_LEDS; j++)
    {
        // Scale the heat value from 0-255 down to 0-240
        // for best results with color palettes.
        colorindex = scale8(heat[j], 190);

        CRGB color = ColorFromPalette(palette, colorindex);

        if (up)
        {
            leds[j] = color;
        }
        else
        {
            leds[(NUM_LEDS - 1) - j] = color;
        }
    }
}

void addGlitter(uint8_t chanceOfGlitter)
{
    if (random8() < chanceOfGlitter)
    {
        leds[random16(NUM_LEDS)] += CRGB::White;
    }
}

uint8_t beatsaw8(accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255,
                 uint32_t timebase = 0, uint8_t phase_offset = 0)
{
    uint8_t beat = beat8(beats_per_minute, timebase);
    uint8_t beatsaw = beat + phase_offset;
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8(beatsaw, rangewidth);
    uint8_t result = lowest + scaledbeat;
    return result;
}

void colorWaves()
{
    colorwaves(leds, NUM_LEDS, gCurrentPalette);
}

// ColorWavesWithPalettes by Mark Kriegsman: https://gist.github.com/kriegsman/8281905786e8b2632aeb
// This function draws color waves with an ever-changing,
// widely-varying set of parameters, using a color palette.
void colorwaves(CRGB *ledarray, uint16_t numleds, CRGBPalette16 &palette)
{
    static uint16_t sPseudotime = 0;
    static uint16_t sLastMillis = 0;
    static uint16_t sHue16 = 0;

    // uint8_t sat8 = beatsin88( 87, 220, 250);
    uint8_t brightdepth = beatsin88(341, 96, 224);
    uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
    uint8_t msmultiplier = beatsin88(147, 23, 60);

    uint16_t hue16 = sHue16; //gHue * 256;
    uint16_t hueinc16 = beatsin88(113, 300, 1500);

    uint16_t ms = millis();
    uint16_t deltams = ms - sLastMillis;
    sLastMillis = ms;
    sPseudotime += deltams * msmultiplier;
    sHue16 += deltams * beatsin88(400, 5, 9);
    uint16_t brightnesstheta16 = sPseudotime;

    for (uint16_t i = 0; i < numleds; i++)
    {
        hue16 += hueinc16;
        uint8_t hue8 = hue16 / 256;
        uint16_t h16_128 = hue16 >> 7;
        if (h16_128 & 0x100)
        {
            hue8 = 255 - (h16_128 >> 1);
        }
        else
        {
            hue8 = h16_128 >> 1;
        }

        brightnesstheta16 += brightnessthetainc16;
        uint16_t b16 = sin16(brightnesstheta16) + 32768;

        uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
        uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
        bri8 += (255 - brightdepth);

        uint8_t index = hue8;
        //index = triwave8( index);
        index = scale8(index, 240);

        CRGB newcolor = ColorFromPalette(palette, index, bri8);

        uint16_t pixelnumber = i;
        pixelnumber = (numleds - 1) - pixelnumber;

        nblend(ledarray[pixelnumber], newcolor, 128);
    }
}

// Alternate rendering function just scrolls the current palette
// across the defined LED strip.
void palettetest(CRGB *ledarray, uint16_t numleds, const CRGBPalette16 &gCurrentPalette)
{
    static uint8_t startindex = 0;
    startindex--;
    fill_palette(ledarray, numleds, startindex, (256 / NUM_LEDS) + 1, gCurrentPalette, 255, LINEARBLEND);
}
