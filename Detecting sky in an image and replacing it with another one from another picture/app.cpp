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
Mat nightImg, night_gray, night_dst;

int open_radius = 50;
int close_radius = 20;
int max_radius = 100;

char* window_name = "Edge Map";

void region_fill(Mat & src){
    cv::Mat dst;
    dst = cv::Mat::zeros(src.size(), CV_8U);
    dst.at<uchar>(75,75) = 1;

    cv::Mat prev;
    cv::Mat kernel = (cv::Mat_<uchar>(3,3) << 0, 1, 0, 1, 1, 1, 0, 1, 0);

    do {
        dst.copyTo(prev);
        cv::dilate(dst, dst, kernel);
        dst &= (1 - src);
    }
    while (cv::countNonZero(dst - prev) > 0);

    cv::normalize(src, src, 0, 255, cv::NORM_MINMAX);
    cv::normalize(dst, dst, 0, 255, cv::NORM_MINMAX);
    dst = 255-dst;

    src = dst;
}

void update(int, void*){

    threshold(src_gray, dst, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
    Mat element;

    //Closing to get big coins
    if(close_radius > 0)
    {
        element = getStructuringElement(MORPH_ELLIPSE, Size(close_radius, close_radius));

        morphologyEx(dst, dst, MORPH_CLOSE, element);
    }

    //Opening to get big coins
    if(open_radius > 0)
    {
        element = getStructuringElement(MORPH_ELLIPSE, Size(open_radius, open_radius));

        morphologyEx(dst, dst, MORPH_OPEN, element);
    }

    Mat clone = src.clone();

    for(int i=0 ; i<nightImg.rows ; i++){
        for(int j=0 ; j<nightImg.cols ; j++){
            if((int) dst.at<uchar>(i,j) == 0){
                clone.at<Vec3b>(i,j)[0] = nightImg.at<Vec3b>(i,j)[0];
                clone.at<Vec3b>(i,j)[1] = nightImg.at<Vec3b>(i,j)[1];
                clone.at<Vec3b>(i,j)[2] = nightImg.at<Vec3b>(i,j)[2];
            }
        }
    }

    imshow("Source Image", clone);
    imshow("night Image", nightImg);
    imshow(window_name, dst);
}

/** @function main */
int main( int argc, char** argv )
{

      if(argc == 1)
      {
          cout << "Usage : ./app nightImage.jpg" << endl ;
          return -1;
      }

  /// Load an image
  src = imread( "1.jpg" );
  nightImg = imread(argv[1]);

  if( !src.data )
  {
      cout << "Image could not be loaded!!" << endl ;
      return -1;
  }

  if( !nightImg.data )
  {
      cout << "night Image could not be loaded!!" << endl ;
      return -1;
  }

  /// Create a matrix of the same type and size as src (for dst)
  dst.create( src.size(), src.type() );

  /// Convert the image to grayscale
  cvtColor( src, src_gray, CV_BGR2GRAY );

  /// Create a matrix of the same type and size as src (for dst)
  night_dst.create( nightImg.size(), nightImg.type() );

  /// Convert the image to grayscale
  cvtColor( nightImg, night_gray, CV_BGR2GRAY );

  /// Create a window
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  createTrackbar( "Open Radius:", window_name, &open_radius, max_radius, update );
  createTrackbar( "Close Radius:", window_name, &close_radius, max_radius, update );

  /// Show the image
  update(0, 0);

  /// Wait until user exit program by pressing a key
  waitKey(0);

  return 0;
  }
