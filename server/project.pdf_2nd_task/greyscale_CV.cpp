#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <highgui.h>
#include <iostream>
using namespace cv;

int greyscale_CV( )
{
 

 Mat image;
 image = imread( "image.png", 1 );

 if( !image.data )
 {
   printf( " No image data \n " );
   return -1;
 }

 Mat gray_image;
 cvtColor( image, gray_image, CV_BGR2GRAY );

 imwrite( "Gray_Image.png", gray_image );

 return 0;
}

