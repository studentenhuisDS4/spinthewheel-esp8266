#include <FastLED.h>

#define LED_PIN D5
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define NUM_LEDS 300

#define BRIGHTNESS 100
#define FRAMES_PER_SECOND 240

bool gReverseDirection = true;

CRGB leds[NUM_LEDS];

void setup()
{
    delay(10); // sanity delay
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    // FastLED.FastLED.setBrightness(BRIGHTNESS);
}

void loop()
{
    fireLoop();
}

void movingLoop()
{
    for (int dot = 0; dot < NUM_LEDS; dot++)
    {
        leds[dot] = CRGB::Blue;
        FastLED.show();
        // clear this led for the next time around the loop
        leds[dot] = CRGB::Black;
        // delayMicroseconds(10);
    }
}

void fireLoop()
{
    // Add entropy to random number generator; we use a lot of it.
    // random16_add_entropy( random());

    Fire2012(); // run simulation frame

    FastLED.show(); // display this frame
    FastLED.delay(1000 / FRAMES_PER_SECOND);
}

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// https://github.com/FastLED/FastLED/blob/master/examples/Fire2012/Fire2012.ino
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING 55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

void Fire2012()
{
    // Array of temperature readings at each simulation cell
    static byte heat[NUM_LEDS];

    // Step 1.  Cool down every cell a little
    for (int i = 0; i < NUM_LEDS; i++)
    {
        heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = NUM_LEDS - 1; k >= 2; k--)
    {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING)
    {
        int y = random8(7);
        heat[y] = qadd8(heat[y], random8(160, 255));
    }

    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < NUM_LEDS; j++)
    {
        CRGB color = HeatColor(heat[j]);
        int pixelnumber;
        if (gReverseDirection)
        {
            pixelnumber = (NUM_LEDS - 1) - j;
        }
        else
        {
            pixelnumber = j;
        }
        leds[pixelnumber] = color;
    }
}
