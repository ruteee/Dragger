#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

static void help()
{
    cout << "\nThis program demonstrates circle finding with the Hough transform.\n"
            "Usage:\n"
            "./houghcircles <image_name>, Default is pic1.png\n" << endl;
}

int main(int argc, char** argv)
{
    Mat hsv_img;
    Mat yellow;
    Mat img;
    Mat cimg;
    int key = 0;


    CvCapture* capture = 0;
    capture = cvCaptureFromCAM(CV_CAP_ANY); //0=default, -1=any camera, 1..99=your camera
    if (!capture) {
      cout << "No camera detected" << endl;
    }
    //const char* filename = argc >= 2 ? argv[1] : "board.jpg";
    do {
     //img = imread(filename, 1);
        img = cvQueryFrame(capture);
        Mat img_trabalho;
        Mat img_configuracao;
        img.copyTo(img_trabalho);
        img.copyTo(img_configuracao);


        cvtColor(img, cimg, COLOR_BGR2GRAY);
        vector<Vec3f> circles(1);
        GaussianBlur(cimg,cimg,  Size(9, 9), 4, 4);
        HoughCircles(cimg, circles, CV_HOUGH_GRADIENT, 1, 8,
                     100, 30, 1, 25 // change the last two parameters
                                    // (min_radius & max_radius) to detect larger circles
                     );

         for( size_t i = 0; i < circles.size(); i++ ){
             Vec3i c = circles[i];
             circle(img_trabalho, Point(c[0], c[1]), c[2], Scalar(0,0,0), 3, CV_AA);
             circle(img_trabalho, Point(c[0], c[1]), c[2], Scalar(0,0,255),-1, 3, CV_AA);
         }

        for( size_t i = 0; i < circles.size(); i++ ){
            Vec3i c = circles[i];
            circle(img_configuracao, Point(c[0], c[1]), 20, Scalar(0,0,255),-1, 3, CV_AA);
        }

        HoughCircles(cimg, circles, CV_HOUGH_GRADIENT, 1, 10,
                     100, 30, 20, 38 // change the last two parameters
                                    // (min_radius & max_radius) to detect larger circles
                     );

         for( size_t i = 0; i < circles.size(); i++ ){
             Vec3i c = circles[i];
             circle(img_trabalho, Point(c[0], c[1]), c[2], Scalar(0,0,0), 3, CV_AA);
             circle(img_trabalho, Point(c[0], c[1]), c[2], Scalar(0,255,0),-1, 3, CV_AA);
         }

        for( size_t i = 0; i < circles.size(); i++ ){
            Vec3i c = circles[i];
            circle(img_configuracao, Point(c[0], c[1]), 10, Scalar(0,255,0),-1, 3, CV_AA);
        }

        imshow("Trabalho", img_trabalho);
        imshow("Configuracao", img_configuracao);

        key = waitKey(500);
        printf("Tecla: %d\n", key);

  } while (key != 1048683 || key == -1);
  return 0;
}
