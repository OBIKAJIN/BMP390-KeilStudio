//example
//using NUCLEO F446RE
//BMP390(Adafruit)  
// for I2C
/*
#include "mbed.h"
#include "BMP390.h"

BMP390 bmp390(PB_9,PB_8);
BufferedSerial pc(USBTX,USBRX);

// main() runs in its own thread in the OS
int main(){
    pc.set_baud(9600);
    bmp390.init(OSR_T1, OSR_P2, IIR_0);

    while (true) {
        while(!bmp390.update());
        printf("pressure = %d\n", (int)bmp390.getpress());
        printf("temperature = %d\n", (int)bmp390.gettemp());
        printf("\n");  

        ThisThread::sleep_for(1000ms);

    }
}
*/



#ifndef BMP390_H
#define BMP390_H

#include "mbed.h"
//pressure//p37
#define OSR_P1 0b000//*1
#define OSR_P2 0b001//*2
#define OSR_P4 0b010//*4
#define OSR_P8 0b011//*8
#define OSR_P16 0b100//*16
#define OSR_P32 0b101//*32

//temperature//p37
#define  OSR_T1 0b000//*1
#define  OSR_T2 0b001//*2
#define  OSR_T4 0b010//*4
#define  OSR_T8 0b011//*8
#define  OSR_T16 0b100//*16
#define  OSR_T32 0b101//*32

//IIR
#define IIR_0 0b000//off
#define IIR_4 0b100
#define IIR_16 0b111

//OSR_1(0b001),OSR_4(0b011),IIR_0(0b000)


#define ADD1 0x77 << 1//defalt in case of adafruit BMP390 //p41
#define ADD2 0x76 << 1

class BMP390 {
public:
    BMP390(PinName sda,PinName scl);
    
    void init(char osr_t, char osr_p, char iir);
    char readdata(char adr);
    void writedata(char adr, char data);
    void readdatas(char adr, char *data, int num);
    
    float bmp390_compensate_temperature(int raw_temp);
    float bmp390_compensate_pressure(int raw_press);
    bool update();
    float getpress();
    float gettemp();
    
 private:
    I2C bmp;
    
    
    unsigned short NVM_PAR_P6, NVM_PAR_P5, NVM_PAR_T2, NVM_PAR_T1;
    short NVM_PAR_P11, NVM_PAR_P10, NVM_PAR_P9, NVM_PAR_P8, NVM_PAR_P7, NVM_PAR_P4, NVM_PAR_P3, NVM_PAR_P2, NVM_PAR_P1, NVM_PAR_T3;

    float PAR_T1, PAR_T2, PAR_T3;
    float PAR_P1, PAR_P2, PAR_P3, PAR_P4, PAR_P5, PAR_P6, PAR_P7, PAR_P8, PAR_P9, PAR_P10, PAR_P11;
    float t_lin;
    float press, temp;
    
    char OSR_T;
    char OSR_P;
    char IIR;
    

};

#endif


