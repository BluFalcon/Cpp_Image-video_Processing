//g++ -Wall cp_mat.cpp -o babe `pkg-config --cflags --libs opencv` -L/usr/local/cuda-6.5/lib64


#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;


Mat anoixe()
{

Mat kati;

kati = imread("mwro.jpg",1);

return kati;

}



int main( int argc, const char** argv )
{
    Mat frame;

frame =  anoixe();

imshow("foo", frame);


waitKey( 1325 );



    return 0;



}
