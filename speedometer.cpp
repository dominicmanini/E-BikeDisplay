#include "mbed.h"
#define M_PI 3.14159265358979323846
 
InterruptIn button(PTD7);
DigitalOut led(LED1);
DigitalOut flash(LED4);
 
Timer t;
int time1 = 0;
int time2 = 0;
bool timeFlag = false;

void recordTime() {
    time2 = time1;
    time1 = t.read_ms();
    
    if (time1 > 1620000){  //As timer can't go over roughly 34 minutes we reset it
        t.stop();
        t.reset();
        t.start();
        time2 = 0;
        time1 = t.read_ms();
    }
}

void changeFlag(){
    timeFlag = true;
}

int calculateVelocity(float circumference){
    int timeDiffRaw = 0;
    float timeDiff;
    int velocity;    
    
    timeDiffRaw = time1 - time2;
    timeDiff = timeDiffRaw/1000.0;
    velocity = circumference * timeDiff;
    return velocity;
}



int main() {
    float circumference;
    int velocity;

    circumference = M_PI * 0.6604; //26" in metres
    t.start();
    button.rise(&changeFlag);  // attach the address of the flip function to the rising edge
    
    
    while(1) {           // wait around, interrupts will interrupt this!
        if (timeFlag == true){
            recordTime();
            timeFlag = false;
        }
        velocity = calculateVelocity(circumference); //calculates velocity in m/s
        
    }
}
