#include "mbed.h"


AnalogIn input(PTB0);
AnalogOut output(PTE30);

int main()
{
    while (true) {
        if (input > 0.1){  //if the input is greater than 0.33V
        output.write(0.33); //should roughly output 1V
        }
        else{
            output.write(0.1); //otherwise outputs 0.33V
        }
    }
}