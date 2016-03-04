#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <time.h>

using namespace cv;
using namespace std;
/// Global variables

Mat src, src_gray;
Mat dst, detected_edges;

int open_radius = 50;
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

    threshold(src_gray, dst, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

    region_fill(dst);

    // Mat erosionKernal = cv::Mat::ones(5,5, CV_8U);
    //
    // cv::erode(dst, dst, erosionKernal);
    //
    // erosionKernal = cv::Mat::ones(3,3, CV_8U);
    //
    // cv::erode(dst, dst, erosionKernal);

    Mat temp;
    Mat structuringElem;
    int radius = 1;
    vector<vector<Point> > contours;
    int size = 0;
    int count = 0;
    vector<Mat> varient;
    bool done = false;
    stringstream ss;
    int blue;
    int red;
    int green;
    srand(time(NULL));

    while(!done){
        //Opening to get big coins
        temp = dst.clone();

        structuringElem = getStructuringElement(MORPH_ELLIPSE, Size(radius, radius));

        morphologyEx(temp, temp, MORPH_OPEN, structuringElem);

        findContours(temp.clone(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

        if(contours.size()/2 == 0)
            done = true;
        else if(contours.size()/2 != size){
            count = 0;
            varient.push_back(temp.clone());
            size = contours.size()/2;
            ss.clear();
            ss.str(string());
        }else{
            count++;
            radius += 5;
            if(count > 10)
                done = true;
        }

    }


    Mat t = varient[varient.size()-1].clone();

    for(int i=varient.size()-1 ; i>0 ; i--){
        varient[i] = varient[i-1] - varient[i];

        structuringElem = getStructuringElement(MORPH_ELLIPSE, Size(open_radius2, open_radius2));
        morphologyEx(varient[i], varient[i], MORPH_OPEN, structuringElem);

        Mat bound;
        Mat dilationKernal = cv::Mat::ones(5,5, CV_8U);

        cv::dilate(varient[i], bound, dilationKernal);
        bound = bound - varient[i];
        blue = (rand()%255)*i;
        red = (rand()%255)*i;
        green = (rand()%255)*i;
        for(int i=0 ; i<bound.rows ; i++){
            for(int j=0 ; j<bound.cols ; j++){
                if((int)bound.at<uchar>(i,j) > 150){
                    src.at<Vec3b>(i,j)[0] = blue;
                    src.at<Vec3b>(i,j)[1] = red;
                    src.at<Vec3b>(i,j)[2] = green;
                }
            }
        }
    }
    varient[0] = t;

    Mat bound;
    Mat dilationKernal = cv::Mat::ones(5,5, CV_8U);

    cv::dilate(varient[0], bound, dilationKernal);
    bound = bound - varient[0];
    blue = (rand()%255)*0;
    red = (rand()%255)*0;
    green = (rand()%255)*0;
    for(int i=0 ; i<bound.rows ; i++){
        for(int j=0 ; j<bound.cols ; j++){
            if((int)bound.at<uchar>(i,j) > 150){
                src.at<Vec3b>(i,j)[0] = blue;
                src.at<Vec3b>(i,j)[1] = red;
                src.at<Vec3b>(i,j)[2] = green;
            }
        }
    }

    for(int i=0 ; i<varient.size() ; i++){
        ss << i;
        imshow("Coin: "+ ss.str(), varient[i]);

        findContours(varient[i], contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

        for(int j=0 ; j<contours.size() ; j++){
            putText(src, ss.str(), contours[j][0], FONT_HERSHEY_SIMPLEX, .5, Scalar(255, 0, 0), 1, CV_AA);
        }

        cout << "Coin: " << i << " -- count: " << contours.size() << endl;
        ss.clear();
        ss.str(string());
    }

    imshow("image", src);
    imshow(window_name, dst);
}

/** @function main */
int main( int argc, char** argv )
{

    if(argc == 1){
        cout << "Usage: ./app ImageName.EXT" << endl;
        return -1;
    }

    /// Load an image
    //src = imread( "Coins2.jpg" );
    src = imread(argv[1]);

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
