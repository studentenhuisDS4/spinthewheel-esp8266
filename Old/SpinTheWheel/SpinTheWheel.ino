#include "SpinTheWheel.h"

// #define FASTLED_FORCE_SOFTWARE_PINS 1
// scale the brightness of all pixels down
void dimAll(byte value)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i].nscale8(value);
    }
}

void setup()
{
    WiFi.setSleepMode(WIFI_NONE_SLEEP);

    Serial.begin(115200);
    delay(100);
    Serial.setDebugOutput(true);

    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS); // for WS2812 (Neopixel)
    FastLED.setDither(false);
    FastLED.setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(brightness);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();

    EEPROM.begin(512);
    loadSettings();

    FastLED.setBrightness(brightness);

    Serial.println();
    Serial.print(F("Heap: "));
    Serial.println(system_get_free_heap_size());
    Serial.print(F("Boot Vers: "));
    Serial.println(system_get_boot_version());
    Serial.print(F("CPU: "));
    Serial.println(system_get_cpu_freq());
    Serial.print(F("SDK: "));
    Serial.println(system_get_sdk_version());
    Serial.print(F("Chip ID: "));
    Serial.println(system_get_chip_id());
    Serial.print(F("Flash ID: "));
    Serial.println(spi_flash_get_id());
    Serial.print(F("Flash Size: "));
    Serial.println(ESP.getFlashChipRealSize());
    Serial.print(F("Vcc: "));
    Serial.println(ESP.getVcc());
    Serial.println();

    SPIFFS.begin();
    {
        Serial.println("SPIFFS contents:");

        Dir dir = SPIFFS.openDir("/");
        while (dir.next())
        {
            String fileName = dir.fileName();
            size_t fileSize = dir.fileSize();
            Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
        }
        Serial.printf("\n");
    }

    if (apMode)
    {
        WiFi.mode(WIFI_AP);

        // Do a little work to get a unique-ish name. Append the
        // last two bytes of the MAC (HEX'd) to "Thing-":
        uint8_t mac[WL_MAC_ADDR_LENGTH];
        WiFi.softAPmacAddress(mac);
        String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                       String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
        macID.toUpperCase();
        String AP_NameString = "ESP8266 Thing " + macID;

        char AP_NameChar[AP_NameString.length() + 1];
        memset(AP_NameChar, 0, AP_NameString.length() + 1);

        for (int i = 0; i < AP_NameString.length(); i++)
            AP_NameChar[i] = AP_NameString.charAt(i);

        WiFi.softAP(AP_NameChar, WiFiAPPSK);

        Serial.printf("Connect to Wi-Fi access point: %s\n", AP_NameChar);
        Serial.println("and open http://192.168.4.1 in your browser");
    }
    else
    {
        WiFi.mode(WIFI_STA);
        Serial.printf("Connecting to %s\n", ssid);
        if (String(WiFi.SSID()) != String(ssid))
        {
            WiFi.begin(ssid, password);
        }
    }

    httpUpdateServer.setup(&webServer);

    webServer.on("/all", HTTP_GET, []() {
        String json = getFieldsJson(fields, fieldCount);
        webServer.send(200, "text/json", json);
    });

    webServer.on("/fieldValue", HTTP_GET, []() {
        String name = webServer.arg("name");
        String value = getFieldValue(name, fields, fieldCount);
        webServer.send(200, "text/json", value);
    });

    webServer.on("/fieldValue", HTTP_POST, []() {
        String name = webServer.arg("name");
        String value = webServer.arg("value");
        String newValue = setFieldValue(name, value, fields, fieldCount);
        webServer.send(200, "text/json", newValue);
    });

    webServer.on("/power", HTTP_POST, []() {
        String value = webServer.arg("value");
        setPower(value.toInt());
        sendInt(power);
    });

    webServer.on("/cooling", HTTP_POST, []() {
        String value = webServer.arg("value");
        cooling = value.toInt();
        broadcastInt("cooling", cooling);
        sendInt(cooling);
    });

    webServer.on("/sparking", HTTP_POST, []() {
        String value = webServer.arg("value");
        sparking = value.toInt();
        broadcastInt("sparking", sparking);
        sendInt(sparking);
    });

    webServer.on("/speed", HTTP_POST, []() {
        String value = webServer.arg("value");
        speed = value.toInt();
        broadcastInt("speed", speed);
        sendInt(speed);
    });

    webServer.on("/twinkleSpeed", HTTP_POST, []() {
        String value = webServer.arg("value");
        twinkleSpeed = value.toInt();
        if (twinkleSpeed < 0)
            twinkleSpeed = 0;
        else if (twinkleSpeed > 8)
            twinkleSpeed = 8;
        broadcastInt("twinkleSpeed", twinkleSpeed);
        sendInt(twinkleSpeed);
    });

    webServer.on("/twinkleDensity", HTTP_POST, []() {
        String value = webServer.arg("value");
        twinkleDensity = value.toInt();
        if (twinkleDensity < 0)
            twinkleDensity = 0;
        else if (twinkleDensity > 8)
            twinkleDensity = 8;
        broadcastInt("twinkleDensity", twinkleDensity);
        sendInt(twinkleDensity);
    });

    webServer.on("/solidColor", HTTP_POST, []() {
        String r = webServer.arg("r");
        String g = webServer.arg("g");
        String b = webServer.arg("b");
        setSolidColor(r.toInt(), g.toInt(), b.toInt());
        sendString(String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b));
    });

    webServer.on("/pattern", HTTP_POST, []() {
        String value = webServer.arg("value");
        setPattern(value.toInt());
        sendInt(currentPatternIndex);
    });

    webServer.on("/patternName", HTTP_POST, []() {
        String value = webServer.arg("value");
        setPatternName(value);
        sendInt(currentPatternIndex);
    });

    webServer.on("/palette", HTTP_POST, []() {
        String value = webServer.arg("value");
        setPalette(value.toInt());
        sendInt(currentPaletteIndex);
    });

    webServer.on("/paletteName", HTTP_POST, []() {
        String value = webServer.arg("value");
        setPaletteName(value);
        sendInt(currentPaletteIndex);
    });

    webServer.on("/brightness", HTTP_POST, []() {
        String value = webServer.arg("value");
        setBrightness(value.toInt());
        sendInt(brightness);
    });

    webServer.on("/autoplay", HTTP_POST, []() {
        String value = webServer.arg("value");
        setAutoplay(value.toInt());
        sendInt(autoplay);
    });

    webServer.on("/autoplayDuration", HTTP_POST, []() {
        String value = webServer.arg("value");
        setAutoplayDuration(value.toInt());
        sendInt(autoplayDuration);
    });

    //list directory
    webServer.on("/list", HTTP_GET, handleFileList);
    //load editor
    webServer.on("/edit", HTTP_GET, []() {
        if (!handleFileRead("/edit.htm"))
            webServer.send(404, "text/plain", "FileNotFound");
    });
    //create file
    webServer.on("/edit", HTTP_PUT, handleFileCreate);
    //delete file
    webServer.on("/edit", HTTP_DELETE, handleFileDelete);
    //first callback is called after the request has ended with all parsed arguments
    //second callback handles file uploads at that location
    webServer.on("/edit", HTTP_POST, []() {
        webServer.send(200, "text/plain", "");
    },
                 handleFileUpload);

    webServer.serveStatic("/", SPIFFS, "/", "max-age=86400");

    webServer.begin();
    Serial.println("HTTP web server started");

    autoPlayTimeout = millis() + (autoplayDuration * 1000);
}

void sendInt(uint8_t value)
{
    sendString(String(value));
}

void sendString(String value)
{
    webServer.send(200, "text/plain", value);
}

void broadcastInt(String name, uint8_t value)
{
    String json = "{\"name\":\"" + name + "\",\"value\":" + String(value) + "}";
    //  webSocketsServer.broadcastTXT(json);
}

void broadcastString(String name, String value)
{
    String json = "{\"name\":\"" + name + "\",\"value\":\"" + String(value) + "\"}";
    //  webSocketsServer.broadcastTXT(json);
}

void loop()
{
    // Add entropy to random number generator; we use a lot of it.
    random16_add_entropy(random(65535));

    //  dnsServer.processNextRequest();
    //  webSocketsServer.loop();
    webServer.handleClient();

    if (power == 0)
    {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        return;
    }

    static bool hasConnected = false;
    EVERY_N_SECONDS(1)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            hasConnected = false;
        }
        else if (!hasConnected)
        {
            hasConnected = true;
            Serial.print("Connected! Open http://");
            Serial.print(WiFi.localIP());
            Serial.println(" in your browser");
        }
    }

    // EVERY_N_SECONDS(10) {
    //   Serial.print( F("Heap: ") ); Serial.println(system_get_free_heap_size());
    // }

    // change to a new cpt-city gradient palette
    EVERY_N_SECONDS(secondsPerPalette)
    {
        gCurrentPaletteNumber = addmod8(gCurrentPaletteNumber, 1, gGradientPaletteCount);
        gTargetPalette = gGradientPalettes[gCurrentPaletteNumber];
    }

    EVERY_N_MILLISECONDS(40)
    {
        // slowly blend the current palette to the next
        nblendPaletteTowardPalette(gCurrentPalette, gTargetPalette, 8);
        gHue++; // slowly cycle the "base color" through the rainbow
    }

    if (autoplay && (millis() > autoPlayTimeout))
    {
        adjustPattern(true);
        autoPlayTimeout = millis() + (autoplayDuration * 1000);
    }

    // Call the current pattern function once, updating the 'leds' array
    patterns[currentPatternIndex].pattern();

    FastLED.show();

    // insert a delay to keep the framerate modest
    FastLED.delay(1000 / FRAMES_PER_SECOND);
}

void loadSettings()
{
    brightness = EEPROM.read(0);

    currentPatternIndex = EEPROM.read(1);
    if (currentPatternIndex < 0)
        currentPatternIndex = 0;
    else if (currentPatternIndex >= patternCount)
        currentPatternIndex = patternCount - 1;

    byte r = EEPROM.read(2);
    byte g = EEPROM.read(3);
    byte b = EEPROM.read(4);

    if (r == 0 && g == 0 && b == 0)
    {
    }
    else
    {
        solidColor = CRGB(r, g, b);
    }

    power = EEPROM.read(5);

    autoplay = EEPROM.read(6);
    autoplayDuration = EEPROM.read(7);

    currentPaletteIndex = EEPROM.read(8);
    if (currentPaletteIndex < 0)
        currentPaletteIndex = 0;
    else if (currentPaletteIndex >= paletteCount)
        currentPaletteIndex = paletteCount - 1;
}

void setPower(uint8_t value)
{
    power = value == 0 ? 0 : 1;

    EEPROM.write(5, power);
    EEPROM.commit();

    broadcastInt("power", power);
}

void setAutoplay(uint8_t value)
{
    autoplay = value == 0 ? 0 : 1;

    EEPROM.write(6, autoplay);
    EEPROM.commit();

    broadcastInt("autoplay", autoplay);
}

void setAutoplayDuration(uint8_t value)
{
    autoplayDuration = value;

    EEPROM.write(7, autoplayDuration);
    EEPROM.commit();

    autoPlayTimeout = millis() + (autoplayDuration * 1000);

    broadcastInt("autoplayDuration", autoplayDuration);
}

void setSolidColor(CRGB color)
{
    setSolidColor(color.r, color.g, color.b);
}

void setSolidColor(uint8_t r, uint8_t g, uint8_t b)
{
    solidColor = CRGB(r, g, b);

    EEPROM.write(2, r);
    EEPROM.write(3, g);
    EEPROM.write(4, b);
    EEPROM.commit();

    setPattern(patternCount - 1);

    broadcastString("color", String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b));
}

// increase or decrease the current pattern number, and wrap around at the ends
void adjustPattern(bool up)
{
    if (up)
        currentPatternIndex++;
    else
        currentPatternIndex--;

    // wrap around at the ends
    if (currentPatternIndex < 0)
        currentPatternIndex = patternCount - 1;
    if (currentPatternIndex >= patternCount)
        currentPatternIndex = 0;

    if (autoplay == 0)
    {
        EEPROM.write(1, currentPatternIndex);
        EEPROM.commit();
    }

    broadcastInt("pattern", currentPatternIndex);
}

void setPattern(uint8_t value)
{
    if (value >= patternCount)
        value = patternCount - 1;

    currentPatternIndex = value;

    if (autoplay == 0)
    {
        EEPROM.write(1, currentPatternIndex);
        EEPROM.commit();
    }

    broadcastInt("pattern", currentPatternIndex);
}

void setPatternName(String name)
{
    for (uint8_t i = 0; i < patternCount; i++)
    {
        if (patterns[i].name == name)
        {
            setPattern(i);
            break;
        }
    }
}

void setPalette(uint8_t value)
{
    if (value >= paletteCount)
        value = paletteCount - 1;

    currentPaletteIndex = value;

    EEPROM.write(8, currentPaletteIndex);
    EEPROM.commit();

    broadcastInt("palette", currentPaletteIndex);
}

void setPaletteName(String name)
{
    for (uint8_t i = 0; i < paletteCount; i++)
    {
        if (paletteNames[i] == name)
        {
            setPalette(i);
            break;
        }
    }
}

void adjustBrightness(bool up)
{
    if (up && brightnessIndex < brightnessCount - 1)
        brightnessIndex++;
    else if (!up && brightnessIndex > 0)
        brightnessIndex--;

    brightness = brightnessMap[brightnessIndex];

    FastLED.setBrightness(brightness);

    EEPROM.write(0, brightness);
    EEPROM.commit();

    broadcastInt("brightness", brightness);
}

void setBrightness(uint8_t value)
{
    if (value > 255)
        value = 255;
    else if (value < 0)
        value = 0;

    brightness = value;

    FastLED.setBrightness(brightness);

    EEPROM.write(0, brightness);
    EEPROM.commit();

    broadcastInt("brightness", brightness);
}