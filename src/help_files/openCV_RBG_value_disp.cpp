//g++ -Wall openCV_RBG_value_disp.cpp -o camcpp `pkg-config --cflags --libs opencv` -L/usr/local/cuda-6.5/lib64


#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;


int main( int argc, const char** argv )
{
    CvCapture* capture = 0;
    Mat frame, frameCopy, image;

    capture = cvCaptureFromCAM( 0 ); //0=default, -1=any camera, 1..99=your camera
    if(!capture) cout << "No camera detected" << endl;

    cvNamedWindow( "result", 1 );

    if( capture )
    {
        cout << "In capture ..." << endl;
        for(;;)
        {
            IplImage* iplImg = cvQueryFrame( capture );
            frame = iplImg;
            if( frame.empty() )
                break;
         //   if( iplImg->origin == IPL_ORIGIN_TL )
           //     frame.copyTo( frameCopy );
           // else
            //    flip( frame, frameCopy, 0 );




//http://docs.opencv.org/modules/core/doc/basic_structures.html

cout << "O # twn kanaliwn: " << frame.channels()  <<endl;
cout << "Total num of pixels: " << frame.total()  <<endl;
cout << "System type of matrix element ( wtf !): " << frame.type()  <<endl;
cout << "Analusi " << frame.size()  <<endl;


//http://docs.opencv.org/doc/user_guide/ug_mat.html
Vec3b intensity = frame.at<Vec3b>(50,50 );
uchar blue = intensity.val[0];
uchar green = intensity.val[1];
uchar red = intensity.val[2];

// Matlab like
//http://answers.opencv.org/question/10336/combining-multiple-cvmat-images-into-single-cvmat/

cout << "R= " <<red << "G= "<<green << "B= " <<blue <<endl; 

// Open the window
//cv::namedWindow("foo");

// Display the image m in this window

flip( frame, image, 0 );
//frameCopy = frame(Rect(20,20,100,100)); // kwvei tetragwnakia 
cv::vconcat(frame,image,frameCopy);///cv::hconcat(frame,image,frameCopy);

cv::imshow("foo", frameCopy);
//cvShowImage( "result", iplImg );


            if( waitKey( 25 ) >= 0 )
                cvReleaseCapture( &capture );
        



}
    //    waitKey(0);

   // cvDestroyWindow("result");

    return 0;
    }
}
