#include "mbed.h"

#include "GD2.h"

#include "stdio.h"

#define M_PI 3.14159265358979323846

//initialising inputs

InterruptIn button(PTD7);
AnalogIn strainInput(PTB0);
AnalogOut motorOutput(PTE30);
DigitalOut led(LED1);
DigitalOut flash(LED4);
DigitalIn level4(PTC4);
DigitalIn level3(PTC3);
DigitalIn level2(PTC0);
DigitalIn level1(PTC7);

InterruptIn noSweatButton(PTA5);   //
InterruptIn sweatButton(PTA4);     //pin numbers subject to change
InterruptIn bigSweatButton(PTA12);  //

GDClass GD(PTD2,PTD3,PTD1,PTD5,PTD0); //mosi,miso,sck,gd,sd



//initialising global variables

Timer t;

int time1 = 0;

int time2 = 0;

bool timeFlag = false;

float strain = 0.0;  //input strain to be displayed on screen 

float threshold1 = 0.2;  //represents percentage of input i.e. 25% of input value
float threshold2 = 0.4;   //also represents thresholding for strain input
float threshold3 = 0.6;
float threshold4 = 0.8;
float threshold5 = 1.0;

bool noSweatFlag = false;
bool sweatFlag = false;
bool bigSweatFlag = true;

void setNoSweat(){
    sweatFlag = false;
    bigSweatFlag = false;
    noSweatFlag = true;
}

void setSweat(){
    noSweatFlag = false;
    bigSweatFlag = false;
    sweatFlag = true;
}

void setBigSweat(){
    sweatFlag = false;
    noSweatFlag = false;
    bigSweatFlag = true;
}


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
    
    char straindd[4];



    //calculating circumference at start so it doesnt need to be recalculated every loop as floating point multiplications are computationally intensive

    circumference = M_PI * 0.6604; //26" in metres

    t.start();

    button.fall(&changeFlag);  // attach the address of the flip function to the rising edge
    
    noSweatButton.fall(&setNoSweat);        //sets up interrupts so that flag will switch
    sweatButton.fall(&setSweat);            //when button is pressed to designated mode
    bigSweatButton.fall(&setBigSweat);

    

    

    while(1) {   

        GD.ClearColorRGB(0x000000); //black background

        GD.Clear();   //screen has to always be cleared before drawing a new one

        

        if (timeFlag == true){

            recordTime();

            timeFlag = false;

        }
        
        
        

        GD.Begin(RECTS);

        

        GD.ColorRGB(0x0D3D56); //indigo screen               also 0x0C374D - darker indigo       0x093145 - darkest indigo

        GD.Vertex2ii(4,4); //screen

        GD.Vertex2ii(476,268);

        //draws empty battery symbol

        GD.ColorRGB(0x000000); //black line

        GD.Vertex2ii(238,4); //top to bottom line 

        GD.Vertex2ii(242,268);

        GD.Vertex2ii(242,134); //battery outline

        GD.Vertex2ii(476,138);

        

        GD.ColorRGB(0xffffff);

        GD.Vertex2ii(298,178); //larger battery section

        GD.Vertex2ii(422,232);

        GD.Vertex2ii(423,193); //wee knobbly thing at the front

        GD.Vertex2ii(430,215);

        //strain stuff
        
        //strain = strainInput.read();
        
        //sprintf(straindd,"%f",strain); //casts integer strain to string

        //GD.cmd_text(350, 70, 31, OPT_CENTER, straindd); //writes strain on screen





        velocity = calculateVelocity(circumference); //calculates velocity in m/s

        

        sprintf(screend,"%d",velocity); //casts integer velocity to string

        GD.cmd_text(70, 136, 31, OPT_CENTER, screend); //writes velocity on screen

        GD.cmd_text(120, 142, 28, OPT_CENTER, "m/s"); //appends the m/s beside it in a smaller font


        if(level4 == 0){

            //draws rectange for when battery is full capacity

            GD.Begin(RECTS);

            GD.ColorRGB(0x4cc417); //sets rectangle to apple green colour

            GD.Vertex2ii(395,180);

            GD.Vertex2ii(420,230);

        }

        if(level3 == 0){

            //will draw the block when battery capacity at 75%

            GD.Begin(RECTS);

            GD.ColorRGB(0x4cc417); 

            GD.Vertex2ii(365,180);

            GD.Vertex2ii(390,230);

        }

        if(level2 == 0){

            //will draw block when battery at 50%

            GD.Begin(RECTS);

            GD.ColorRGB(0x4cc417); 

            GD.Vertex2ii(335,180);

            GD.Vertex2ii(360,230);

        }

        if(level1 == 0){

            //will draw block at 25%

            GD.Begin(RECTS);

            GD.ColorRGB(0x4cc417); 

            GD.Vertex2ii(300,180);

            GD.Vertex2ii(330,230);

        }
        if(noSweatFlag == true){
            GD.cmd_text(355, 70, 31, OPT_CENTER, "NO SWEAT"); //writes strain on screen
            if(strainInput > 0.0 && strainInput <= threshold1){
                motorOutput.write(0.0);
            }
            else if (strainInput > threshold1 && strainInput <= threshold2){
                motorOutput.write(0.25); //outputs 25% of motor power
            }
            else if (strainInput > threshold2 && strainInput <= threshold3){
                motorOutput.write(0.5);  //outputs 50% of motor power
            }
            else if (strainInput > threshold3 && strainInput <= threshold4){
                motorOutput.write(0.75);
            }
            else if (strainInput > threshold4 && strainInput <= threshold5){
                motorOutput.write(1.0);
            }
        }
        
        
        else if(sweatFlag == true){
            GD.cmd_text(355, 70, 31, OPT_CENTER, "SWEAT"); //writes strain on screen
            if(strainInput > 0.0 && strainInput <= threshold2){
                motorOutput.write(0.0);
            }
            else if (strainInput > threshold2 && strainInput <= threshold3){
                motorOutput.write(0.25);  
            }
            else if (strainInput > threshold3 && strainInput <= threshold4){
                motorOutput.write(0.5);
            }
            else if (strainInput > threshold4 && strainInput <= threshold5){
                motorOutput.write(0.75);
            }
        }
        else{ //basically says if bigSweatFlag == true but also sets this mode as a default for safety reasons
            GD.cmd_text(355, 70, 31, OPT_CENTER, "BIG SWEAT"); //writes strain on screen
            if(strainInput > 0.0 && strainInput <= threshold3){
                motorOutput.write(0.0);
            }
            else if (strainInput > threshold3 && strainInput <= threshold4){
                motorOutput.write(0.25);
            }
            else if (strainInput > threshold4 && strainInput <= threshold5){
                motorOutput.write(0.5);
            }
        }
      

        GD.swap(); //draws the image
    }
}