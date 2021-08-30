#ifndef SHARED_H__
#define SHARED_H__

#define COLOR_TYPE GRB
#define NUM_LEDS 140
#define SECTIONS 10
#define DATA_PIN D5
#define MAX_FPS 10

#define SECTION_LEN (NUM_LEDS / SECTIONS)

#define IR1 D6 //Sensor inside
#define IR2 D7 //Sensor middle inside
#define IR3 D4 //Sensor middle outside
#define IR4 D3 //Sensor outside

bool EncoderValues[4] = {0, 0, 0, 0};
uint8_t SensorPins[4] = {IR4, IR3, IR2, IR1};
uint8_t LUT[10] = {7, 5, 4, 12, 13, 0, 1, 3, 2, 6};

CRGB leds[NUM_LEDS];
uint8_t section_offset = 0;
uint8_t section_index = 0;

#define UpdateFPS 30
#define UpdateHuePeriodMS (1000 / UpdateFPS)

#define BlinkBPM (2 * 2 * 60)
#define BlinkPeriodMS (1000 / (BlinkBPM / 60))

#define StroboBPM (2 * 10 * 60)
#define StroboPeriodMS (1000 / (StroboBPM / 60))

#define TailFPS (18 * 2)
#define TailPeriodMS (1000 / TailFPS)
#define TailChangePeriodMS 20

#define AngleIndicateRPM 60
#define RPMToMS(Revs) (60000 / (SECTIONS * Revs))

#define PulseFadeCounts 10

int8_t SpectrumBPM = 120;
int8_t SpectrumPeriodMS = 1000 / (SpectrumBPM);

#define ErrorMS 500

#endif