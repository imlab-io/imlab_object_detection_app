#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "imcore.h"
#include "cvcore.h"
#include "iocore.h"
#include "alcore.h"

struct haar_feature_t
{
    int tilt;

    // rectangles under a feature
    uint32_t length;
    struct rectangle_t *rects;
};

struct haar_tree_t
{
    double threshold;
    double l_val;
    double r_val;

    struct haar_feature_t *feats;
};

struct haar_stage_t
{
    double stage_threshold;
    int parent;
    int next;

    int length;
    struct haar_tree_t **trees;
};

typedef struct haar_t
{
    int size1;
    int size2;

    int length;
    struct haar_stage_t **stages;
} haar_t;

matrix_t* draw_haar_feature(matrix_t *in, int i, int j, float scale, struct rectangle_t *rects, int numRects)
{
    // copy matrix img
    matrix_t *img = matrix_create(uint8_t, rows(in), cols(in), channels(in), mdata(in,0), 0);
    
    int x, y, c, r = 0;
    for (r = 0; r < numRects; r++)
    {
        int x1 = round(i + scale * rects[r].x);
        int y1 = round(j + scale * rects[r].y);

        int w = round(scale * rects[r].width);
        int h = round(scale * rects[r].height);

        uint8_t color = rects[r].coefficient > 0 ? 255:0;

        // fill rectangle
        for (y = 0; y < h; y++)
        {
            for(x = 0; x < w; x++)
            {
                for(c = 0; c < channels(img); c++)
                {
                    atui8(img, y + y1, x + x1, c) = clamp(0.3*atui8(img, y + y1, x + x1, c)  + 0.7*color, 0, 255);
                }
            }
        }
    }

    return img;
}


vector_t *haar_detector_visual(matrix_t *img, struct haar_t *cascade, float scale, float scaleFactor, float stepSize)
{
    int modelWindowWidth = cascade->size1;
    int modelWindowHeight = cascade->size2;

    // output
    vector_t *detections = vector_create(struct rectangle_t);

    // necessary variables
    int i, j, s, t, f, r, h;
    float sum_stage, sum_feature;

    matrix_t *sums = matrix_create(float);
    matrix_t *ssum = matrix_create(float);

    // create display image
    matrix_t *disp = matrix_create(uint8_t, rows(img), cols(img), channels(img), mdata(img, 0), 0);

    // convert it to grayscale
    matrix_t *gray = matrix_create(uint8_t, rows(img), cols(img), 1);
    rgb2gray(img, gray);

    // compute the integral image
    integral(gray, sums, ssum);

    // start haar classifier for different scales
    // WS
    int currentWindowWidth = modelWindowWidth * scale;
    int currentWindowHeight = modelWindowHeight * scale;

    if (!(currentWindowWidth < cols(img) && currentWindowHeight < rows(img)))
    {
        return detections;
    }

    float InvArea = 1.0f / (currentWindowWidth * currentWindowHeight);
    int stride = round(scale * stepSize);

    for (j = stride; j < rows(img) - currentWindowHeight; j += maximum(1, stride))
    {
        for (i = stride; i < cols(img) - currentWindowWidth; i += maximum(1, stride))
        {
            // get variance and mean

            float e2 = InvArea * integral_get_float(ssum, i, j, i + currentWindowWidth - 1, j + currentWindowHeight - 1, 0);
            float e1 = InvArea * integral_get_float(sums, i, j, i + currentWindowWidth - 1, j + currentWindowHeight - 1, 0);

            float variance = (e2 - e1 * e1);

            // find the variance factor
            float stdev = sqrt(variance > 0 ? variance : 1);

            // check for each stage
            for (s = 0; s < cascade->length; s++)
            {
                uint8_t hue = map(s, 0, cascade->length-1, 0, 100);
                struct color_t faceColor = HSV(hue, 200, 255);

                double sum_stage = 0;

                // each stage has a stage threshold and a tree array
                struct haar_stage_t *stage_s = cascade->stages[s];

                // check a stage pass the threshold
                for (t = 0; t < stage_s->length; t++)
                {
                    // each tree has number of paramaters for the algorithm
                    struct haar_tree_t *trees_t = stage_s->trees[t];

                    // draw haar feature on image
                    matrix_t *out = draw_haar_feature(img, i, j, scale, trees_t->feats->rects, trees_t->feats->length);

                    // draw stage
                    draw_line(out, point(0, 0, 0), point(map(s+1, 0, cascade->length, 0, cols(out) - 1), 0, 0), faceColor, 4);

                    // change color from red to green according to the cascade number
                    struct rectangle_t obj = rectangle(i, j, currentWindowWidth, currentWindowHeight, 1.0);
                    draw_rectangle(out, obj, faceColor, 3);

                    // examine each feature
                    float sum_feature = 0;
                    for (r = 0; r < trees_t->feats->length; r++)
                    {
                        int x1 = round(i + scale * trees_t->feats->rects[r].x);
                        int y1 = round(j + scale * trees_t->feats->rects[r].y);

                        int w = round(scale * trees_t->feats->rects[r].width);
                        int h = round(scale * trees_t->feats->rects[r].height);

                        sum_feature += integral_get_float(sums, x1, y1, x1 + w - 1, y1 + h - 1, 0) * trees_t->feats->rects[r].coefficient;
                    }

                    // construct filename and save the file
                    char filename[256];
                    sprintf(filename, "frames//frame_%04d_%02d_%03d.bmp", j * cols(img) + i, s, t);
                    //imwrite(out, filename);

                    matrix_free(&out);

                    // choose left or right node depending on the feature summation
                    sum_stage += (sum_feature * InvArea < stdev * trees_t->threshold) ? trees_t->l_val : trees_t->r_val;
                }

                // check that the sum is larger than the stage threshold
                if (sum_stage < stage_s->stage_threshold)
                {
                    break;
                }
            }
            // push the detected object into the output buffer
            if (s == cascade->length)
            {
                struct rectangle_t found = rectangle(i, j, currentWindowWidth, currentWindowHeight, 1.0);
                vector_push(detections, &found);

                printf("obj[%03d]: %d %d %d %d\n", length(detections), found.x, found.y, found.width, found.height);

                struct rectangle_t *obj = vdata(detections, 0);
                for (h = 0; h < length(detections); h++)
                {
                    draw_rectangle(img, obj[h], RGB(80, 250, 255), 2);
                }
            }
        }
    }

    // clear the allocations
    matrix_free(&sums);
    matrix_free(&ssum);
    matrix_free(&gray);

    // return the detections
    return detections;
}

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

    // do the detection
    printf("Detecting objects...\n");
    vector_t *detections = haar_detector_visual(test, model, 7, 1.15, 2.0f);

    // detections = rectangle_merge(detections, 0.3, 1);

    // draw the detections
    printf("Drawing the %d detected objects...\n", length(detections));

    struct rectangle_t *obj = vdata(detections, 0);
    for(i = 0; i < length(detections); i++)
    {
        printf("obj[%03d]: %d %d %d %d\n", i, obj[i].x, obj[i].y, obj[i].width, obj[i].height);

        // set the thickness of the detected object 
        uint32_t thickness = maximum(1, sqrt(obj[i].width * obj[i].height) / 20);
        
        draw_rectangle(test, obj[i], RGB(50, 140, 100), 2 * thickness + 1);
        draw_rectangle(test, obj[i], RGB(80, 250, 255), thickness);
        
    }

    // write the marked objects as image
    printf("Writing the output image ...\n");
    imwrite(test, "detected_objects.bmp");

    // clear the allocations
    matrix_free(&test);
    vector_free(&detections);

    return 0;
}