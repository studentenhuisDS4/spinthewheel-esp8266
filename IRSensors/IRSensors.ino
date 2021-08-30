#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

#define IR1 D6 //Sensor inside
#define IR2 D7 //Sensor middle inside
#define IR3 D4 //Sensor middle outside
#define IR4 D3 //Sensor outside

bool EncoderValues[4] = {0, 0, 0, 0};
uint8_t SensorPins[4] = {IR4, IR3, IR2, IR1};
int LUT[10] = {7, 5, 4, 12, 13, 0, 1, 3, 2, 6};

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

const char DEVICE_NAME[] = "SpinTheWheel-Controller-AP";
WiFiUDP Udp;                      // A UDP instance to let us send and receive packets over UDP
const unsigned int inPort = 9999; // remote port to receive OSC
unsigned int ledState = LOW;      // LOW means led is *on*

#define NUM_SENSORS sizeof(SensorPins)
#define NUM_SECTIONS sizeof(LUT)

uint8_t readBinaryVector(bool *v, uint8_t length)
{
    int result = 0;
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
        return 4;

    for (int i = 0; i < NUM_SECTIONS; i++)
    {
        if (sectionNumber == LUT[i])
        {
            return i;
        }
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println(F("\nSpinTheWheel Board Controller Output"));
    for (uint8_t i = 0; i < NUM_SENSORS; i++)
    {
        pinMode(SensorPins[i], INPUT);
    }

    WiFiManager wifiManager;
    wifiManager.autoConnect(DEVICE_NAME);

    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, ledState); // turn *on* led

    Serial.print(F("WiFi connected! IP address: "));
    Serial.println(WiFi.localIP());

    Serial.println("Starting UDP");
    Udp.begin(inPort);
    Serial.print("Dispatching on UDP port: ");
    Serial.println(Udp.localPort());
}

void imuquat(OSCMessage &msg)
{
    char output[100];
    float q[4] = {msg.getFloat(0), msg.getFloat(1), msg.getFloat(2), msg.getFloat(3)};
    float yaw = atan2(-2.0f * (q[1] * q[2] - q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]) / (2.0f * PI) * 360.0f;
    float pitch = asin(2.0f * (q[1] * q[3] + q[0] * q[2])) / (2.0f * PI) * 360.0f;
    float roll = atan2(-2.0f * (-q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]) / (2.0f * PI) * 360.0f;
    sprintf(output, "/imuquat: Y: %.2f, P: %.2f, R: %.2f", yaw, pitch, roll);

    Serial.println(output);
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

    for (uint8_t i = 0; i < NUM_SENSORS; i++)
    {
        EncoderValues[i] = digitalRead(SensorPins[i]);
        Serial.print(EncoderValues[i]);
        Serial.print(" ");
    }
    Serial.print(" ");
    // Serial.print(readBinaryVector(EncoderValues, NUM_SENSORS));
    Serial.print(convertAngle(EncoderValues, NUM_SENSORS));
    Serial.print('\n');

    // OSCMessage msg;
    // int size = Udp.parsePacket();
    // if (size > 0)
    // {
    //     while (size--)
    //     {
    //         msg.fill(Udp.read());
    //     }
    //     if (!msg.hasError())
    //     {
    //         msg.dispatch("/imuquat", imuquat);
    //         msg.empty();

    //         ledState = !ledState;
    //         digitalWrite(BUILTIN_LED, ledState);
    //     }
    //     else
    //     {
    //         Serial.print("error: ");
    //         Serial.println(msg.getError());
    //     }
    // }
}