#include "mbed.h"
#include "GD2.h"
#include "stdio.h"
#define M_PI 3.14159265358979323846
//initialising inputs
InterruptIn button(PTD7);
DigitalOut led(LED1);
DigitalOut flash(LED4);
DigitalIn level4(PTC4);
DigitalIn level3(PTC3);
DigitalIn level2(PTC0);
DigitalIn level1(PTC7);
GDClass GD(PTD2,PTD3,PTD1,PTD5,PTD0); //mosi,miso,sck,gd,sd

//initialising global variables
Timer t;
int time1 = 0;
int time2 = 0;
bool timeFlag = false;

//used to register the time when the magnet passes hall effect sensor
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

//called in interrupt so MC knows magnet has passed hall effect
void changeFlag(){ 
    timeFlag = true;
}

//figures out how much time has passed since last revolution and uses v = s/t to calculate velocity in m/s
int calculateVelocity(float circumference){
    int timeDiffRaw = 0;
    float timeDiff;
    int velocity;    
    
    timeDiffRaw = time1 - time2;
    timeDiff = timeDiffRaw/1000.0;
    velocity = circumference / timeDiff;
    return velocity;
}



int main() {
    //initialising variables
    GD.begin(~GD_STORAGE);
    float circumference;
    int velocity;
    char screend[2];

    //calculating circumference at start so it doesnt need to be recalculated every loop as floating point multiplications are computationally intensive
    circumference = M_PI * 0.6604; //26" in metres
    t.start();
    button.rise(&changeFlag);  // attach the address of the flip function to the rising edge
    
    
    while(1) {   
        GD.ClearColorRGB(0x103000);
        GD.Clear();   //screen has to always be cleared before drawing a new one
        
        if (timeFlag == true){
            recordTime();
            timeFlag = false;
        }
        velocity = calculateVelocity(circumference); //calculates velocity in m/s
        
        sprintf(screend,"%d",velocity); //casts integer velocity to string
        GD.cmd_text(70, 136, 31, OPT_CENTER, screend); //writes velocity on screen
        GD.cmd_text(120, 142, 28, OPT_CENTER, "m/s"); //appends the m/s beside it in a smaller font
        
        
        //draws empty battery symbol
        GD.Begin(RECTS);
        GD.Vertex2ii(298,198); //larger battery section
        GD.Vertex2ii(422,252);
        GD.Vertex2ii(423,213); //wee knobbly thing at the front
        GD.Vertex2ii(430,235);
        
        
        
        if(level4 == 1){
            //draws rectange for when battery is full capacity
            GD.Begin(RECTS);
            GD.ColorRGB(0x4cc417); //sets rectangle to apple green colour
            GD.Vertex2ii(395,200);
            GD.Vertex2ii(420,250);
        }
        if(level3 == 1){
            //will draw the block when battery capacity at 75%
            GD.Begin(RECTS);
            GD.ColorRGB(0x4cc417); 
            GD.Vertex2ii(365,200);
            GD.Vertex2ii(390,250);
        }
        if(level2 == 1){
            //will draw block when battery at 50%
            GD.Begin(RECTS);
            GD.ColorRGB(0x4cc417); 
            GD.Vertex2ii(335,200);
            GD.Vertex2ii(360,250);
        }
        if(level1 == 1){
            //will draw block at 25%
            GD.Begin(RECTS);
            GD.ColorRGB(0x4cc417); 
            GD.Vertex2ii(300,200);
            GD.Vertex2ii(330,250);
        }
        
        
        
        
        GD.swap(); //draws the image
    }
}
