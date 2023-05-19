#ifndef VREADCONFIG_H
#define VREADCONFIC_h

//general includes
#include <iostream> //for writing to screen
#include <fstream>  //for writing to file

class VReadConfig{
private:

//for current monitor

       short int num_pixels;
       short int humidity_sensors;
       short int position_sensors;
       short int temperature_sensors;
       short int brightness_sensors;
       float angular_pixel_size;
       float display_scale;
       float display_x_offset;
       float display_y_offset;
       short int sector_number[499];
       short int channel_number[499];
       float x[499],y[499];
       short int status[499];
  //humidity sensors [non-existent so far]
  // h <number> <board> <channel> <offset> <slope> <tempcorr> <conversion> <state>
  //position sensors [non-existent so far]
  // p <number> <board> <channel> <threshold> <state>
  //temperature sensors [non-existent so far]
  // t <number> <board> <channel> <coeff0> <coeff1> <state>
  //brightness sensors [non-existent so far]
  // b <number> <board> <channel> <threshold> <state>

//for HV

       short int hv_slot[499];
       short int hv_channel[499];
       short int voltage[499];

public:
       VReadConfig(){
        for(short int i=0;i<499;i++){
            sector_number[i]=channel_number[i]=status[i]=-1;
            x[i]=y[i]=0;
        }//for
       };//constructor
       ~VReadConfig(){};//destructor
       int ReadVDCMon(char filename[]);
       int ReadHV(char filename[]);
       short int getPixelStatus(short int pixel){return status[pixel];};
       short int getSector(short int pixel){return sector_number[pixel];};
       short int getChannel(short int pixel){return channel_number[pixel];};
       float getX(short int pixel){return x[pixel];};
       float getY(short int pixel){return y[pixel];};
       short int getNumPixels(void){return num_pixels;};
       short int getNumHumiditySensors(void){return humidity_sensors;};
       short int getNumPositionSensors(void){return position_sensors;};
       short int getNumTemperatureSensors(void){return temperature_sensors;};
       short int getNumBrightnessSensors(void){return brightness_sensors;};
       float getAngularPixelSize(void){return angular_pixel_size;};
};

#endif
