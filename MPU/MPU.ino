/* This driver reads quaternion data from the MPU6060 and sends
   Open Sound Control messages.
  GY-521  NodeMCU
  MPU6050 devkit 1.0
  board   Lolin         Description
  ======= ==========    ====================================================
  VCC     VU (5V USB)   Not available on all boards so use 3.3V if needed.
  GND     G             Ground
  SCL     D1 (GPIO05)   I2C clock
  SDA     D2 (GPIO04)   I2C data
  XDA     not connected
  XCL     not connected
  AD0     not connected
  INT     D8 (GPIO15)   Interrupt pin
*/

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Calibration.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;        // [w, x, y, z]         quaternion container
VectorInt16 aa;      // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;  // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld; // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity; // [x, y, z]            gravity vector

////////////////////////////////// WIFI, PINS, CONFIG ///////////////////////////
#define OUTPUT_TEAPOT_OSC
#define INTERRUPT_PIN 15 // use pin 15 on ESP8266
#define CONFIG_CALIBRATION 0
const char DEVICE_NAME[] = "SpinTheWheel-Sensor-AP";
WiFiUDP Udp;                               // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(255, 255, 255, 255); // remote IP to receive OSC
const unsigned int outPort = 9999;         // remote port to receive OSC

//////////////////////////// INTERRUPT DETECTION ROUTINE ////////////////////////
volatile bool mpuInterrupt = false; // indicates whether MPU interrupt pin has gone high
void dmpDataReady()
{
    mpuInterrupt = true;
}

void mpu_setup()
{
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties

    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    mpu.setRate(1);
    Serial.print(F("GetRate: "));
    Serial.println(mpu.getRate());

    // Static calibration
    mpu.setXAccelOffset(-3420);
    mpu.setYAccelOffset(122);
    mpu.setZAccelOffset(1479);
    mpu.setXGyroOffset(169);
    mpu.setYGyroOffset(-6);
    mpu.setZGyroOffset(7);

    // make sure it worked (returns 0 if so)
    if (devStatus == 0)
    {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
    else
    {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
}

void setup(void)
{
    Serial.begin(115200);
    Serial.println(F("\nOrientation Sensor OSC output"));
    Serial.println();

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect(DEVICE_NAME);

    Serial.print(F("WiFi connected! IP address: "));
    Serial.println(WiFi.localIP());

    mpu_setup();

    if (CONFIG_CALIBRATION == 1)
        calibrateInit();
}

void mpu_loop()
{
    // if programming failed, don't try to do anything
    if (!dmpReady)
        return;

    // wait for MPU interrupt or extra packet(s) available
    if (!mpuInterrupt && fifoCount < packetSize)
        return;

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024)
    {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));
        // otherwise, check for DMP data ready interrupt (this should happen frequently)
    }
    else if (mpuIntStatus & 0x02)
    {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize)
            fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);

        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

#ifdef OUTPUT_TEAPOT_OSC
#ifndef OUTPUT_READABLE_QUATERNION
        // display quaternion values in easy matrix form: w x y z
        mpu.dmpGetQuaternion(&q, fifoBuffer);
#endif
        // Send OSC message
        OSCMessage msg("/imuquat");
        msg.add((float)q.w);
        msg.add((float)q.x);
        msg.add((float)q.y);
        msg.add((float)q.z);

        Udp.beginPacket(outIp, outPort);
        msg.send(Udp);
        Udp.endPacket();

        msg.empty();
#endif
    }
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println();
        Serial.println("*** Disconnected from AP so rebooting ***");
        Serial.println();
        ESP.reset();
    }

    if (CONFIG_CALIBRATION == 1 && CALIB_FINISHED == false)
        calibrate();
    else
        mpu_loop();
}