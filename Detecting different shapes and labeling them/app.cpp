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

int open_radius = 20;
int open_radius2 = 5;
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

    region_fill(dst);

    //Opening to get big coins
    Mat imgB = dst.clone();

    if(open_radius > 0)
    {
        element = getStructuringElement(MORPH_ELLIPSE, Size(open_radius, open_radius));

        morphologyEx(imgB, imgB, MORPH_OPEN, element);
    }

    //Opening to get small coins
    Mat imgC = dst.clone();

    imgC = imgC - imgB;

    if(open_radius2 > 0)
    {
        element = getStructuringElement(MORPH_ELLIPSE, Size(open_radius2, open_radius2));

        morphologyEx(imgC, imgC, MORPH_OPEN, element);
    }


    //get small coins boundary
    Mat boundC;
    Mat dilationKernal = cv::Mat::ones(5,5, CV_8U);

    cv::dilate(imgC, boundC, dilationKernal);
    boundC = boundC - imgC;

    //get big coins boundary
    Mat boundB;

    cv::dilate(imgB, boundB, dilationKernal);
    boundB = boundB - imgB;

    //
    for(int i=0 ; i<boundB.rows ; i++){
        for(int j=0 ; j<boundC.cols ; j++){
            if((int)boundB.at<uchar>(i,j) > 150){
                src.at<Vec3b>(i,j)[0] = 0;
                src.at<Vec3b>(i,j)[1] = 0;
                src.at<Vec3b>(i,j)[2] = 255;
            }
            if((int)boundC.at<uchar>(i,j) > 150){
                src.at<Vec3b>(i,j)[0] = 255;
                src.at<Vec3b>(i,j)[1] = 0;
                src.at<Vec3b>(i,j)[2] = 0;
            }
        }
    }

    vector<vector<Point> > coinContours;
    findContours(boundB, coinContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    vector<vector<Point> > penContours;
    findContours(boundC, penContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    for(int i=0 ; i<coinContours.size()-1 ; i+=2){
        putText(src, "Coin", coinContours[i][0], FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0), 1, CV_AA);
    }

    for(int i=0 ; i<penContours.size() ; i+=2){
        putText(src, "Pen", penContours[i][0], FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255,0,0), 1, CV_AA);
    }

    imshow("pen", imgC);
    imshow("coins", imgB);

    imshow("Source Image", src);

    imshow(window_name, dst);
}

/** @function main */
int main( int argc, char** argv )
{
  /// Load an image
  src = imread( "Coins&pen.jpg" );

  if( !src.data )
  {
      cout << "Image could not be loaded!!" << endl ;
      return -1;
  }

  /// Create a matrix of the same type and size as src (for dst)
  dst.create( src.size(), src.type() );

  /// Convert the image to grayscale
  cvtColor( src, src_gray, CV_BGR2GRAY );

  /// Create a window
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  createTrackbar( "Open Radius:", window_name, &open_radius, max_radius, update );
  createTrackbar( "2nd Open Radius:", window_name, &open_radius2, max_radius, update );


  /// Show the image
  update(0, 0);

  /// Wait until user exit program by pressing a key
  waitKey(0);

  return 0;
  }
