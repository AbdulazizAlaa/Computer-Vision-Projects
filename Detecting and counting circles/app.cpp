#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;
/// Global variables

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold=150;
int highThreshold=300;
int const max_lowThreshold = 500;
int const max_sigma = 100;
int ratio = 3;
int kernel_size = 3;
int sigma=1;
int houghThresh = 30;
int dp = 1;
int minDis = 10;
int countBig = 0;
int countSmall = 0;

char* window_name = "Edge Map";

void getLines(Mat & detEdges, Mat & src){
    if(houghThresh < 1)
        houghThresh = 1;
    if(dp < 1)
        dp = 1;
    if(minDis < 1)
        minDis = 1;
    vector<Vec3f> circles;
    HoughCircles(detEdges, circles, CV_HOUGH_GRADIENT, dp, minDis, highThreshold, houghThresh, 0, 100);
    Mat clone = src.clone();
    for( size_t i = 0; i < circles.size(); i++ )
    {
        Vec3i c = circles[i];
        circle( clone, Point(c[0], c[1]), c[2], Scalar(0,0,255), 1, CV_AA);
        circle( clone, Point(c[0], c[1]), 2, Scalar(0,255,0), 3, CV_AA);

        stringstream ss;
        ss << i;
        string s = ss.str();

        putText(clone, s, Point(c[0], c[1]), FONT_HERSHEY_SIMPLEX, .5, Scalar(255, 0, 0), 1, CV_AA);
        cout << "Circle " << i << ": " << c[2] << endl;
        if(c[2] >= 27 && c[2] <= 34)
            countBig++;

        if(c[2] >= 19 && c[2] <= 25)
            countSmall++;

    }

    cout << countBig << " * 100 + " << countSmall << " * 50 = " << (countBig*100)+(countSmall*50) << endl;

    imshow("circles", clone);
    imshow("image", src);
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
  src = imread( "coins.png" );

  if( !src.data )
  {
      cout << "Image Could not be loaded." << endl;
      return -1;
   }

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
  createTrackbar( "dp:", window_name, &dp, max_sigma, CannyThreshold );
  /// Create a Trackbar for user to enter threshold
  createTrackbar( "Min Distance:", window_name, &minDis, max_sigma, CannyThreshold );


  /// Show the image
  CannyThreshold(0, 0);

  /// Wait until user exit program by pressing a key
  waitKey(0);

  return 0;
  }
