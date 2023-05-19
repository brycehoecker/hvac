/*
  Read the various configuration files that have evolved with
  the VERITAS software
  
  mkd 5/9/04
*/

//project specific includes
#include "VReadConfig.h"

using namespace std;

int VReadConfig::ReadVDCMon(char filename[]){

 ifstream infile;

 cout << "Opening " << filename << endl;
 infile.open(filename);
 if(!infile.is_open()){
    cerr << filename << " did not open" << endl;
    return -1;
 }//if
 
//read the initial crap
 short int crap;
 char que;

 infile >> num_pixels;
// cout << num_pixels << endl;
 infile >> que >> crap;
 if(que=='h') humidity_sensors=crap;
 infile >> que >> crap;
 if(que=='p') position_sensors=crap;
 infile >> que >> crap;
 if(que=='t') temperature_sensors=crap;
 infile >> que >> crap;
 if(que=='b') brightness_sensors=crap;
 
 infile >> angular_pixel_size;
 infile >> display_scale;
 infile >> display_x_offset;
 infile >> display_y_offset;

 short int anal_number,pixel;
 short int v1, v2, v6;
 float v3, v4, v5;
//and now the fun begins...
 for(short int i=1;i<=num_pixels;i++){
    infile >> anal_number >> pixel
           >> v1 >> v2 >> v3 >> v4 >> v5 >> v6;//allow the numbers to catch up with themselves
//     cout << anal_number << " " 
//          << pixel << " " 
//          << v1 << " " 
//          << v2 << " " 
//          << v3 << " " 
//          << v4 << " " 
//          << v5 << " " 
//          << v6 << " "; 
    sector_number[pixel] = v1;
    channel_number[pixel] = v2;
    x[pixel] = v3;
    y[pixel] = v4;
    status[pixel] = v6;
//     cout << "* " << status[pixel] << " *" << endl;
 }//for

//add in the stuff about the other sensors later
//but as they don't exist yet shall stop reading here.

 infile.close();
 return 0;

};//ReadVDCMon

int VReadConfig::ReadHV(char filename[])
{

 ifstream infile;

 cout << "Opening " << filename << endl;
 infile.open(filename);
 if(!infile.is_open()){
    cerr << filename << " did not open" << endl;
    return -1;
 }//if

 short int pixel,v1,v2,v3,v4;
 for(short int i=0;i<499;i++){
   infile >> pixel >> v1 >> v2 >> v3 >> v4;
   hv_slot[pixel]=v2;
   hv_channel[pixel]=v3;
   voltage[pixel]=v4;
 }//read for

 return 0;
  
}
