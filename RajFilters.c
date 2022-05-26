/**
* Program applies different types of filter to bmp images in parallel using threads.
*
* Completion time: 5 hours
*
* @author Muhammad Raj
* @version 1.0
*/

////////////////////////////////////////////////////////////////////////////////
//INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include "BmpProcessor.h"
#include "PixelProcessor.h"

//TODO: finish me

//UNCOMMENT BELOW LINE IF USING SER334 LIBRARY/OBJECT FOR BMP SUPPORT
//#include "BmpProcessor.h"


////////////////////////////////////////////////////////////////////////////////
//MACRO DEFINITIONS

//problem assumptions
#define BMP_HEADER_SIZE 14
#define BMP_DIB_HEADER_SIZE 40
#define MAXIMUM_IMAGE_SIZE 4096

//TODO: finish me


////////////////////////////////////////////////////////////////////////////////
//DATA STRUCTURES

//TODO: finish me
struct blur_thread_info {
    int height;
    int width;
    int start_col;
    int end_col;
    struct Pixel** original;
    struct Pixel** blurred;
};

struct swiss_thread_info {
    int x;
    int y;
    int height;
    int width;
    int radius;
    struct Pixel** pixels;
};


////////////////////////////////////////////////////////////////////////////////
//MAIN PROGRAM CODE

//TODO: finish me
struct Pixel* average(struct Pixel** original, int i, int j, int height, int width){
    int num_valid = 0;
    int sum_red = 0;
    int sum_green = 0;
    int sum_blue = 0;
    for(int x=i-1; x<i+2; x++){
        for(int y=j-1; y<j+2; y++){
            if((x>=0 && x<height) && (y>=0 && y<width)){
                num_valid++;
                sum_red += original[x][y].red;
                sum_green += original[x][y].green;
                sum_blue += original[x][y].blue;
            }
        }
    }
    struct Pixel* average = (struct Pixel*)malloc(sizeof(struct Pixel));
    average->red = sum_red/num_valid;
    average->green = sum_green/num_valid;
    average->blue = sum_blue/num_valid;
    
    return average;
}

void* blur(void* data){
    struct blur_thread_info* info = (struct blur_thread_info*)data;
    
    for(int i=0; i<info->height; i++){
        for(int j=info->start_col; j<info->end_col; j++){
            struct Pixel* pixel;
            pixel= average(info->original, i, j, info->height, info->width);
            info->blurred[i][j].red = pixel->red;
            info->blurred[i][j].green = pixel->green;
            info->blurred[i][j].blue = pixel->blue;
        }
    }
        
    pthread_exit(0);
}


void process_blur(FILE* input, FILE* output, int NUM_THREADS){
    struct BMP_Header* bmp_header = (struct BMP_Header*)malloc(sizeof(struct BMP_Header));   
    readBMPHeader(input, bmp_header);
    struct DIB_Header* dib_header = (struct DIB_Header*)malloc(sizeof(struct DIB_Header));
    readDIBHeader(input, dib_header);
    struct Pixel** pixels = (struct Pixel**)malloc(sizeof(struct Pixel*)*(dib_header->height));
    for(int i=0; i<dib_header->height; i++){
        pixels[i] = (struct Pixel*)malloc(sizeof(struct Pixel)*(dib_header->width));
    }
    readPixelsBMP(input, pixels, dib_header->width, dib_header->height);
    fclose(input);
    
    struct Pixel** blurred = (struct Pixel**)malloc(sizeof(struct Pixel*) * dib_header->height);
    for(int i=0; i<dib_header->height; i++){
        blurred[i] = (struct Pixel*)malloc(sizeof(struct Pixel) * dib_header->width);
    }
    
    pthread_t tids[NUM_THREADS];
    struct blur_thread_info infos[NUM_THREADS];
    for(int i=0; i<NUM_THREADS; i++){
        infos[i].height = dib_header->height;
        infos[i].width = dib_header->width;
        infos[i].start_col = i*(dib_header->width/NUM_THREADS);
        if((i==NUM_THREADS-1) && (dib_header->width % NUM_THREADS != 0)){
            infos[i].end_col = dib_header->width;
        }
        else{
            infos[i].end_col = (i+1)*(dib_header->width/NUM_THREADS);

        }
        infos[i].original = pixels;
        infos[i].blurred = blurred;
    }
    for(int i=0; i<NUM_THREADS; i++){
        pthread_create(&tids[i], NULL, blur, &infos[i]);
    }
    for(int i=0; i<NUM_THREADS; i++){
        pthread_join(tids[i], NULL);
    }
    
    writeBMPHeader(output, bmp_header);
    writeDIBHeader(output, dib_header);
    writePixelsBMP(output, blurred, dib_header->width, dib_header->height);
    fclose(output);
    printf("Output image successfully created.\n");
}

void* swiss(void* data) {
    struct swiss_thread_info* info = (struct swiss_thread_info*)data;
    
    for(int i=0; i<info->height; i++){
        for(int j=0; j<info->width; j++){
            int dx = i-info->x;
            int dy = j-info->y;
            int dist = sqrt(dx*dx + dy*dy);
            if(dist <= info->radius){
                info->pixels[i][j].red = 0;
                info->pixels[i][j].green = 0;
                info->pixels[i][j].blue = 0;
            }
        }
    }
}

void process_swiss(FILE* input, FILE* output) {
    struct BMP_Header* bmp_header = (struct BMP_Header*)malloc(sizeof(struct BMP_Header));   
    readBMPHeader(input, bmp_header);
    struct DIB_Header* dib_header = (struct DIB_Header*)malloc(sizeof(struct DIB_Header));
    readDIBHeader(input, dib_header);
    struct Pixel** pixels = (struct Pixel**)malloc(sizeof(struct Pixel*)*(dib_header->height));
    for(int i=0; i<dib_header->height; i++){
        pixels[i] = (struct Pixel*)malloc(sizeof(struct Pixel)*(dib_header->width));
    }
    readPixelsBMP(input, pixels, dib_header->width, dib_header->height);
    fclose(input);
    
    colorShiftPixels(pixels, dib_header->width, dib_header->height, 30, 30, -30);
    
    int min = dib_header->width;
    if(dib_header->height < min){
        min = dib_header->height;
    }
    time_t t;
    srand((unsigned) time(&t));
    int max_radius = 3*min/20;
    int min_radius = min/20;
    int NUM_THREADS = min/10;
    pthread_t tids[NUM_THREADS];
    struct swiss_thread_info infos[NUM_THREADS];
    for(int i=0; i<NUM_THREADS; i++){
        infos[i].height = dib_header->height;
        infos[i].width = dib_header->width;
        infos[i].radius = (rand() % (max_radius - min_radius + 1)) + min_radius;
        infos[i].x = (rand() % (dib_header->width + 1));
        infos[i].y = (rand() % (dib_header->height + 1));
        infos[i].pixels = pixels;
    }
    for(int i=0; i<NUM_THREADS; i++){
        pthread_create(&tids[i], NULL, swiss, &infos[i]);
    }
    for(int i=0; i<NUM_THREADS; i++){
        pthread_join(tids[i], NULL);
    }
    
    writeBMPHeader(output, bmp_header);
    writeDIBHeader(output, dib_header);
    writePixelsBMP(output, pixels, dib_header->width, dib_header->height);
    fclose(output);
    printf("Image file output.bmp successfully created.\n");
}

void main(int argc, char* argv[]) {
    char* input_name;
    char* output_name;
    char* filter_type;
    int opt;
    output_name = "output.bmp";
    
    while ((opt = getopt (argc, argv, "i:o:f:")) != -1){
        switch (opt){
          case 'i':
            input_name = optarg;
            break;
          case 'o':
            output_name = optarg;
            break;
          case 'f':
            filter_type = optarg;
            break;
          case '?':
            if (optopt == 'i' || optopt == 'o' || optopt == 'f')
              fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
              fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
              fprintf (stderr,
                       "Unknown option character `\\x%x'.\n",
                       optopt);
            return 1;
          default:
            abort ();
        }
    }
    
    FILE* input = fopen(input_name, "rb");
    FILE* output = fopen(output_name, "wb");
    
    if(strcmp(filter_type, "b") == 0){
        printf("Please input the desired number of threads.\n");
        int NUM_THREADS;
        scanf("%d", &NUM_THREADS);
        process_blur(input, output, NUM_THREADS);
    }
    else if(strcmp(filter_type, "c") == 0){
        process_swiss(input, output);
    }
    else{
        printf("Please ensure filter type is 'b' for blur or 'c' for swiss cheese.\n");
        exit(1);
    }
}