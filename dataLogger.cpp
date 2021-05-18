#include "mbed.h"
AnalogIn input(PTE30);
Timer t;
DigitalIn writeLine(PTD7);
float strain = 0.0;
float time1 = 0;


int main() {
    t.start();
    if(writeLine == 1){
    FILE *fp;

    fp = fopen("/tmp/data.txt", "a+");
        while(1) {
            strain = input.read();
            time1 = t.read();
            fprintf(fp, "%f %f \n", strain, time1);
            wait(0.25);
        }
    fclose(fp);
    }
    t.stop();
}