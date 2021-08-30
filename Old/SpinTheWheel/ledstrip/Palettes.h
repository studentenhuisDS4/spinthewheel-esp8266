#ifndef PALETTES_H__
#define PALETTES_H__

#include "../ledstrip/Modes.h"
#include "../ledstrip/Twinkles.h"
#include "../ledstrip/TwinkleFOX.h"

// Palette Typedefs
typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct
{
    Pattern pattern;
    String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

// List of patterns to cycle through.  Each is defined as a separate function below.
PatternAndNameList patterns = {
    {pride, "Pride"},
    {colorWaves, "Color Waves"},

    // twinkle patterns
    {rainbowTwinkles, "Rainbow Twinkles"},
    {snowTwinkles, "Snow Twinkles"},
    {cloudTwinkles, "Cloud Twinkles"},
    {incandescentTwinkles, "Incandescent Twinkles"},

    // TwinkleFOX patterns
    {retroC9Twinkles, "Retro C9 Twinkles"},
    {redWhiteTwinkles, "Red & White Twinkles"},
    {blueWhiteTwinkles, "Blue & White Twinkles"},
    {redGreenWhiteTwinkles, "Red, Green & White Twinkles"},
    {fairyLightTwinkles, "Fairy Light Twinkles"},
    {snow2Twinkles, "Snow 2 Twinkles"},
    {hollyTwinkles, "Holly Twinkles"},
    {iceTwinkles, "Ice Twinkles"},
    {partyTwinkles, "Party Twinkles"},
    {forestTwinkles, "Forest Twinkles"},
    {lavaTwinkles, "Lava Twinkles"},
    {fireTwinkles, "Fire Twinkles"},
    {cloud2Twinkles, "Cloud 2 Twinkles"},
    {oceanTwinkles, "Ocean Twinkles"},

    {rainbow, "Rainbow"},
    {rainbowWithGlitter, "Rainbow With Glitter"},
    {rainbowSolid, "Solid Rainbow"},
    {confetti, "Confetti"},
    {sinelon, "Sinelon"},
    {bpm, "Beat"},
    {juggle, "Juggle"},
    {fire, "Fire"},
    {water, "Water"},

    {showSolidColor, "Solid Color"}};

const uint8_t patternCount = ARRAY_SIZE(patterns);

typedef struct
{
    CRGBPalette16 palette;
    String name;
} PaletteAndName;
typedef PaletteAndName PaletteAndNameList[];

const CRGBPalette16 palettes[] = {
    RainbowColors_p,
    RainbowStripeColors_p,
    CloudColors_p,
    LavaColors_p,
    OceanColors_p,
    ForestColors_p,
    PartyColors_p,
    HeatColors_p};

const uint8_t paletteCount = ARRAY_SIZE(palettes);

const String paletteNames[paletteCount] = {
    "Rainbow",
    "Rainbow Stripe",
    "Cloud",
    "Lava",
    "Ocean",
    "Forest",
    "Party",
    "Heat",
};

#endif // PALETTES_H__