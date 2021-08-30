#ifndef TAIL_H__
#define TAIL_H__

#include <FastLed.h>
#include "../Shared.h"
#include "General.h"

#define max_brightness 255
#define part_brightness 50

bool flipflop = false;
uint16_t fadeCount = 0;

void tail(int fade_perc, CRGB base_color)
{
    if (fade_perc >= 1)
        fadeall(fade_perc);

    set_separated_leds(base_color, section_offset, max_brightness);
    FastLED.show();
    offset_incr();
}

void tailBounce(int fade_perc, CRGB base_color)
{
    if (fade_perc >= 1)
        fadeall(fade_perc);

    set_separated_leds(base_color, section_offset, max_brightness);
    FastLED.show();
    offset_incr_bounce();
}

void spectrum_bar(int fade_perc, CRGB base_color, int height)
{
    if (fade_perc >= 1)
        fadeall(200);

    set_separated_leds_until(base_color, 0, height, part_brightness);
    set_separated_leds(base_color, height, max_brightness);
    FastLED.show();
}

void toggleflipFlop()
{
    flipflop = !flipflop;
}

void blink_color(CHSV color)
{
    toggleflipFlop();
    if (flipflop)
        fadeall(255);
    else
        set_separated_leds_until(color, 0, SECTION_LEN, max_brightness);

    FastLED.show();
}

void blink_error()
{
    toggleflipFlop();

    fadeall(255);
    if (flipflop)
    {
        set_separated_leds_until(CRGB(255, 0, 0), 0, SECTION_LEN / 2, part_brightness);
        set_separated_leds_until(CRGB(255, 255, 255), SECTION_LEN / 2, SECTION_LEN, part_brightness);
    }
    else
    {
        set_separated_leds_until(CRGB(255, 0, 0), SECTION_LEN / 2, SECTION_LEN, part_brightness);
        set_separated_leds_until(CRGB(255, 255, 255), 0, SECTION_LEN / 2, part_brightness);
    }
    FastLED.show();
}

// Light up angle section
void light_angle(CHSV color, uint8_t fade_factor, uint16_t angle)
{
    fadeall(fade_factor);
    // index is mapped to angle (10 sections)
    for (uint8_t i = 0; i < SECTIONS; i++)
    {
        uint16_t falloff = 0;
        // Check 0 or 100% wraparound
        uint16_t distance = radial_distance(angle, i * (360 / SECTIONS));
        falloff = 3 * distance * max_brightness / 180;

        // Serial.println(map(i, 0, SECTIONS - 1, 0, max_brightness));
        // Serial.println(round(angle * 10 / 255));

        // Serial.print(max_brightness);
        if (falloff > max_brightness)
            set_section(color, i, 0);
        else
            set_section(color, i, max_brightness - falloff);
    }

    FastLED.show();
}

// Pulse and fade whole ledstrip
void pulse(CHSV color)
{
    set_separated_leds_until(color, 0, SECTION_LEN, max_brightness);

    FastLED.show();
}

void fade(uint8_t fade_factor)
{
    fadeall(fade_factor);

    fadeCount--;
    if (fadeCount <= 0)
    {
        fadeCount = PulseFadeCounts;
    }
    FastLED.show();
}

// Strobo blink
void strobo_blink()
{
    toggleflipFlop();
    if (flipflop)
        fadeall(255);
    else
        set_separated_leds_until(CRGB(255, 255, 255), 0, SECTION_LEN, part_brightness);

    FastLED.show();
}

// Circular fill
void circular_fill()
{
    FastLED.show();
}

// Circular_snake
void circular_snake(CHSV color, int tail)
{
    static int section = 0;
    section++;

    for (size_t i = 0; i < tail; i++)
    {
        int section_iterator = section;
        set_section_until(color, section, 0, 3, max_brightness);
    }

    FastLED.show();
}
#endif