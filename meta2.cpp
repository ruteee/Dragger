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


    //CvCapture* capture = 0;
    //capture = cvCaptureFromCAM(CV_CAP_ANY); //0=default, -1=any camera, 1..99=your camera
    //if (!capture) {
      //cout << "No camera detected" << endl;
    //}
    const char* filename = argc >= 2 ? argv[1] : "board.jpg";
    do {
        img = imread(filename, 1);
    //    img = cvQueryFrame(capture);
        Mat img_new;
        Mat img_trabalho;
        Mat img_configuracao;
        img.copyTo(img_configuracao);

        GaussianBlur(img, img_new,  Size(9, 9), 0.5, 0.5);
    	cvtColor(img_new, hsv_img, COLOR_BGR2HSV);

        //inRange(hsv_img, Scalar(21, 89, 232),Scalar(21, 194 , 255), yellow);

        Mat branca(Size(640,480), CV_8UC3, Scalar(255,255, 255));

        Mat yellow;
        inRange(hsv_img, Scalar(21, 100, 200),Scalar(30, 200, 255), yellow);
        GaussianBlur(yellow, yellow, Size(9, 9), 4, 4);

        Mat blue;
        inRange(hsv_img, Scalar(100, 60, 100), Scalar(120,210,210), blue);
        GaussianBlur(blue,blue,  Size(9, 9), 4, 4);

        cvtColor(img, cimg, COLOR_BGR2GRAY);
        vector<Vec3f> circles(1);
        GaussianBlur(cimg,cimg,  Size(9, 9), 0.5, 0.5);
        HoughCircles(cimg, circles, CV_HOUGH_GRADIENT, 1, 8,
                     100, 30, 1, 25 // change the last two parameters
                                    // (min_radius & max_radius) to detect larger circles
                     );

        for( size_t i = 0; i < circles.size(); i++ ){
            Vec3i c = circles[i];
            circle(branca, Point(c[0], c[1]), 20, Scalar(0,0,255),-1, 3, CV_AA);
        }

        //Partida
        HoughCircles(yellow, circles, CV_HOUGH_GRADIENT, 1, 10,
                     100, 30, 20, 38 // change the last two parameters
                                    // (min_radius & max_radius) to detect larger circles
                     );
        for( size_t i = 0; i < circles.size(); i++ ){
            Vec3i c = circles[i];
            circle(branca, Point(c[0], c[1]), 4, Scalar(0,255,255),-1, 3, CV_AA);
        }

        //Chegada
        HoughCircles(blue, circles, CV_HOUGH_GRADIENT, 1, 10,
                     100, 30, 20, 38 // change the last two parameters
                                    // (min_radius & max_radius) to detect larger circles
                     );

         for( size_t i = 0; i < circles.size(); i++ ){
             Vec3i c = circles[i];
             circle(branca, Point(c[0], c[1]), 4, Scalar(255,0,0),-1, 3, CV_AA);
         }



        imshow("branco", branca);

        //mapeamento de osbtaculos
        int mapa[640*480];
        bool partida = false;
        bool chegada = false;

        for(int i = 0; i < branca.rows; i++){
            for(int j = 0; j < branca.cols; j++){
                Vec3b bgrPixel = branca.at<Vec3b>(i, j);
                if(bgrPixel[0] == 0 && bgrPixel[1] == 0 && bgrPixel[2]==255){
                    mapa[i*branca.cols + j] = 3;
                }else if(bgrPixel[0] == 255 && bgrPixel[1] == 0 && bgrPixel[2]==0 && !chegada){
                    mapa[i*branca.cols + j] =-1 ;
                    chegada = true;
                }else if(bgrPixel[0] == 0 && bgrPixel[1] == 255 && bgrPixel[2]==255 && !partida){
                    mapa[i*branca.cols + j] =-2 ;
                    partida = true;
                }else{
                    mapa[i*branca.cols + j] = 0;
                }
            }
        }

        for(int i = 0; i < branca.rows*branca.cols; i++){
                printf("%d", mapa[i]);
        }

        //imshow("Configuracao", img_configuracao);
        waitKey(0);


        key = waitKey(100);
        printf("Tecla: %d\n", key);

  } while (key != 1048683 || key == -1);
  return 0;
}
