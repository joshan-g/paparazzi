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


def filter_color(image_name = '../dark_black.jpg', y_low = 0, y_high = 24, \
                 u_low = 0, u_high = 255, v_low = 0, v_high = 255, resize_factor=1, showpicbefore=1):
    count_ones = 0;
    im = cv2.imread(image_name);
    im = cv2.resize(im, (int(im.shape[1]/resize_factor), int(im.shape[0]/resize_factor)));
    im = cv2.rotate(im, cv2.ROTATE_90_COUNTERCLOCKWISE)
    YUV = cv2.cvtColor(im, cv2.COLOR_BGR2YUV);
    Filtered = np.zeros([YUV.shape[0], YUV.shape[1]]);
    protected = {}
    for i in getAllRanges():
        for x in range(YUV.shape[1]):
            count_x = 0;
            density_high_lum_x = 0;
            density_black_x = 0;
            good_y_high_lum = []
            good_y_black = []
            density_orange_x = 0;
            good_y_orange = []
            for y in range(YUV.shape[0]):
                """Comment the below out to apply the middle cut filter"""
                if (90 < y < 150): 
                    if(YUV[y,x,0] >= i[0] and YUV[y,x,0] <= i[1] and \
                      YUV[y,x,1] >= i[2] and YUV[y,x,1] <= i[3] and \
                      YUV[y,x,2] >= i[4] and YUV[y,x,2] <= i[5]):
                        YUV[y,x,0] = 255;
                        Filtered[y,x] = 1;
                        count_x += 1;
            if (x < 70 or x > 450):
                count_ones += count_x
            elif (x < 140 or x > 380):
                count_ones += count_x * 2
            elif (x < 210 or x > 310):
                count_ones += count_x * 3
            else:
                count_ones += count_x * 4
                
    plt.figure()
    plt.imshow(Filtered);
    plt.title(f"All colour without density filter for {image_name}");
    if (showpicbefore == 1):
        
        plt.figure();
        RGB = cv2.cvtColor(YUV, cv2.COLOR_YUV2RGB);
        plt.imshow(RGB);
        plt.title(f"All colour without density filter for {image_name}");
        
        plt.figure();
        RGB = cv2.cvtColor(im, cv2.COLOR_BGR2RGB);
        plt.imshow(RGB);
        plt.title(image_name);

    

    print(f"Total count for {image_name}: {count_ones}");

def getOrangeRange(y_low = 70, y_high = 190, \
                 u_low = 0, u_high = 150, v_low = 150, v_high = 255):
    return [y_low, y_high, u_low, u_high, v_low, v_high, "orange"]

def getGreenRange(y_low = 60, y_high = 130, \
                 u_low = 75, u_high = 110, v_low = 120, v_high = 140):
    return [y_low, y_high, u_low, u_high, v_low, v_high, "green"]


def getDarkBlackRange(y_low = 0, y_high = 24, \
                 u_low = 0, u_high = 255, v_low = 0, v_high = 255):
    return [y_low, y_high, u_low, u_high, v_low, v_high, "dark black"]

def getHighLumRange(y_low = 220, y_high = 255, \
                 u_low = 0, u_high = 255, v_low = 0, v_high = 255):
    return [y_low, y_high, u_low, u_high, v_low, v_high, "High Lum"]
    
def getBlackRange(y_low = 70, y_high = 130, \
                 u_low = 0, u_high = 170, v_low = 0, v_high = 155):
    return [y_low, y_high, u_low, u_high, v_low, v_high, "black"]     

def getAllRanges():
    return [getOrangeRange(), getGreenRange(), getDarkBlackRange(), getHighLumRange(), getBlackRange()]

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
    highLum = getHighLumRange();
    
    for image in images:
        filter_color(image_name = image);

        
        
        
        
        
        
        
        
        
        
        