/** 
* Provides functionality to easier handle pixel data in BMP image files.
*
* Completion time: 5 hours
* 
* @author Muhammad Raj
* @version 1.0
*
*/

#include "PixelProcessor.h"
#include <stdio.h>
#include <stdlib.h>

void colorShiftPixels(struct Pixel** pArr, int width, int height, int rShift, int gShift, int bShift){
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            pArr[i][j].red = pArr[i][j].red + rShift;
            if(pArr[i][j].red > 255){
                pArr[i][j].red = 255;
            }
            else if(pArr[i][j].red < 0){
                pArr[i][j].red = 0;
            }
            pArr[i][j].green = pArr[i][j].green + gShift;
            if(pArr[i][j].green > 255){
                pArr[i][j].green = 255;
            }
            else if(pArr[i][j].green < 0){
                pArr[i][j].green = 0;
            }
            pArr[i][j].blue = pArr[i][j].blue + bShift;
            if(pArr[i][j].blue > 255){
                pArr[i][j].blue = 255;
            }
            else if(pArr[i][j].blue < 0){
                pArr[i][j].blue = 0;
            }
        }
    }
}