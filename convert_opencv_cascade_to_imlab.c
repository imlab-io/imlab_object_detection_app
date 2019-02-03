#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "imcore.h"
#include "iocore.h"

int main(int argc, char *argv[])
{
    char *filename = "../data/cascades/haarcascade_eye.json";

    // extension must be JSON
    vector_t *root = json_read(filename);

    // allocate output json object
    vector_t *output = vector_create(struct json_object_t);

    // tmp variables
    uint32_t i, j, k, l;
    struct json_data_t tmp_data, tmp_obj;

    // get width and height, and push it to the output
    int s1 = 0, s2 = 0;
    char *size_info = json_get_string(root, "size", 0);
    sscanf(size_info, "%d %d", &s1, &s2);

    printf("size: %d x %d\n", s1, s2);

    json_number(s1, &tmp_data);
    json_push(output, tmp_data, "feature_width");
    json_number(s2, &tmp_data);
    json_push(output, tmp_data, "feature_height");

    // read stages and data
    vector_t *stages_obj = json_get_object(root, "stages", 0);
    vector_t *stages_arr = json_get_array(stages_obj, "_", 0);

    vector_t *stages_output = vector_create(struct json_data_t);
    
    // each haar classifier consists of different number of stages
    for (i = 0; i < length(stages_arr); i++)
    {
        // each stage has a stage threshold and a tree array
        vector_t *stage_i = json_get_object(stages_arr, i);
        vector_t *trees_obj = json_get_object(stage_i, "trees", 0);
        vector_t *trees_arr = json_get_array(trees_obj, "_", 0);

        vector_t *trees_object = vector_create(struct json_object_t);

        // clean array and push
        vector_t *trees_output = vector_create(struct json_data_t);

        for (j = 0; j < length(trees_arr); j++)
        {
            vector_t *trees_object_i = vector_create(struct json_object_t);

            vector_t *tree_i = json_get_object(trees_arr, j);
            vector_t *trees_i_arr = json_get_array(tree_i, "_", 0);

            /*
            "feature": {
                  "rects": {
                    "_": [
                      "2 0 17 18 -1.",
                      "2 6 17 6 3."
                    ]
                  },
                  "tilted": "0"
                },
            */

            vector_t *feature_object = vector_create(struct json_object_t);
            vector_t *feature_obj = json_get_object(trees_i_arr, "feature", 0);

            vector_t *rect_obj = json_get_object(feature_obj, "rects", 0);
            vector_t *rect_arr = json_get_array(rect_obj, "_", 0);
            
            // get and push rectangles
            struct json_data_t rect_data;
            json_array(rect_arr, &rect_data);
            json_push(feature_object, rect_data, "rects");
            
            // get and push tilted
            char *tilted_chr = json_get_string(feature_obj, "tilted", 0);
            json_number(strtod(tilted_chr, NULL), &tmp_data);
            json_push(feature_object, tmp_data, "tilted");

            // push the object into the stage
            struct json_data_t feature_data;
            json_object(feature_object, &feature_data);

            json_push(trees_object_i, feature_data, "feature");

            // get and push left,right values and threshold
            char *threshold_chr = json_get_string(trees_i_arr, "threshold", 0);
            json_number(strtod(threshold_chr, NULL), &tmp_data);
            json_push(trees_object_i, tmp_data, "threshold");

            char *left_val_chr = json_get_string(trees_i_arr, "left_val", 0);
            json_number(strtod(left_val_chr, NULL), &tmp_data);
            json_push(trees_object_i, tmp_data, "left_val");

            char *right_val_chr = json_get_string(trees_i_arr, "right_val", 0);
            json_number(strtod(right_val_chr, NULL), &tmp_data);
            json_push(trees_object_i, tmp_data, "right_val");

            struct json_data_t trees_array;
            json_object(trees_object_i, &trees_array);

            // push all the trees
            json_push(trees_output, trees_array);
        }

        struct json_data_t trees_array;
        json_array(trees_output, &trees_array);

        // push all the trees
        json_push(trees_object, trees_array, "trees");

        // get stage threshold, parent and next fields from the source and copy it into the output
        char *stage_thr = json_get_string(stage_i, "stage_threshold", 0);
        json_number(strtod(stage_thr, NULL), &tmp_data);
        json_push(trees_object, tmp_data, "stage_threshold");

        char *parent_chr = json_get_string(stage_i, "parent", 0);
        json_number(strtod(parent_chr, NULL), &tmp_data);
        json_push(trees_object, tmp_data, "parent");

        char *next_chr = json_get_string(stage_i, "next", 0);
        json_number(strtod(next_chr, NULL), &tmp_data);
        json_push(trees_object, tmp_data, "next");

        // push the object into the stage
        struct json_data_t trees_object_data;
        json_object(trees_object, &trees_object_data);

        json_push(stages_output, trees_object_data);

    }             // end of stages
    
    // push stages as array
    struct json_data_t stages_array;
    json_array(stages_output, &stages_array);
    json_push(output, stages_array, "stages");

    json_write(filename, output, 0);
}
