#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "imcore.h"
#include "cvcore.h"
#include "iocore.h"
#include "alcore.h"

int main(int argc, char *argv[]) 
{
    int i;

    matrix_t *test;

    // load the test sample
    printf("Reading the image...\n");
    if(argc > 1)
    {
        test = imread(argv[1]);
    }
    else
    {
        test = imread("..//data//faces//oscar.bmp");
    }

    // load the object model
    printf("Loading the object model...\n");
    struct haar_t *model = haar_read("../data/cascades/haarcascade_frontalface_alt.json");

    // convert it to grayscale
    printf("Converting image into grayscale...\n");
    matrix_t *gray = matrix_create(uint8_t, rows(test), cols(test), 1);
    rgb2gray(test, gray);

    // do the detection
    printf("Detecting objects...\n");
    vector_t *detections = haar_detector(gray, model, 20, 500, 1.15, 1.5f);

    detections = rectangle_merge(detections, 0.3, 1);

    // draw the detections
    printf("Drawing the %d detected objects...\n", length(detections));

    struct rectangle_t *obj = vdata(detections, 0);
    for(i = 0; i < length(detections); i++)
    {
        printf("obj[%03d]: %d %d %d %d\n", i, obj[i].x, obj[i].y, obj[i].width, obj[i].height);

        // set the thickness of the detected object 
        uint32_t thickness = max(1, sqrt(obj[i].width * obj[i].height) / 20);

        draw_rectangle(test, obj[i], RGB(50, 140, 100), 2 * thickness + 1);
        draw_rectangle(test, obj[i], RGB(80, 250, 255), thickness);
    }

    // write the marked objects as image
    printf("Writing the output image ...\n");
    imwrite(test, "detected_objects.bmp");

    // clear the allocations
    matrix_free(&test);
    matrix_free(&gray);
    vector_free(&detections);

    return 0;
}