uint8_t previousSection = 0;

// Between 13 and 0 a slight error
// Jammer joh:   7      0 1 1 1
// Cocktail:     5      0 1 0 1
// Dobbelen:     4      0 1 0 0 (slight offset, too early in entering)
// Shots (3):   12      1 1 0 0
// Beer (2):    13      1 1 0 1 (possibility of 8 in beer zone)
// Prize:        0      0 0 0 0 (early in beer zone)
// Dobbelen:     1      0 0 0 1
// Replay:       3      0 0 1 1
// Beer (3):     2      0 0 1 0 (very early)
// Shot (1):     6      0 1 1 0 (good enter, bit early exit)

#define NUM_SENSORS sizeof(SensorPins)
#define NUM_SECTIONS sizeof(LUT)

uint8_t readBinaryVector(bool *v, uint8_t length)
{
    uint8_t result = 0;
    for (size_t i = 0; i < length; i++)
    {
        result <<= 1;
        if (v[i]++ == 1)
            result |= 1;
    }
    return result;
}

uint8_t convertAngle(bool *v, uint8_t length)
{
    uint8_t sectionNumber = readBinaryVector(v, length);
    if (sectionNumber == 8)
    {
        previousSection = 4;
        return 4;
    }
    else if (sectionNumber == 3 && (previousSection == 5 || previousSection == 4))
    {
        return 4;
    }

    for (uint8_t i = 0; i < NUM_SECTIONS; i++)
    {
        if (sectionNumber == LUT[i])
        {
            previousSection = i;
            return i;
        }
    }
}