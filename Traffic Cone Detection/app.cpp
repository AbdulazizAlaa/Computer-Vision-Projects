#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

char key;
char* output_window_name = "Camera Output";
char* grayscale_window_name = "Grayscale Image";
char* thresholded_window_name = "Thresholded Image";
char* contours_window_name = "Contours Image";

int canny_lowthreshold = 20;
int canny_highthreshold = 50;
int width_bound = 5;
int height_bound = 5;
int solidity_bound = 9;
int max_val = 200;
int float_max_val = 30;
int color_max_val = 256;
int u_h=0;
int u_s=120;
int u_v=120;
int d_h=256;
int d_s=256;
int d_v=256;

RNG rng(12345);

void canny(int, void*){}

int main(int argc, char** argv)
{

    namedWindow(output_window_name, CV_WINDOW_AUTOSIZE);
    namedWindow(grayscale_window_name, 100);
    namedWindow(thresholded_window_name, 100);
    namedWindow(contours_window_name, 100);

    moveWindow(grayscale_window_name, 20,20);
    moveWindow(thresholded_window_name, 20, 250);
    moveWindow(contours_window_name, 20, 600);


    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);

    VideoCapture cap(0);
    if(!cap.isOpened())  // check if we succeeded
       return -1;

    Mat frame;
    Mat imgGrayScale;
    Mat imgHsv;
    Mat imgRedThresh;
    Mat edges;
    vector< vector<Point> > contours;
    vector< Vec4i > hierarchy;
    vector<Point> approx;
    vector<Point> tempCont;
    vector<vector<Point> > tempVecCont;
    double contPeri;
    Rect bBox;
    double aspectRatio;
    double area;
    double hullArea;
    double solidity;

    createTrackbar("canny low Threshold: ", output_window_name, &canny_lowthreshold, max_val, canny);
    createTrackbar("canny high Threshold: ", output_window_name, &canny_highthreshold, max_val, canny);
    createTrackbar("height: ", output_window_name, &height_bound, float_max_val, canny);
    createTrackbar("width: ", output_window_name, &width_bound, float_max_val, canny);
    createTrackbar("solidity: ", output_window_name, &solidity_bound, float_max_val, canny);
    // createTrackbar("Up H: ", output_window_name, &u_h, color_max_val, canny);
    // createTrackbar("Up S: ", output_window_name, &u_s, color_max_val, canny);
    // createTrackbar("Up v: ", output_window_name, &u_v, color_max_val, canny);
    // createTrackbar("Down H: ", output_window_name, &d_h, color_max_val, canny);
    // createTrackbar("Down S: ", output_window_name, &d_s, color_max_val, canny);
    // createTrackbar("Down v: ", output_window_name, &d_v, color_max_val, canny);


    while(1){

        cap >> frame;

        key = waitKey(10);
        //cout <<(int) char(key) << endl;
        if(char(key) == 27){
            break;
        }
        // if(char(key) == 10){

            imgHsv.create(frame.size(), frame.type());
            cvtColor(frame, imgHsv, CV_BGR2HSV);

            inRange(imgHsv, Scalar(u_h, u_s, u_v), Scalar(d_h, d_s, d_v), imgRedThresh);

            //converting the original image into grayscale
            imgGrayScale.create(frame.size(), frame.type());
            cvtColor(frame, imgGrayScale, CV_BGR2GRAY);
            bitwise_and(imgRedThresh, imgGrayScale, imgGrayScale);

            // Floodfill from point (0, 0)
            Mat im_floodfill = imgGrayScale.clone();
            floodFill(im_floodfill, cv::Point(0,0), Scalar(255));

            // Invert floodfilled image
            Mat im_floodfill_inv;
            bitwise_not(im_floodfill, im_floodfill_inv);

            // Combine the two images to get the foreground.
            imgGrayScale = (imgGrayScale | im_floodfill_inv);

            GaussianBlur(imgGrayScale, imgGrayScale, Size(7,7), 1.5, 1.5);

            imshow(grayscale_window_name, imgGrayScale);

            edges.create(imgGrayScale.size(), imgGrayScale.type());
            Canny(imgGrayScale, edges, canny_lowthreshold, canny_highthreshold, 3);

            findContours(edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
            Mat drawing = Mat::zeros(edges.size(), CV_8UC3);
            for(int i=0 ; i<contours.size() ; i++){
              contPeri = arcLength(contours.at(i), true);
              approxPolyDP(contours.at(i), approx,  0.01 * contPeri, true);

              if(approx.size()>=7 && approx.size()<=9){
                bBox = boundingRect(approx);
                aspectRatio = bBox.width/bBox.height;

                area = contourArea(contours.at(i));
                convexHull(contours.at(i), tempCont);
                hullArea = contourArea(tempCont);
                solidity = area / hullArea;

                // cout << "contour : " << approx.size() << endl;
                // cout << bBox.width << "::" << bBox.height << "::" << solidity  << "::" << aspectRatio <<endl;

                tempVecCont.clear();
                tempVecCont.push_back(approx);

                // if(bBox.width > width_bound && bBox.height > height_bound && solidity > solidity_bound/10 && aspectRatio >= down_aspectRatio/10 && aspectRatio <= up_aspectRatio/10)
                if(bBox.width > width_bound && bBox.height > height_bound && solidity > solidity_bound/10)
                {
                  Scalar color = Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));
                  drawContours(drawing, tempVecCont, 0, color, 2, 8, hierarchy, 0, Point());
                }
              }

            }
            // cout << endl << endl;
            imshow(thresholded_window_name, edges);
            imshow(contours_window_name, drawing);
            // imshow(thresholded_window_name, imgGrayScale);
            // imshow(contours_window_name, imgRedThresh);

        // }
        imshow(output_window_name, frame);

    }
    destroyWindow(output_window_name);
    destroyWindow(thresholded_window_name);
    destroyWindow(grayscale_window_name);
    destroyWindow(contours_window_name);
    cap.release();

    return 0;

}
