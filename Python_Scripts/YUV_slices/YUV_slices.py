# -*- coding: utf-8 -*-
"""
Created on Sun Feb 18 08:59:33 2018

Generate and show slices of YUV

@author: LocalAdmin
"""
import sys
print(sys.executable)
import cv2;
cv2.__version__
import matplotlib.pyplot as plt
import numpy as np
import YUV_slices_alltogether as other


def YUV_to_RGB(im):
    """ Convert YUV to RGB """
    if(np.max(im[:]) <= 1.0):
        im *= 255;
        
    Y = im[:,:,0];
    U = im[:,:,1];
    V = im[:,:,2];
    
    R  = Y + 1.402   * ( V - 128 )
    G  = Y - 0.34414 * ( U - 128 ) - 0.71414 * ( V - 128 )
    B  = Y + 1.772   * ( U - 128 )

    rgb = im;
    rgb[:,:,0] = R / 255.0;
    rgb[:,:,1] = G / 255.0;
    rgb[:,:,2] = B / 255.0;

    inds1 = np.where(rgb < 0.0);
    for i in range(len(inds1[0])):
        rgb[inds1[0][i], inds1[1][i], inds1[2][i]] = 0.0;
        
    inds2 = np.where(rgb > 1.0);
    for i in range(len(inds2[0])):
        rgb[inds2[0][i], inds2[1][i], inds2[2][i]] = 1.0;
    return rgb;

def generate_slices_YUV(n_slices = 5, H = 255, W = 255):
    """ Generate YUV slices and convert them to RGB to show them """
#    bgr = np.zeros([H, W, 3]);
#    bgr[:,:,2] = 1.0;
#    cv2.imshow('BGR', bgr);
#    cv2.waitKey();
    
    plt.ion();
    
    V_step = 1.0 / H;
    U_step = 1.0 / W;    
    Y_step = 1.0 / (n_slices-1);
    
    for s in range(n_slices):
        
        im = np.zeros([H, W, 3]);
        Y = s * Y_step;
        im[:,:,0] = Y;
        
        # print('Slice %d / %d, Y = %f' % (s, n_slices, Y));
        
        for y in range(H):
            for x in range(W):
                im[y,x,1] = U_step * x;
                im[y,x,2] = V_step * y;
                
        rgb = YUV_to_RGB(im);
        
        plt.figure()
        plt.imshow(rgb);
        plt.xlabel('U');
        plt.ylabel('V');
        plt.title('Y = ' + str(Y));

#        im=im.astype(np.float32)
#        bgr = cv2.cvtColor(im, cv2.COLOR_YUV2BGR);
#        cv2.imshow('BGR', bgr); cv2.waitKey();



def filter_color(image_name = '../outside.jpg', y_low = 220, y_high = 255, \
                 u_low = 0, u_high = 255, v_low = 0, v_high = 255, resize_factor=1, title="Filtered Image", showpicbefore=1):
    count_ones = 0;
    im = cv2.imread(image_name);
    im = cv2.resize(im, (int(im.shape[1]/resize_factor), int(im.shape[0]/resize_factor)));
    im = cv2.rotate(im, cv2.ROTATE_90_COUNTERCLOCKWISE)
    YUV = cv2.cvtColor(im, cv2.COLOR_BGR2YUV);
    Filtered = np.zeros([YUV.shape[0], YUV.shape[1]]);
    #print(y_high)
    for x in range(YUV.shape[1]):
        count_x = 0;
        for y in range(YUV.shape[0]):
            """Comment the below out to apply the middle cut filter"""
            if (90 < y <= 150): 
                if(YUV[y,x,0] >= y_low and YUV[y,x,0] <= y_high and \
                  YUV[y,x,1] >= u_low and YUV[y,x,1] <= u_high and \
                  YUV[y,x,2] >= v_low and YUV[y,x,2] <= v_high):
                       Filtered[y,x] = 1;
                       count_x += 1;
                       YUV[y,x,0] = 255;
        if count_x > 50:
            if (x < 70 or x > 450):
                count_ones += count_x
            elif (x < 140 or x > 380):
                count_ones += count_x * 2
            elif (x < 210 or x > 310):
                count_ones += count_x * 3
            else:
                count_ones += count_x * 4           
    
    if (showpicbefore == 1):
        
        plt.figure();
        RGB = cv2.cvtColor(YUV, cv2.COLOR_YUV2RGB);
        plt.imshow(RGB);
        plt.title(image_name);

    plt.figure()
    plt.imshow(Filtered);
    plt.title(title);

    print(f"{title} count for {image_name} = {count_ones}");

def getOrangeRange(y_low = 80, y_high = 190, \
                 u_low = 0, u_high = 150, v_low = 150, v_high = 255):
    return [y_low, y_high, u_low, u_high, v_low, v_high, "orange"]
    
def getBlackRange(y_low = 70, y_high = 130, \
                 u_low = 0, u_high = 170, v_low = 0, v_high = 155):
    return [y_low, y_high, u_low, u_high, v_low, v_high, "black"]     

def getGreenRange(y_low = 60, y_high = 130, \
                 u_low = 75, u_high = 110, v_low = 120, v_high = 140):
    return [y_low, y_high, u_low, u_high, v_low, v_high, "green"]


def getDarkBlackRange(y_low = 0, y_high = 24, \
                 u_low = 0, u_high = 255, v_low = 0, v_high = 155):
    return [y_low, y_high, u_low, u_high, v_low, v_high, "dark black"]

def getHighLuminescenceRange(y_low = 220, y_high = 255, \
                 u_low = 0, u_high = 255, v_low = 0, v_high = 255):
    return [y_low, y_high, u_low, u_high, v_low, v_high, "High Luminecense"]



if __name__ == '__main__':
    generate_slices_YUV();
    filter_color();
    
    images = ["../all.jpg", "../outside_g.jpg","../closer_black.jpg","../cross-board.jpg", \
              "../green_close.jpg","../black-board-bright.jpg",  "../outside.jpg",\
                  "../Whiteboard.jpg", "../dark_black.jpg", "../Board_Behind.jpg"]
    
    orange = getOrangeRange()   
    black = getBlackRange()
    green = getGreenRange()
    darkBlack = getDarkBlackRange()
    highLum = getHighLuminescenceRange();
    
    for image in images:
        filter_color(image_name = image, y_low = orange[0], y_high = orange[1], u_low = orange[2], u_high = orange[3], v_low = orange[4], v_high = orange[5], resize_factor=1, title = orange[6], showpicbefore=1)
        filter_color(image_name = image, y_low = green[0], y_high = green[1], u_low = green[2], u_high = green[3], v_low = green[4], v_high = green[5], resize_factor=1, title = green[6], showpicbefore=0)  
        filter_color(image_name = image, y_low = darkBlack[0], y_high = darkBlack[1], u_low = darkBlack[2], u_high = darkBlack[3], v_low = darkBlack[4], v_high = darkBlack[5], resize_factor=1, title = darkBlack[6], showpicbefore=0)
        
        filter_color(image_name = image, y_low = black[0], y_high = black[1], u_low = black[2], u_high = black[3], v_low = black[4], v_high = black[5], resize_factor=1, title = black[6], showpicbefore=0)
            
        
        filter_color(image_name = image, y_low = highLum[0], y_high = highLum[1], u_low = highLum[2], u_high = highLum[3], v_low = highLum[4], v_high = highLum[5], resize_factor=1, title = highLum[6], showpicbefore=0)
        
        other.filter_color(image_name = image);
        


        
        
        
        
        
        
        
        
        
        
        