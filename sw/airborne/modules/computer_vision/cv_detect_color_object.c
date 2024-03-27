/*
 * Copyright (C) 2019 Kirk Scheper <kirkscheper@gmail.com>
 *
 * This file is part of Paparazzi.
 *
 * Paparazzi is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * Paparazzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Paparazzi; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * @file modules/computer_vision/cv_detect_object.h
 * Assumes the object consists of a continuous and checks
 * if you are over the defined object or not
 */

// Own header
#include "modules/computer_vision/cv_detect_color_object.h"
#include "modules/computer_vision/cv.h"
#include "modules/core/abi.h"
#include "std.h"

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "pthread.h"

#define PRINT(string,...) fprintf(stderr, "[object_detector->%s()] " string,__FUNCTION__ , ##__VA_ARGS__)
#if OBJECT_DETECTOR_VERBOSE
  #define VERBOSE_PRINT PRINT
#else
  #define VERBOSE_PRINT(...)
#endif

static pthread_mutex_t mutex;

#ifndef COLOR_OBJECT_DETECTOR_FPS
  #define COLOR_OBJECT_DETECTOR_FPS 0 ///< Default FPS (zero means run at camera fps)
#endif

// Filter Setting initialization
uint8_t cod_lum_min[] = {0, 0, 0, 0};
uint8_t cod_lum_max[] = {0, 0, 0, 0};
uint8_t cod_cb_min[] = {0, 0, 0, 0};
uint8_t cod_cb_max[] = {0, 0, 0, 0};
uint8_t cod_cr_min[] = {0, 0, 0, 0};
uint8_t cod_cr_max[] = {0, 0, 0, 0};

bool cod_draw[] = {false, false, false, false};

// define global variables
struct color_object_t {
  int32_t x_c;
  int32_t y_c;
  uint32_t color_count;
  bool updated;
};
struct color_object_t global_filters[1];

// Function
uint32_t find_object_centroid(struct image_t *img, int32_t* p_xc, int32_t* p_yc, bool draw[],
                              uint8_t lum_min[], uint8_t lum_max[],
                              uint8_t cb_min[], uint8_t cb_max[],
                              uint8_t cr_min[], uint8_t cr_max[]);

/*
 * object_detector
 * @param img - input image to process
 * @param filter - which detection filter to process
 * @return img
 */

uint8_t numFilters = 4;

static struct image_t *object_detector(struct image_t *img)
{
  uint8_t lum_min[numFilters], lum_max[numFilters];
  uint8_t cb_min[numFilters], cb_max[numFilters];
  uint8_t cr_min[numFilters], cr_max[numFilters];
  bool draw[numFilters];
  
  int32_t x_c, y_c;
  
  // TODO -- reimplement green scary

  // Filter and find centroid
  uint32_t count = find_object_centroid(img, &x_c, &y_c, draw, lum_min, lum_max, cb_min, cb_max, cr_min, cr_max);
  VERBOSE_PRINT("Color count %d: %u, threshold %u, x_c %d, y_c %d\n", camera, object_count, count_threshold, x_c, y_c);
  VERBOSE_PRINT("centroid %d: (%d, %d) r: %4.2f a: %4.2f\n", camera, x_c, y_c,
        hypotf(x_c, y_c) / hypotf(img->w * 0.5, img->h * 0.5), RadOfDeg(atan2f(y_c, x_c)));
  pthread_mutex_lock(&mutex);
  global_filters[0].color_count = count;
  global_filters[0].x_c = x_c;
  global_filters[0].y_c = y_c;
  global_filters[0].updated = true;
  pthread_mutex_unlock(&mutex);

  return img;
}

struct image_t *object_detector1(struct image_t *img, uint8_t camera_id);
struct image_t *object_detector1(struct image_t *img, uint8_t camera_id __attribute__((unused)))
{
  return object_detector(img);
}

void color_object_detector_init(void)
{
  memset(global_filters, 0, sizeof(struct color_object_t));
  pthread_mutex_init(&mutex, NULL);

  #ifdef COLOR_OBJECT_DETECTOR_CAMERA
    #ifdef COLOR_OBJECT_DETECTOR_LUM_MIN
      cod_lum_min[0] = COLOR_OBJECT_DETECTOR_LUM_MIN1;
      cod_lum_min[1] = COLOR_OBJECT_DETECTOR_LUM_MIN2;
      cod_lum_min[2] = COLOR_OBJECT_DETECTOR_LUM_MIN3;
      cod_lum_min[3] = COLOR_OBJECT_DETECTOR_LUM_MIN4;

      cod_lum_max[0] = COLOR_OBJECT_DETECTOR_LUM_MAX1;
      cod_lum_max[1] = COLOR_OBJECT_DETECTOR_LUM_MAX2;
      cod_lum_max[2] = COLOR_OBJECT_DETECTOR_LUM_MAX3;
      cod_lum_max[3] = COLOR_OBJECT_DETECTOR_LUM_MAX4;

      cod_cb_min[0] = COLOR_OBJECT_DETECTOR_CB_MIN1;
      cod_cb_min[1] = COLOR_OBJECT_DETECTOR_CB_MIN2;
      cod_cb_min[2] = COLOR_OBJECT_DETECTOR_CB_MIN3;
      cod_cb_min[3] = COLOR_OBJECT_DETECTOR_CB_MIN4;
      
      cod_cb_max[0] = COLOR_OBJECT_DETECTOR_CB_MAX1;
      cod_cb_max[1] = COLOR_OBJECT_DETECTOR_CB_MAX2;
      cod_cb_max[2] = COLOR_OBJECT_DETECTOR_CB_MAX3;
      cod_cb_max[3] = COLOR_OBJECT_DETECTOR_CB_MAX4;

      
      cod_cr_min[0] = COLOR_OBJECT_DETECTOR_CR_MIN1;
      cod_cr_min[1] = COLOR_OBJECT_DETECTOR_CR_MIN2;
      cod_cr_min[2] = COLOR_OBJECT_DETECTOR_CR_MIN3;
      cod_cr_min[3] = COLOR_OBJECT_DETECTOR_CR_MIN4;

      cod_cr_max[0] = COLOR_OBJECT_DETECTOR_CR_MAX1;
      cod_cr_max[1] = COLOR_OBJECT_DETECTOR_CR_MAX2;
      cod_cr_max[2] = COLOR_OBJECT_DETECTOR_CR_MAX3;
      cod_cr_max[3] = COLOR_OBJECT_DETECTOR_CR_MAX4;
    #endif
    #ifdef COLOR_OBJECT_DETECTOR_DRAW
      draw[0] = COLOR_OBJECT_DETECTOR_DRAW1;
      draw[1] = COLOR_OBJECT_DETECTOR_DRAW2;
      draw[2] = COLOR_OBJECT_DETECTOR_DRAW3;
      draw[3] = COLOR_OBJECT_DETECTOR_DRAW4;
    #endif
    cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA, object_detector1, COLOR_OBJECT_DETECTOR_FPS, 0);
  #endif
}

/*
 * find_object_centroid
 *
 * Finds the centroid of pixels in an image within filter bounds.
 * Also returns the amount of pixels that satisfy these filter bounds.
 *
 * @param img - input image to process formatted as YUV422.
 * @param p_xc - x coordinate of the centroid of color object
 * @param p_yc - y coordinate of the centroid of color object
 * @param lum_min - minimum y value for the filter in YCbCr colorspace
 * @param lum_max - maximum y value for the filter in YCbCr colorspace
 * @param cb_min - minimum cb value for the filter in YCbCr colorspace
 * @param cb_max - maximum cb value for the filter in YCbCr colorspace
 * @param cr_min - minimum cr value for the filter in YCbCr colorspace
 * @param cr_max - maximum cr value for the filter in YCbCr colorspace
 * @param draw - whether or not to draw on image
 * @return number of pixels of image within the filter bounds.
 */
uint32_t find_object_centroid(struct image_t *img, int32_t* p_xc, int32_t* p_yc, bool draw[],
                              uint8_t lum_min[], uint8_t lum_max[],
                              uint8_t cb_min[], uint8_t cb_max[],
                              uint8_t cr_min[], uint8_t cr_max[])
{
  uint32_t cnt = 0;
  uint32_t cunt = 0;
  uint32_t tot_x = 0;
  uint32_t tot_y = 0;
  uint8_t *buffer = img->buf;


  /** NR - The below section loops through all pixels, 
   * and checks if the pixel is within given colour range.
   * Then returns the total count of pixels that fall within the range.
   * I've changed it to only multiply once rather than like several thousand times...
   */
  


  
  int x = 0;
  int y = 0;
  
  int tol = 30;
  
  // Start pixel count
  uint32_t p = 0;
  //int tol = 30;
  uint32_t area = img->h * img->w;
  
  // Go through all the pixels
  while (p < area) {
    if (x >= ((img->w / 2) - tol) && x <= ((img->w/2) + tol)){
        //printf("x: %d", x);
        // Check if the color is inside the specified values
        uint8_t *yp, *up, *vp;
        // NR: Honestly, I don't know why it multiplys by 2 here but it keeps it consistent with the OG code.
        uint32_t p2 = p+p; 
        if (p % 2 == 0) {
          // Even x
          up = &buffer[p2];      // U
          yp = &buffer[p2 + 1];  // Y1
          vp = &buffer[p2 + 2];  // V
          //yp = &buffer[p2 + 3]; // Y2
        } else {
          // Uneven x
          up = &buffer[p2 - 2];  // U
          //yp = &buffer[p2 - 1]; // Y1
          vp = &buffer[p2];      // V
          yp = &buffer[p2 + 1];  // Y2
        }
        if ( // holy mother of if statements
            ( // for efficiency
              (*yp >= lum_min[0] ) && (*yp <= lum_max[0] ) &&
              (*up >= cb_min[0] ) && (*up <= cb_max[0] ) &&
              (*vp >= cr_min[0] ) && (*vp <= cr_max[0] )
            ) || (
              (*yp >= lum_min[1] ) && (*yp <= lum_max[1] ) &&
              (*up >= cb_min[1] ) && (*up <= cb_max[1] ) &&
              (*vp >= cr_min[1] ) && (*vp <= cr_max[1] )
            ) || (
              (*yp >= lum_min[2] ) && (*yp <= lum_max[2] ) &&
              (*up >= cb_min[2] ) && (*up <= cb_max[2] ) &&
              (*vp >= cr_min[2] ) && (*vp <= cr_max[2] )
            ) || (
              (*yp >= lum_min[3] ) && (*yp <= lum_max[3] ) &&
              (*up >= cb_min[3] ) && (*up <= cb_max[3] ) &&
              (*vp >= cr_min[3] ) && (*vp <= cr_max[3] )
            )
           ) {
                  
          // NR: Make more the weight of pixels within central range.
          if (y >= 205 && y <= 315){   /// 210 & 310
            cnt += 4;
          } else if (y >= 70 && y <= 450) {
            cnt += 2;
          } else {
            cnt ++;
          }
          
          cunt++;

          tot_x += x;
          tot_y += y;
          if (draw){
            *yp = 255;  // make pixel brighter in image
          }
        }
      }
    // NR: Increment pixel
    p ++;
    x ++;
    if (x >= img->w){
      x = 0;
      y ++;
    }
  }
  // Centroid calculations
  if (cnt > 0) {
    *p_xc = (int32_t)roundf(tot_x / ((float) cunt) - img->w * 0.5f);
    *p_yc = (int32_t)roundf(img->h * 0.5f - tot_y / ((float) cunt));
  } else {
    *p_xc = 0;
    *p_yc = 0;
  }

  return cnt;
}


void color_object_detector_periodic(void)
{
  static struct color_object_t local_filters[1];
  pthread_mutex_lock(&mutex);
  memcpy(local_filters, global_filters, sizeof(struct color_object_t)); // replace local_filters with global_filters
  pthread_mutex_unlock(&mutex);

  if(local_filters[0].updated){
    AbiSendMsgVISUAL_DETECTION(COLOR_OBJECT_DETECTION1_ID, local_filters[0].x_c, local_filters[0].y_c,
        0, 0, local_filters[0].color_count, 0);
    local_filters[0].updated = false;
  }
}
