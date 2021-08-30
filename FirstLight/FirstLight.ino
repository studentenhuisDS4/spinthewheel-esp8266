#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

#include "FirstLight.h"
#include "Effects/Effects.h"
#include "Controller/Angle.h"
#include "Controller/FSM.h"
#include "UDP/Dispatch.h"

const char DEVICE_NAME[] = "SpinTheWheel-Controller-AP";
WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
WiFiManager wifiManager;
const unsigned int inPort = 9999; // remote port to receive OSC
unsigned int ledState = LOW;      // LOW means led is *on*

#define baseColor(hue) CHSV(hue, 255, 192)

uint8_t gHue = 0;
uint8_t bar_height = 0;
uint16_t angle = 0;
uint16_t prevAngle = 0;
bool error = 0;

String inString = "";
int AngleRevs = AngleIndicateRPM;
float offset = 0.0f;
float IndicateAngle = 0.0f;
float prevIndicateAngle = 0.0f;

#define CHANGE_TIME_MAX 500
long changeTimeLast;

void setup()
{
    Serial.begin(115200);
    Serial.println(F("\nSpinTheWheel Board Controller Output"));
    pinMode(LED_BUILTIN, OUTPUT);

    FastLED.addLeds<WS2813, DATA_PIN, COLOR_TYPE>(leds, NUM_LEDS);
    state_startup();

    wifiManager.autoConnect(DEVICE_NAME);
    state_wifi();

    Serial.println("Starting UDP");
    Udp.begin(inPort);
    Serial.print("Dispatching on UDP port: ");
    Serial.println(Udp.localPort());
    digitalWrite(LED_BUILTIN, ledState); // turn *on* led

    Serial.print(F("WiFi connected! IP address: "));
    Serial.println(WiFi.localIP());

    // Initiate IR sensors
    for (uint8_t i = 0; i < NUM_SENSORS; i++)
    {
        pinMode(SensorPins[i], INPUT);
    }
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println();
        Serial.println("*** Disconnected from AP so rebooting ***");
        Serial.println();
        ESP.reset();
    }

    OSCMessage msg;
    int size = Udp.parsePacket();
    if (size > 0)
    {
        while (size--)
        {
            msg.fill(Udp.read());
        }
        if (!msg.hasError())
        {
            msg.dispatch("/esp/wifireset", resetWifi);
            msg.dispatch("/esp/reset", resetESP);
            msg.empty();

            ledState = !ledState;
            digitalWrite(LED_BUILTIN, ledState);
        }
        else
        {
            Serial.print("error: ");
            Serial.println(msg.getError());
        }
    }

    // EVERY_N_MILLISECONDS_I(thistimer, RPMToMS(AngleRevs))
    EVERY_N_MILLISECONDS_I(thistimer, UpdateHuePeriodMS / 4)
    {
        Serial.println(millis());
        /* READ IR SENSORS */
        for (uint8_t i = 0; i < NUM_SENSORS; i++)
        {
            EncoderValues[i] = digitalRead(SensorPins[i]);
        }
        IndicateAngle = convertAngle(EncoderValues, NUM_SENSORS) * 36.0f + 18.0f;

        float diff = abs(IndicateAngle - prevIndicateAngle);
        if (diff > 1.0f && (diff == 36.0f || diff > 300.0f))
        {
            prevIndicateAngle = IndicateAngle;
            changeTimeLast = millis();
            
            state_rotating();
            Serial.println("Change detected. Going to rotate mode.");
        }
        else if (diff != 0.0f)
        {
            Serial.print(diff);
            Serial.print("\t");
            Serial.print(IndicateAngle);
            Serial.print("\t");
            Serial.print(prevIndicateAngle);
            Serial.println(" correction applied");
            IndicateAngle = prevIndicateAngle;
        }
        else
        {
            if (state == 2)
            {
                if (millis() - changeTimeLast > CHANGE_TIME_MAX)
                {
                    state_prize();
                    changeTimeLast = millis();
                }
                else
                {
                    light_angle(baseColor(gHue), 125, (uint16_t)(IndicateAngle));
                }
            }
            // else if (state == 3) {

            // }
        }
    }

    EVERY_N_MILLISECONDS(UpdateHuePeriodMS)
    {
        gHue++;
    }
    digitalWrite(LED_BUILTIN, LOW);
}

// EVERY_N_MILLISECONDS_I(timingObj, SpectrumPeriodMS)
// {
//     // timingObj.setPeriod(1000 / (SpectrumBPM / 60));
//     bar_height = random(0, SECTION_LEN - 1);

// // blink_rainbow(CHSV(gHue, 255, 192));
// // spectrum_bar(85, CHSV(gHue, 255, 192), bar_height);
// }

// if (SpectrumBPM <= 0)
//     error = 1;
// else
// {
//     error = 0;
// }

// fade(35); // combine with pulse()

// blink_color(baseColor(gHue));

// spectrum_bar(85, baseColor(gHue), bar_height);
// circular_snake(baseColor(gHue));
// strobo_blink();

// EVERY_N_MILLISECONDS(100)
// {
//     while (Serial.available() > 0)
//     {
//         int inChar = Serial.read();
//         if (isDigit(inChar))
//         {
//             // convert the incoming byte to a char and add it to the string:
//             inString += (char)inChar;
//         }
//         // if you get a newline, print the string, then the string's value:
//         if (inChar == '\n')
//         {
//             Serial.print("Accepted BPM:");
//             Serial.println(inString.toInt());
//             // pulse(CHSV(gHue, 255, 192));

//             AngleRevs = inString.toInt();
//             if (AngleRevs > 200)
//                 AngleRevs = AngleRevs % 120;
//             if (AngleRevs <= 0)
//                 AngleRevs = 60;

//             thistimer.setPeriod(RPMToMS(AngleRevs));
//             // clear the string for new input:
//             inString = "";
//         }
//     }
// }