#ifndef FSM_H__
#define FSM_H__

uint8_t state = 0;

void init_FSM();
void state_startup();

void state_startup()
{
    Serial.println("Blink 1");
    for (size_t i = 0; i < 6; i++)
    {
        blink_color(CHSV(255, 180, 180));

        delay(75);
    }
}

void state_wifi()
{
    state = 1;
    for (size_t i = 0; i < 12; i++)
    {
        blink_color(CHSV(122, 122, 122));
        delay(35);
    }
    Serial.println("Blink Wifi done");
}

void state_rotating()
{
    state = 2;
    // Do the rotating part with sensors

    Serial.println("Rotating right now");
}

void state_prize()
{
    int delayMin = 35;
    int delayMax = 250;
    int delayDecr = 25;

    int delayer = delayMax;

    Serial.println("Prize detected");
    while (1)
    {
        blink_color(CHSV(255, 255, 255));
        if (delayer > 0)
            delay(delayer);
        else 
            break;
        delayer -= delayDecr;
    }
    for (size_t i = 0; i < 12; i++)
    {
        blink_color(CHSV(random8(), 255, 255));
        delay(delayer);
    }
    Serial.println("Prize detection done.");
    for (size_t i = 0; i < 4; i++)
    {
        pulse(CHSV(random8(), 255, 255));
        for (size_t i = 0; i < 15; i++)
        {
            delay(delayMin);
            fade(30);
        }
    }
    Serial.println("State prize done");
    state = 3;
}

void state_idle()
{
    state = 3;
    // Do the rotating part with sensors

    Serial.println("Doing nothing goto program");
}

#endif // FSM_H__