/*
MIT License

Copyright (c) 2021 jp-96

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "inttypes.h"
#include "MicroBit.h"

#define K (uint64_t)60000000

MicroBit uBit;
MicroBitPin sensorPin(MICROBIT_ID_IO_P2, MICROBIT_PIN_P2, PIN_CAPABILITY_DIGITAL);

uint64_t lastFallTimestamp=0;
uint32_t fallCount=0;
MicroBitImage IMG_FALL("1,1,1,0,0\n0,0,1,0,0\n0,0,1,0,0\n0,0,1,0,0\n0,0,1,1,1\n");

uint64_t t[] = {0, 0, 0, 0};

void onEdgeSensorPin(MicroBitEvent e)
{
    int cadence;
    int speed10;

    lastFallTimestamp = e.timestamp;
    fallCount++;

    t[0]=t[1];
    t[1]=t[2];
    t[2]=t[3];
    t[3] = lastFallTimestamp;

    if (fallCount>3) {
        cadence = (int)( K* 3/(t[3]-t[0]));
        //speed10 = (int)( K*10/(t[3]-t[0]));
        speed10 = (int)( K* 9/(t[3]-t[0]));
    } else {
        cadence = 0;
        speed10 = 0;
    }  

    uBit.serial.printf("%" PRIu32 ", %" PRIu32 ", %d, %d\r\n"
        , fallCount, (uint32_t)e.timestamp, cadence, speed10);
}

void setup(void)
{
    uBit.serial.printf("#, Timestamp, Cadence, Speed10\r\n");

    uBit.messageBus.listen(MICROBIT_ID_IO_P2, MICROBIT_PIN_EVT_FALL, onEdgeSensorPin);
    sensorPin.getDigitalValue(PullUp);
    sensorPin.eventOn(MICROBIT_PIN_EVENT_ON_EDGE);

    uBit.display.scrollAsync("GO!");
}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();

    create_fiber(setup);

    // If main exits, there may still be other fibers running or registered event handlers etc.
    // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
    // sit in the idle task forever, in a power efficient sleep.
    release_fiber();
}
