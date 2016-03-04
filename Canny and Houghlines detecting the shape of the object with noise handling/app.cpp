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
int lowThreshold=100;
int highThreshold=200;
int const max_lowThreshold = 500;
int const max_sigma = 100;
int ratio = 3;
int kernel_size = 3;
int sigma=2;
int houghThresh = 35;
int minLineLength = 20;

char* window_name = "Edge Map";

void getLines(Mat & detEdges, Mat & src){
    vector<Vec4i> lines;
    HoughLinesP(detEdges, lines, 1, CV_PI/180, houghThresh, minLineLength, 10 );
    Mat clone = src.clone();
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        line( clone, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 2, CV_AA);
    }
    imshow("lines", clone);

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

  getLines(detected_edges, src);

  imshow( window_name, detected_edges );
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
  /// Create a Trackbar for user to enter threshold
  createTrackbar( "Hough Threshold:", window_name, &houghThresh, max_sigma, CannyThreshold );
  /// Create a Trackbar for user to enter threshold
  createTrackbar( "Min Line Length:", window_name, &minLineLength, max_sigma, CannyThreshold );


  /// Show the image
  CannyThreshold(0, 0);

  /// Wait until user exit program by pressing a key
  waitKey(0);

  return 0;
  }
