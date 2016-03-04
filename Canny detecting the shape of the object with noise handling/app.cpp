#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;
/// Global variables

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold=0;
int highThreshold=200;
int const max_lowThreshold = 500;
int const max_sigma = 100;
int ratio = 3;
int kernel_size = 3;
int sigma=20;

char* window_name = "Edge Map";

void findCoord(Mat & img, Mat & dst)
{

    bool f = false;
    Point first, second, third, forth;

    for(int i=0 ; i<img.rows ; i++){
        for(int j=0 ; j<img.cols ; j++){
            if((int) img.at<uchar>(i,j) > 50 ){
                first = Point(j,i);
                for(int k=img.cols ; k>first.x ; k--){
                    if((int) img.at<uchar>(i,k) > 0){
                        second = Point(k,i);
                        break;
                    }
                }
                f = true;
                break;
            }
        }
        if(f)
            break;
    }
    f=false;
    for(int i=img.rows-1 ; i>=0 ; i--){
        for(int j=0 ; j<img.cols ; j++){
            if((int) img.at<uchar>(i,j) > 200 ){
                third = Point(j,i);
                for(int k=img.cols ; k>third.x ; k--){
                    if((int) img.at<uchar>(i,k) > 0){
                        forth = Point(k,i);
                        break;
                    }
                }
                f = true;
                break;
            }
        }
        if(f)
            break;
    }

    Mat clone = src.clone();

    line(clone, first, second, Scalar(0,0,255), 1);
    line(clone, second, forth, Scalar(0,0,255), 1);
    line(clone, forth, third, Scalar(0,0,255), 1);
    line(clone, third, first, Scalar(0,0,255), 1);

    imshow("Final Image", clone);



}

/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*)
{
  /// Reduce noise with a kernel 3x3
  GaussianBlur( src_gray, detected_edges, Size(7,7), sigma);

  /// Canny detector
  Canny( detected_edges, detected_edges, lowThreshold, highThreshold, kernel_size );

  /// Using Canny's output as a mask, we display our result
  dst = Scalar::all(0);

  src.copyTo( dst, detected_edges);

  findCoord(detected_edges, dst);

  imshow( window_name, dst );
}


/** @function main */
int main( int argc, char** argv )
{
  /// Load an image
  src = imread( "salt.tif" );

  if( !src.data )
  { return -1; }

  /// Create a matrix of the same type and size as src (for dst)
  dst.create( src.size(), src.type() );

  /// Convert the image to grayscale
  cvtColor( src, src_gray, CV_BGR2GRAY );

  /// Create a window
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  /// Create a Trackbar for user to enter threshold
  createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );
  /// Create a Trackbar for user to enter threshold
  createTrackbar( "high Threshold:", window_name, &highThreshold, max_lowThreshold, CannyThreshold );
  /// Create a Trackbar for user to enter threshold
  createTrackbar( "Sigma:", window_name, &sigma, max_sigma, CannyThreshold );


  /// Show the image
  CannyThreshold(0, 0);

  /// Wait until user exit program by pressing a key
  waitKey(0);

  return 0;
  }
