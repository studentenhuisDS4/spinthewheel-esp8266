#ifndef GENERAL_H__
#define GENERAL_H__

#include "../Shared.h"

bool bounceUp = true;

int check(int indexer)
{
    if (indexer >= NUM_LEDS || indexer < 0)
    {
        return 0;
    }
    return 1;
}

uint16_t radial_distance(uint16_t angle, uint16_t angle2)
{
    if (abs(angle - angle2) > 180)
    {
        return (360 - abs(angle - angle2));
    }
    else
    {
        return abs(angle - angle2);
    }
}

int offset_incr_bounce()
{
    if (bounceUp)
        section_offset++;
    else
        section_offset--;

    if (section_offset >= SECTION_LEN)
    {
        bounceUp = false;
    }
    else if (section_offset <= 0)
    {
        bounceUp = true;
    }
}

int offset_incr()
{
    section_offset++;
    if (section_offset >= SECTION_LEN)
    {
        section_offset = 0;
    }
}

int section_incr()
{
    section_index++;
    if (section_index >= SECTIONS)
    {
        section_index = 0;
    }
}

int correct_section_inversion_safe(int section, int offset)
{
    // Prevent overflow by reduction
    if (offset >= SECTION_LEN)
        offset = SECTION_LEN - 1;

    if (section % 2)
        return SECTION_LEN - offset - 1;
    else
        return offset;
}

void fadeall(int tail)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        if (!check(i))
            continue;
        leds[i].fadeLightBy(tail);
    }
}

void set_separated_leds(CRGB color, int offset, int brightness)
{
    for (int i = 0; i < SECTIONS; i = i + 1)
    {
        int indexed = i * SECTION_LEN + correct_section_inversion_safe(i, offset);
        if (!check(indexed))
            continue;

        leds[indexed] = color;
        leds[indexed] %= brightness;
    }
}

void set_separated_leds_until(CRGB color, uint8_t offset, uint8_t limit, uint8_t brightness)
{
    for (int i = 0; i < SECTIONS; i = i + 1)
    {
        for (int j = 0; j < limit; j = j + 1)
        {
            int indexed = i * SECTION_LEN + correct_section_inversion_safe(i, offset + j);
            if (!check(indexed))
                continue;

            leds[indexed] = color;
            leds[indexed] %= brightness;
        }
    }
}

void set_section(CRGB color, int section, int brightness)
{
    for (int i = 0; i < SECTION_LEN; i = i + 1)
    {
        int indexed = section * SECTION_LEN + correct_section_inversion_safe(section, i);
        if (!check(indexed))
            continue;

        leds[indexed] = color;
        leds[indexed] %= brightness;
    }
}

void set_section_until(CRGB color, int section, int offset, int limit, int brightness)
{
    for (int i = offset; i < limit; i = i + 1)
    {
        int indexed = section * SECTION_LEN + correct_section_inversion_safe(section, i);
        if (!check(indexed))
            continue;

        leds[indexed] = color;
        leds[indexed] %= brightness;
    }
}

#endif