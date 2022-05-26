/** 
* Loads input BPM/PPM image file and converts it into BPM/PPM image. Also capable of optional
* color shifting of input images.
*
* Completion time: 3 days
* 
* @author Muhammad Raj
* @version 1.0
*
*/

#include "BmpProcessor.h"
#include "PixelProcessor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void readBMPHeader(FILE* file, struct BMP_Header* header){   
    fread(&header->signature, sizeof(char)*2, 1, file);
    fread(&header->size, sizeof(int), 1, file);
    fread(&header->reserved1, sizeof(short), 1, file);
    fread(&header->reserved2, sizeof(short), 1, file);
    fread(&header->offset_pixel_array, sizeof(int), 1, file);
}

void writeBMPHeader(FILE* file, struct BMP_Header* header){
    fwrite(&header->signature, 1, sizeof(char)*2, file);
    fwrite(&header->size, 1, sizeof(int), file);
    fwrite(&header->reserved1, 1, sizeof(short), file);
    fwrite(&header->reserved2, 1, sizeof(short), file);
    fwrite(&header->offset_pixel_array, 1, sizeof(int), file);
}

void readDIBHeader(FILE* file, struct DIB_Header* header){
    fread(&header->size, sizeof(int), 1, file);
    fread(&header->width, sizeof(int), 1, file);
    fread(&header->height, sizeof(int), 1, file);
    fread(&header->planes, sizeof(short), 1, file);
    fread(&header->bits_per_pixel, sizeof(short), 1, file);
    fread(&header->compression, sizeof(int), 1, file);
    fread(&header->image_size, sizeof(int), 1, file);
    fread(&header->x_pixels_per_meter, sizeof(int), 1, file);
    fread(&header->y_pixels_per_meter, sizeof(int), 1, file);
    fread(&header->colors_in_color_table, sizeof(int), 1, file);
    fread(&header->important_color_count, sizeof(int), 1, file);
}

void writeDIBHeader(FILE* file, struct DIB_Header* header){
    fwrite(&header->size, 1, sizeof(int), file);
    fwrite(&header->width, 1, sizeof(int), file);
    fwrite(&header->height, 1, sizeof(int), file);
    fwrite(&header->planes, 1, sizeof(short), file);
    fwrite(&header->bits_per_pixel,1,  sizeof(short), file);
    fwrite(&header->compression, 1, sizeof(int), file);
    fwrite(&header->image_size, 1, sizeof(int), file);
    fwrite(&header->x_pixels_per_meter, 1, sizeof(int), file);
    fwrite(&header->y_pixels_per_meter, 1, sizeof(int), file);
    fwrite(&header->colors_in_color_table, 1, sizeof(int), file);
    fwrite(&header->important_color_count, 1, sizeof(int), file);
}

void makeBMPHeader(struct BMP_Header* header, int width, int height){
    char signature[2] = "BM";
    strcpy(header->signature, signature);
    int pad_size = (3*width) % 4;
    header->size = 54 + 3*width*height + pad_size*height;
    header->reserved1 = 0;
    header->reserved2 = 0;
    header->offset_pixel_array = 54;
}

void makeDIBHeader(struct DIB_Header* header, int width, int height){
    header->size = 40;
    header->width = width;
    header->height = height;
    header->planes = 1;
    header->bits_per_pixel = 24;
    header->compression = 0;
    header->image_size = 3*width*height;
    header->x_pixels_per_meter = 3780;
    header->y_pixels_per_meter = 3780;
    header->colors_in_color_table = 0;
    header->important_color_count = 0;
}

void readPixelsBMP(FILE* file, struct Pixel** pArr, int width, int height){
    int pad_size;
    if(width > 1){
        pad_size = (3*width) % 4;
    }else{
        pad_size = 1;
    }
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            fread(&pArr[height-i-1][j].blue, sizeof(unsigned char), 1, file);
            fread(&pArr[height-i-1][j].green, sizeof(unsigned char), 1, file);
            fread(&pArr[height-i-1][j].red, sizeof(unsigned char), 1, file);
        }
        fseek(file, pad_size, SEEK_CUR);
    }
}

void writePixelsBMP(FILE* file, struct Pixel** pArr, int width, int height){
    int pad_size;
    if(width > 1){
        pad_size = (3*width) % 4;
    }else{
        pad_size = 1;
    }
    short pad = 0;
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            fwrite(&pArr[height-i-1][j].blue, 1, sizeof(unsigned char), file);
            fwrite(&pArr[height-i-1][j].green, 1, sizeof(unsigned char), file);
            fwrite(&pArr[height-i-1][j].red, 1, sizeof(unsigned char), file);
        }
        fwrite(&pad, 1, pad_size, file);
    }
}