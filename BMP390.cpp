
#include "BMP390.h"
using namespace std;
//BufferedSerial pc(USBTX,USBRX);
//Serial pc(USBTX, USBRX);
BMP390::BMP390(PinName sda,PinName scl):bmp(sda, scl)
{
    //pc.set_baud(9600);
    //pc.baud(9600);
    bmp.frequency(40000);
    ThisThread::sleep_for(100ms);
      //resetがないと電源再投入する必要がある(you need to power on BMP390 without softreset)
      //プログラムを修正して読み込ませるときにいちいち電源を入れ直す必要が生じるということ，だった気がする
      //(because you need to power on BMP390　again every time the program is modified and loaded.)
    writedata(0x7E, 0xB6);//softreset

    float par_t1 = pow(2.0, 8.0);
    float par_t2 = pow(2.0, -30.0);
    float par_t3 = pow(2.0, -48.0);
    float par_p1 = pow(2.0, -20.0);
    float par_p2 = pow(2.0, -29.0);
    float par_p3 = pow(2.0, -32.0);
    float par_p4 = pow(2.0, -37.0);
    float par_p5 = pow(2.0, 3.0);
    float par_p6 = pow(2.0, -6.0);
    float par_p7 = pow(2.0, -8.0);
    float par_p8 = pow(2.0, -15.0);
    float par_p9 = pow(2.0, -48.0);
    float par_p10 = pow(2.0, -48.0);
    float par_p11 = pow(2.0, -65.0);
    
    while(!((readdata(0x03)) == 0b00010000));//p32
    
    char buf[2];
    //8bit
    NVM_PAR_P11 = ((short)(readdata(0x45)));
    NVM_PAR_P10 = ((short)(readdata(0x44)));
    NVM_PAR_P8 = ((short)(readdata(0x41)));
    NVM_PAR_P7 = ((short)(readdata(0x40)));
    NVM_PAR_P4 = ((short)(readdata(0x3B)));
    NVM_PAR_P3 = ((short)(readdata(0x3A)));
    NVM_PAR_T3 = ((short)(readdata(0x35)));
    
    //16bit
    readdatas(0x42, buf, 2);
    NVM_PAR_P9 = ((short)(buf[1]<<8 | buf[0]));
    readdatas(0x3E, buf, 2);
    NVM_PAR_P6 = ((unsigned short)(buf[1]<<8 | buf[0]));
    readdatas(0x3C, buf, 2);
    NVM_PAR_P5 = ((unsigned short)(buf[1]<<8 | buf[0]));
    readdatas(0x38, buf, 2);
    NVM_PAR_P2 = ((short)(buf[1]<<8 | buf[0]));
    readdatas(0x36, buf, 2);
    NVM_PAR_P1 = ((short)(buf[1]<<8 | buf[0]));
    readdatas(0x33, buf, 2);
    NVM_PAR_T2 = ((unsigned short)(buf[1]<<8 | buf[0]));
    readdatas(0x31, buf, 2);
    NVM_PAR_T1 = ((unsigned short)(buf[1]<<8 | buf[0]));

    PAR_T1 = NVM_PAR_T1 * par_t1;
    PAR_T2 = NVM_PAR_T2 * par_t2;
    PAR_T3 = NVM_PAR_T3 * par_t3;
    PAR_P1 = (NVM_PAR_P1 - pow(2.0, 14.0)) * par_p1;
    PAR_P2 = (NVM_PAR_P2 - pow(2.0, 14.0)) * par_p2;
    PAR_P3 = NVM_PAR_P3 * par_p3;
    PAR_P4 = NVM_PAR_P4 * par_p4;
    PAR_P5 = NVM_PAR_P5 * par_p5;
    PAR_P6 = NVM_PAR_P6 * par_p6;
    PAR_P7 = NVM_PAR_P7 * par_p7;
    PAR_P8 = NVM_PAR_P8 * par_p8;
    PAR_P9 = NVM_PAR_P9 * par_p9;
    PAR_P10 = NVM_PAR_P10 * par_p10;
    PAR_P11 = NVM_PAR_P11 * par_p11;

}

void BMP390::init(char osr_t,char osr_p,char iir){
    OSR_T=osr_t;
    OSR_P=osr_p;
    IIR=iir;
    
    char buf = 0b00010011;//enable pressure and temperature and select forced mode
    writedata(0x1B, buf);//p36
    buf = 0b00000000;//初期化
    buf=((OSR_T<<3)|(OSR_P));//論理和//0bは2進数//<<は左シフト，>>は右シフト
    writedata(0x1C,buf);//config//p37
    
    buf=0;
    buf=(IIR<<1);//short_in(p30)what's short_in?
    writedata(0x1f,buf);

}

char BMP390::readdata(char adr){
    
    char data[1] = {adr};
    while(bmp.write(ADD1, data , 1, false));
    data[0] = 0;
    while(bmp.read(ADD1 | 1, data, 1, false));
    return data[0];
    
}

void BMP390::writedata(char adr,char data){
    char senddata[2] = {adr, data};
    while(bmp.write(ADD1, senddata, 2, false));
}

void BMP390::readdatas(char adr,char *data,int num){
    char addre[1] = {adr};
    while(bmp.write(ADD1, addre , 1, false));
    while(bmp.read(ADD1 | 1, data, num, false));//これでいいの？//サンプルコードとかを確認すること
    
    
}

float BMP390::bmp390_compensate_temperature(int raw_temp){
    
    float partial_data1;
    float partial_data2;
    partial_data1 = (float)(raw_temp - PAR_T1);
    partial_data2 = (float)(partial_data1 * PAR_T2);
    /* Update the compensated temperature in calib structure since this is
    * needed for pressure calculation */
    t_lin = partial_data2 + (partial_data1 * partial_data1) * PAR_T3;
    //Returns compensated temp

    return t_lin;
    
}

float BMP390::bmp390_compensate_pressure(int raw_press){
    //Variable to store the compensated pressure 
    float comp_press;
    //Temporary variables used for compensation
    float partial_data1;
    float partial_data2;
    float partial_data3;
    float partial_data4;
    float partial_out1;
    float partial_out2;
    //Calibration data
    partial_data1 = PAR_P6 * t_lin;
    partial_data2 = PAR_P7 * (t_lin * t_lin);
    partial_data3 = PAR_P8 * (t_lin * t_lin * t_lin);
    partial_out1 = PAR_P5 + partial_data1 + partial_data2 + partial_data3;

    partial_data1 = PAR_P2 * t_lin;
    partial_data2 = PAR_P3 * (t_lin * t_lin);
    partial_data3 = PAR_P4 * (t_lin * t_lin * t_lin);
    partial_out2 = (float)raw_press * (PAR_P1 + partial_data1 + partial_data2 + partial_data3);

    partial_data1 = (float)raw_press * (float)raw_press;
    partial_data2 = PAR_P9 + PAR_P10 * t_lin;
    partial_data3 = partial_data1 * partial_data2;
    partial_data4 = partial_data3 + ((float)raw_press * (float)raw_press * (float)raw_press) * PAR_P11;
    comp_press = partial_out1 + partial_out2 + partial_data4;

    return comp_press;
    
}

bool BMP390::update(){
    if((readdata(0x03)) == 0b01110000){
    
    char data[3];
    
    readdatas(0x07,data,3);
    int raw_temp = (int)((data[2]<<16)|(data[1]<<8)|data[0]);
    
    readdatas(0x04,data,3);
    int raw_press = (int)((data[2]<<16)|(data[1]<<8)|data[0]);

    //init関数と同じことをする。forced modeだから
    char buf = 0b00010011;//enable pressure and temperature and select forced mode
    writedata(0x1B, buf);//p36
    buf = 0b00000000;//初期化
    buf=((OSR_T<<3)|(OSR_P));//論理和//0bは2進数//<<は左シフト，>>は右シフト
    writedata(0x1C,buf);//config//p37
    
    buf=0;
    buf=(IIR<<1);
    writedata(0x1f,buf);
 
    temp = bmp390_compensate_temperature(raw_temp);
    press = bmp390_compensate_pressure(raw_press);
    
    return true;
    }else {
        return false;
    }
}

float BMP390::getpress(){
    return press;
}

float BMP390::gettemp(){
    return temp;
}
