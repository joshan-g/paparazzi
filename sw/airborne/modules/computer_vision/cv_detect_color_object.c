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

#ifndef COLOR_OBJECT_DETECTOR_FPS1
#define COLOR_OBJECT_DETECTOR_FPS1 0 ///< Default FPS (zero means run at camera fps)
#endif
#ifndef COLOR_OBJECT_DETECTOR_FPS2
#define COLOR_OBJECT_DETECTOR_FPS2 0 ///< Default FPS (zero means run at camera fps)
#endif
#ifndef COLOR_OBJECT_DETECTOR_FPS3
#define COLOR_OBJECT_DETECTOR_FPS3 0 ///< Default FPS (zero means run at camera fps)
#endif
#ifndef COLOR_OBJECT_DETECTOR_FPS4
#define COLOR_OBJECT_DETECTOR_FPS4 0 ///< Default FPS (zero means run at camera fps)
#endif
#ifndef COLOR_OBJECT_DETECTOR_FPS5
#define COLOR_OBJECT_DETECTOR_FPS5 0 ///< Default FPS (zero means run at camera fps)
#endif

// Filter Settings
uint8_t cod_lum_min1 = 0;
uint8_t cod_lum_max1 = 0;
uint8_t cod_cb_min1 = 0;
uint8_t cod_cb_max1 = 0;
uint8_t cod_cr_min1 = 0;
uint8_t cod_cr_max1 = 0;

uint8_t cod_lum_min2 = 0;
uint8_t cod_lum_max2 = 0;
uint8_t cod_cb_min2 = 0;
uint8_t cod_cb_max2 = 0;
uint8_t cod_cr_min2 = 0;
uint8_t cod_cr_max2 = 0;

uint8_t cod_lum_min3 = 0;
uint8_t cod_lum_max3 = 0;
uint8_t cod_cb_min3 = 0;
uint8_t cod_cb_max3 = 0;
uint8_t cod_cr_min3 = 0;
uint8_t cod_cr_max3 = 0;

uint8_t cod_lum_min4 = 0;
uint8_t cod_lum_max4 = 0;
uint8_t cod_cb_min4 = 0;
uint8_t cod_cb_max4 = 0;
uint8_t cod_cr_min4 = 0;
uint8_t cod_cr_max4 = 0;

uint8_t cod_lum_min5 = 0;
uint8_t cod_lum_max5 = 0;
uint8_t cod_cb_min5 = 0;
uint8_t cod_cb_max5 = 0;
uint8_t cod_cr_min5 = 0;
uint8_t cod_cr_max5 = 0;

bool cod_draw1 = false;
bool cod_draw2 = false;
bool cod_draw3 = false;
bool cod_draw4 = false;
bool cod_draw5 = false;

// define global variables
struct color_object_t {
  int32_t x_c;
  int32_t y_c;
  uint32_t color_count;
  bool updated;
};
struct color_object_t global_filters[5];

// Function
uint32_t find_object_centroid(struct image_t *img, int32_t* p_xc, int32_t* p_yc, bool draw,
                              uint8_t lum_min, uint8_t lum_max,
                              uint8_t cb_min, uint8_t cb_max,
                              uint8_t cr_min, uint8_t cr_max, int colour);

/*
 * object_detector
 * @param img - input image to process
 * @param filter - which detection filter to process
 * @return img
 */
static struct image_t *object_detector(struct image_t *img, uint8_t filter)
{
  uint8_t lum_min, lum_max;
  uint8_t cb_min, cb_max;
  uint8_t cr_min, cr_max;
  bool draw;

  uint32_t count = 0;
  uint32_t count_i = 0;
  
  //int32_t y_c_sum = 0;
  int32_t x_c, y_c;

  for (int i = 5; i >= 1; i--){
    if (i == 1){
      lum_min = cod_lum_min1; // Orange
      lum_max = cod_lum_max1;
      cb_min = cod_cb_min1;
      cb_max = cod_cb_max1;
      cr_min = cod_cr_min1;
      cr_max = cod_cr_max1;
      draw = cod_draw1;
    } else if (i == 2){ // Bright Black (brighter than the wall)
      lum_min = cod_lum_min2;
      lum_max = cod_lum_max2;
      cb_min = cod_cb_min2;
      cb_max = cod_cb_max2;
      cr_min = cod_cr_min2;
      cr_max = cod_cr_max2;
      draw = cod_draw2;
    } else if (i == 3){ //Green
      lum_min = cod_lum_min3;
      lum_max = cod_lum_max3;
      cb_min = cod_cb_min3;
      cb_max = cod_cb_max3;
      cr_min = cod_cr_min3;
      cr_max = cod_cr_max3;
      draw = cod_draw3;
    } else if (i == 4){ //Dark black (darker than the wall)
      lum_min = cod_lum_min4;
      lum_max = cod_lum_max4;
      cb_min = cod_cb_min4;
      cb_max = cod_cb_max4;
      cr_min = cod_cr_min4;
      cr_max = cod_cr_max4;
      draw = cod_draw4;
    } else if (i == 5){
      lum_min = cod_lum_min5;
      lum_max = cod_lum_max5;
      cb_min = cod_cb_min5;
      cb_max = cod_cb_max5;
      cr_min = cod_cr_min5;
      cr_max = cod_cr_max5;
      draw = cod_draw5;
    }
    count_i = find_object_centroid(img, &x_c, &y_c, draw, lum_min, lum_max, cb_min, cb_max, cr_min, cr_max, i);
    count += count_i;
    
    //If green, triple the count, because green is scary. E.g. trees have less density.
    if (i == 3){
      count += (count_i + count_i); // NR: I think it's better to use addition here because multiplication badness?
    }

    //y_c_sum += y_c;
  }
  //y_c = y_c_sum / 3;
//The above for loop sums the count of all 3. 
  /*
  switch (filter){
    case 1:
      lum_min = cod_lum_min1;
      lum_max = cod_lum_max1;
      cb_min = cod_cb_min1;
      cb_max = cod_cb_max1;
      cr_min = cod_cr_min1;
      cr_max = cod_cr_max1;
      draw = cod_draw1;
      break;
    case 2:
      lum_min = cod_lum_min2;
      lum_max = cod_lum_max2;
      cb_min = cod_cb_min2;
      cb_max = cod_cb_max2;
      cr_min = cod_cr_min2;
      cr_max = cod_cr_max2;
      draw = cod_draw2;
      break;
    case 3:
      lum_min = cod_lum_min3;
      lum_max = cod_lum_max3;
      cb_min = cod_cb_min3;
      cb_max = cod_cb_max3;
      cr_min = cod_cr_min3;
      cr_max = cod_cr_max3;
      draw = cod_draw3;
      break;
    case 4:
      lum_min = cod_lum_min4;
      lum_max = cod_lum_max4;
      cb_min = cod_cb_min4;
      cb_max = cod_cb_max4;
      cr_min = cod_cr_min4;
      cr_max = cod_cr_max4;
      draw = cod_draw4;
      break;
    case 4:
      lum_min = cod_lum_min5;
      lum_max = cod_lum_max5;
      cb_min = cod_cb_min5;
      cb_max = cod_cb_max5;
      cr_min = cod_cr_min5;
      cr_max = cod_cr_max5;
      draw = cod_draw5;
      break;
    default:
      return img;
  };*/

  //int32_t x_c, y_c;

  // Filter and find centroid
  //uint32_t count = find_object_centroid(img, &x_c, &y_c, draw, lum_min, lum_max, cb_min, cb_max, cr_min, cr_max);
  VERBOSE_PRINT("Color count %d: %u, threshold %u, x_c %d, y_c %d\n", camera, object_count, count_threshold, x_c, y_c);
  VERBOSE_PRINT("centroid %d: (%d, %d) r: %4.2f a: %4.2f\n", camera, x_c, y_c,
        hypotf(x_c, y_c) / hypotf(img->w * 0.5, img->h * 0.5), RadOfDeg(atan2f(y_c, x_c)));
  pthread_mutex_lock(&mutex);
  global_filters[filter-1].color_count = count;
  global_filters[filter-1].x_c = x_c;
  global_filters[filter-1].y_c = y_c;
  global_filters[filter-1].updated = true;
  pthread_mutex_unlock(&mutex);

  return img;
}

struct image_t *object_detector1(struct image_t *img, uint8_t camera_id);
struct image_t *object_detector1(struct image_t *img, uint8_t camera_id __attribute__((unused)))
{
  return object_detector(img, 1);
}

struct image_t *object_detector2(struct image_t *img, uint8_t camera_id);
struct image_t *object_detector2(struct image_t *img, uint8_t camera_id __attribute__((unused)))
{
  return object_detector(img, 2);
}

struct image_t *object_detector3(struct image_t *img, uint8_t camera_id);
struct image_t *object_detector3(struct image_t *img, uint8_t camera_id __attribute__((unused)))
{
  return object_detector(img, 3);
}

struct image_t *object_detector4(struct image_t *img, uint8_t camera_id);
struct image_t *object_detector4(struct image_t *img, uint8_t camera_id __attribute__((unused)))
{
  return object_detector(img, 4);
}

struct image_t *object_detector5(struct image_t *img, uint8_t camera_id);
struct image_t *object_detector5(struct image_t *img, uint8_t camera_id __attribute__((unused)))
{
  return object_detector(img, 5);
}

void color_object_detector_init(void)
{
  memset(global_filters, 0, 2*sizeof(struct color_object_t));
  pthread_mutex_init(&mutex, NULL);
#ifdef COLOR_OBJECT_DETECTOR_CAMERA1
#ifdef COLOR_OBJECT_DETECTOR_LUM_MIN1
  cod_lum_min1 = COLOR_OBJECT_DETECTOR_LUM_MIN1; // Y
  cod_lum_max1 = COLOR_OBJECT_DETECTOR_LUM_MAX1; // Y
  cod_cb_min1 = COLOR_OBJECT_DETECTOR_CB_MIN1; //min U   // CB stands for colour blue
  cod_cb_max1 = COLOR_OBJECT_DETECTOR_CB_MAX1; //max U
  cod_cr_min1 = COLOR_OBJECT_DETECTOR_CR_MIN1; //min V    //CR stands for colour red.
  cod_cr_max1 = COLOR_OBJECT_DETECTOR_CR_MAX1; //max V
#endif
#ifdef COLOR_OBJECT_DETECTOR_DRAW1
  cod_draw1 = COLOR_OBJECT_DETECTOR_DRAW1;
#endif

  cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA1, object_detector1, COLOR_OBJECT_DETECTOR_FPS1, 0);
#endif

#ifdef COLOR_OBJECT_DETECTOR_CAMERA2
#ifdef COLOR_OBJECT_DETECTOR_LUM_MIN2
  cod_lum_min2 = COLOR_OBJECT_DETECTOR_LUM_MIN2;
  cod_lum_max2 = COLOR_OBJECT_DETECTOR_LUM_MAX2;
  cod_cb_min2 = COLOR_OBJECT_DETECTOR_CB_MIN2;
  cod_cb_max2 = COLOR_OBJECT_DETECTOR_CB_MAX2;
  cod_cr_min2 = COLOR_OBJECT_DETECTOR_CR_MIN2;
  cod_cr_max2 = COLOR_OBJECT_DETECTOR_CR_MAX2;
#endif
#ifdef COLOR_OBJECT_DETECTOR_DRAW2
  cod_draw2 = COLOR_OBJECT_DETECTOR_DRAW2;
#endif

  cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA2, object_detector2, COLOR_OBJECT_DETECTOR_FPS2, 0);
#endif

#ifdef COLOR_OBJECT_DETECTOR_CAMERA3
#ifdef COLOR_OBJECT_DETECTOR_LUM_MIN3
  cod_lum_min3 = COLOR_OBJECT_DETECTOR_LUM_MIN3;
  cod_lum_max3 = COLOR_OBJECT_DETECTOR_LUM_MAX3;
  cod_cb_min3 = COLOR_OBJECT_DETECTOR_CB_MIN3;
  cod_cb_max3 = COLOR_OBJECT_DETECTOR_CB_MAX3;
  cod_cr_min3 = COLOR_OBJECT_DETECTOR_CR_MIN3;
  cod_cr_max3 = COLOR_OBJECT_DETECTOR_CR_MAX3;
#endif
#ifdef COLOR_OBJECT_DETECTOR_DRAW3
  cod_draw3 = COLOR_OBJECT_DETECTOR_DRAW3;
#endif

  cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA3, object_detector3, COLOR_OBJECT_DETECTOR_FPS3, 0);
#endif

#ifdef COLOR_OBJECT_DETECTOR_CAMERA4
#ifdef COLOR_OBJECT_DETECTOR_LUM_MIN4
  cod_lum_min4 = COLOR_OBJECT_DETECTOR_LUM_MIN4;
  cod_lum_max4 = COLOR_OBJECT_DETECTOR_LUM_MAX4;
  cod_cb_min4 = COLOR_OBJECT_DETECTOR_CB_MIN4;
  cod_cb_max4 = COLOR_OBJECT_DETECTOR_CB_MAX4;
  cod_cr_min4 = COLOR_OBJECT_DETECTOR_CR_MIN4;
  cod_cr_max4 = COLOR_OBJECT_DETECTOR_CR_MAX4;
#endif
#ifdef COLOR_OBJECT_DETECTOR_DRAW4
  cod_draw4 = COLOR_OBJECT_DETECTOR_DRAW4;
#endif

  cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA4, object_detector4, COLOR_OBJECT_DETECTOR_FPS4, 0);
#endif

#ifdef COLOR_OBJECT_DETECTOR_CAMERA5
#ifdef COLOR_OBJECT_DETECTOR_LUM_MIN5
  cod_lum_min5 = COLOR_OBJECT_DETECTOR_LUM_MIN5;
  cod_lum_max5 = COLOR_OBJECT_DETECTOR_LUM_MAX5;
  cod_cb_min5 = COLOR_OBJECT_DETECTOR_CB_MIN5;
  cod_cb_max5 = COLOR_OBJECT_DETECTOR_CB_MAX5;
  cod_cr_min5 = COLOR_OBJECT_DETECTOR_CR_MIN5;
  cod_cr_max5 = COLOR_OBJECT_DETECTOR_CR_MAX5;
#endif
#ifdef COLOR_OBJECT_DETECTOR_DRAW5
  cod_draw5 = COLOR_OBJECT_DETECTOR_DRAW5;
#endif

  cv_add_to_device(&COLOR_OBJECT_DETECTOR_CAMERA5, object_detector5, COLOR_OBJECT_DETECTOR_FPS5, 0);
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
uint32_t find_object_centroid(struct image_t *img, int32_t* p_xc, int32_t* p_yc, bool draw,
                              uint8_t lum_min, uint8_t lum_max,
                              uint8_t cb_min, uint8_t cb_max,
                              uint8_t cr_min, uint8_t cr_max,
                              int colour)
{
  uint32_t cnt = 0;
  uint32_t cntrd_count = 0;
  uint32_t tot_x = 0;
  uint32_t tot_y = 0;
  uint8_t *buffer = img->buf;
  


  /** NR - The below section loops through all pixels, 
   * and checks if the pixel is within given colour range.
   * Then returns the total count of pixels that fall within the range.
   * I've changed it to only multiply once rather than like several thousand times...
   */

  /* 27 March
  * Below has been updated to check density of each colour in columns.
  * The purpose of this is to get rid of some of the outside noise.
  * However, to be honest, it doesn't work very well.
  * But i think it's better than not having it.
  * Still need to figure out that cross white-black board.
  */
  

  uint32_t x = 0;
  uint32_t y = 0;
  
  int tol = 30;
  
// Start pixel count
  uint32_t p = 0;
  //int tol = 30;
  uint32_t area = img->h * img->w;
  
  uint32_t density_column = 0;
  uint32_t cnt_column = 0;

  uint32_t x_min = img->w / 2 - tol;
  uint32_t x_max = img->w / 2 + tol;


  // Go through all the pixels
  while (p < area) {
    if (x >= x_min && x <= x_max){
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
        if ( (*yp >= lum_min) && (*yp <= lum_max) &&
              (*up >= cb_min ) && (*up <= cb_max ) &&
              (*vp >= cr_min ) && (*vp <= cr_max )) {
          
          density_column ++;
          cntrd_count++;

          tot_x += x;
          tot_y += y;
          if (draw){
            //*yp = 255;  // make pixel brighter in image //NR: Commenting this out so we can get good footage.
          }
        }
      }
    // NR: Increment pixel
    p ++;
    x ++;
    if (x >= x_max){ // NR: Wow i see the issue!!! This was x > img->w... Now that it is x >= img->w, it should correctly take the line across the middle
      x = 0;
      y ++;
      //When y resets, check the density of the current column for some colours. Then add to count if good enough density.
      if (colour == 1){ // If orange
        if (density_column > 25){
          cnt_column += density_column;
        }
      } else if (colour == 2){ //black
        if (density_column > 25){
          cnt_column += density_column;
        }
      } else if (colour == 4){ // Dark Black
        if (density_column > 20){ 
          cnt_column += density_column;
        }
      } else if (colour == 5){ // High Lum
        if (density_column > 40){ ///blocks out the background, but turns when close to white board
          cnt_column += density_column;
        } 
      } else{
        cnt_column += density_column;
      }
    
    // NR: Make more the weight of pixels within central range.
      if (y >= 210 && y <= 310){
        cnt += (cnt_column + cnt_column + cnt_column + cnt_column);
      } else if (y >= 140 && y <= 380){  
        cnt += (cnt_column + cnt_column + cnt_column);
      } else if (y >= 70 && y <= 450) {
        cnt += (cnt_column + cnt_column);
      } else {
        cnt ++;
      }
      ////////////////////////////printf("check density: %d", density_column);
      cnt_column = 0;
      density_column = 0;
    }
  }
    if (cntrd_count > 0) {
    *p_xc = (int32_t)roundf(tot_x / ((float) cntrd_count) - img->w * 0.5f);
    *p_yc = (int32_t)roundf(img->h * 0.5f - tot_y / ((float) cntrd_count));
  } else {
    *p_xc = 0;
    *p_yc = 0;
  }
    return cnt;
}


void color_object_detector_periodic(void)
{
  static struct color_object_t local_filters[5];
  pthread_mutex_lock(&mutex);
  memcpy(local_filters, global_filters, 2*sizeof(struct color_object_t));
  pthread_mutex_unlock(&mutex);

  if(local_filters[0].updated){
    AbiSendMsgVISUAL_DETECTION(COLOR_OBJECT_DETECTION1_ID, local_filters[0].x_c, local_filters[0].y_c,
        0, 0, local_filters[0].color_count, 0);
    local_filters[0].updated = false;
  }
  if(local_filters[1].updated){
    AbiSendMsgVISUAL_DETECTION(COLOR_OBJECT_DETECTION2_ID, local_filters[1].x_c, local_filters[1].y_c,
        0, 0, local_filters[1].color_count, 1);
    local_filters[1].updated = false;  // NR: I think we don't have to touch this? This seems to be referring to whether to use floor detection. Which is an interesting prospect.
  }
}