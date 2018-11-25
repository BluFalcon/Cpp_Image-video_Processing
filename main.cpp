//g++ -Wall openCV_example.cpp -o camcpp `pkg-config --cflags --libs opencv` -L/usr/local/cuda-6.5/lib64


#include "FlyCapture2.h"



#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#include <iostream>
#include <stdio.h>
#include <math.h>

#include <QString>
#include <QDebug>

#include <string>
#include <iomanip>   // gia to setprecision



#include <QElapsedTimer>


using namespace std;
using namespace cv;
using namespace FlyCapture2;


const char* my_win_name = "My la_capture";

int box_size ;//= 6; //na dinetai apo ton xristi k na einai global
int WEIGHT ,HEIGHT; //Kapou na ekxwrountai auta kai na einai tis original image

QElapsedTimer timer;

void PrintCameraInfo( CameraInfo* pCamInfo )
{
    cout << endl;
    cout << "*** CAMERA INFORMATION ***" << endl;
    cout << "Serial number -" << pCamInfo->serialNumber << endl;
    cout << "Camera model - " << pCamInfo->modelName << endl;
    cout << "Camera vendor - " << pCamInfo->vendorName << endl;
    cout << "Sensor - " << pCamInfo->sensorInfo << endl;
    cout << "Resolution - " << pCamInfo->sensorResolution << endl;
    cout << "Firmware version - " << pCamInfo->firmwareVersion << endl;
    cout << "Firmware build time - " << pCamInfo->firmwareBuildTime << endl << endl;


}

Mat convert_FL2_to_OpCV(Image rawImage)//,int k)
{


    // convert to rgb
    Image rgbImage;
    rawImage.Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage );

    // convert to OpenCV Mat
    unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize()/(double)rgbImage.GetRows();
    Mat temp = Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(),rowBytes);//.clone();

    cvtColor(temp,temp,CV_RGB2GRAY);

    return temp.clone();
    // cv::imshow("image", image);
    /*
if(k==1)
 {   imshow("image0", fun_image);
    waitKey(30);
}else
{    imshow("image1", fun_image);
    waitKey(30);
}
*/
    // return fun_image;
}

Mat la_focus_selction(Mat cam_1_rgb, Mat cam_2_rgb)
{
    Mat cam_output, cam_1_BW, cam_2_BW;
    Mat temp_box, temp_horiz;


    vector<Mat> orizontio;
    vector<Mat> ka8eto;

    cam_1_BW=cam_1_rgb;
    cam_2_BW=cam_2_rgb;

    //ta kanei aspromaura
//    cvtColor(cam_1_rgb,cam_1_BW,CV_RGB2GRAY);
//    cvtColor(cam_2_rgb,cam_2_BW,CV_RGB2GRAY);


    int i , j , box2 ;
    int xBegin,xEnd,yBegin,yEnd;
    float im_1_median,im_2_median,S_1,S_2;


    xBegin = 0;
    yBegin = 0;

    xEnd = box_size;
    yEnd = box_size;
    /*
    Vec3b intensity = cam_1_rgb.at<Vec3b>(50,50 );
    uchar blue = intensity.val[0];
    uchar green = intensity.val[1];
    uchar red = intensity.val[2];

    //http://answers.opencv.org/question/10336/combining-multiple-cvmat-images-into-single-cvmat/

    cout << "R= " << (int)red << "G= "<<(int)green << "B= " <<(int)blue <<endl;

    intensity = cam_2_rgb.at<Vec3b>(50,50 );
    blue = intensity.val[0];
    green = intensity.val[1];
    red = intensity.val[2];

    //http://answers.opencv.org/question/10336/combining-multiple-cvmat-images-into-single-cvmat/

    cout << "R= " << (int)red << "G= "<<(int)green << "B= " <<(int)blue <<endl;
*/

    do{

        //Auto ginetai gia na exoume x,y apo ta koutia
        if(xEnd>WEIGHT)
        {
            xEnd = WEIGHT ;

            //einai i 2i fora pou mpainei edw k i 1i exei epexegastei ta ...
            if(xBegin >= WEIGHT)
            {
                xBegin = 0;
                xEnd = box_size;

                yBegin += box_size;
                yEnd += box_size;

                if(yEnd>HEIGHT)
                {
                    yEnd=HEIGHT;

                    if(yBegin>=HEIGHT)
                    {
                        break;
                    }

                }//yEnd > Height

            }  //if xBeg>=weight

        } //if xEnd>weight

        im_1_median = 0;
        im_2_median = 0;

        /* Scalar intensity = im1.at<uchar>(10, 10);//y,x

    cout << intensity.val[0] << endl;*/


        Scalar intensity1 ,intensity2;

        intensity1 = mean(cam_1_BW(Rect(xBegin,yBegin,xEnd-xBegin,yEnd-yBegin)));
        intensity2 = mean(cam_2_BW(Rect(xBegin,yBegin,xEnd-xBegin,yEnd-yBegin)));

        im_1_median = intensity1.val[0];
        im_2_median = intensity2.val[0];

        S_1 = 0;
        S_2 = 0;


        //Euresi Mesis Timis gia ka8e box
        for(i= xBegin;i<xEnd;i++)
        {

            for(j= yBegin;j<yEnd;j++)
            {

                intensity1 = cam_1_BW.at<uchar>(j,i); //prosoxi einai Y , X
                S_1 += pow((float) intensity1.val[0]-im_1_median,2);

                intensity2 = cam_2_BW.at<uchar>(j,i); //prosoxi einai Y , X
                S_2 += pow((float) intensity2.val[0]-im_2_median,2);
            }
        }




        //Euresi S gia ka8e box. prosoxi "axristes praxeis" paralipontai


        //Mat cropedImage = fullImage(Rect(X,Y,Width,Height)); // kwvei tetragwnakia
        //cv::vconcat(frame,image,frameCopy);///cv::hconcat(src1,src2,dest);

        //dialexe to focused kommati

        //
        if(S_1>S_2)
        {
            orizontio.push_back(cam_1_rgb(Rect(xBegin,yBegin,xEnd-xBegin,yEnd-yBegin)));
        }
        else
        {
            orizontio.push_back(cam_2_rgb(Rect(xBegin,yBegin,xEnd-xBegin,yEnd-yBegin)));
        }

        if(xEnd == WEIGHT)//telos grammis
        {
            hconcat(orizontio,temp_box);
            ka8eto.push_back(temp_box);
            orizontio.clear();
            temp_box.release();
        }
        //         cout << "\n xBeg = " << xBegin << " xEnd= "  << xEnd << " yBeg = " << yBegin << " yEnd = " << yEnd << "\n";

        //Dwse vima
        xBegin += box_size;
        xEnd +=box_size;

    }while(true);

    //    imshow("image0",cam_output );


    vconcat(ka8eto,cam_output);
    return cam_output;

}



/*
http://stackoverflow.com/questions/1585535/convert-rgb-to-black-white-in-opencv
// C
IplImage *im_rgb  = cvLoadImage("image.jpg");
IplImage *im_gray = cvCreateImage(cvGetSize(im_rgb),IPL_DEPTH_8U,1);
cvCvtColor(im_rgb,im_gray,CV_RGB2GRAY);

// C++
Mat im_rgb  = imread("image.jpg");
Mat im_gray;
cvtColor(im_rgb,im_gray,CV_RGB2GRAY);
*/
void petaw2_capture()
{
    double fps_count_prev=0,fps_count=0 ;
    Mat fin_image;
    Error error;
    String text;


    int camera_code =0 ;//0 an i hicol einai assign sto 0 kai 1 an i hicol einai assign sto 1
    BusManager busMgr;
    unsigned int numCameras;
    error = busMgr.GetNumOfCameras(&numCameras);
    if (error != PGRERROR_OK)
    {
        error.PrintErrorTrace();
        return ;
    }

    cout << "Number of cameras detected: " << numCameras << endl;

    Camera** ppCameras = new Camera*[numCameras];

    // Connect to all detected cameras and attempt to set them to
    // a common video mode and frame rate
    for ( int i = 0; i < numCameras; i++)
    {
        ppCameras[i] = new Camera();

        PGRGuid guid;
        error = busMgr.GetCameraFromIndex( i, &guid );
        if (error != PGRERROR_OK)
        {
            error.PrintErrorTrace();
            return ;
        }

        // Connect to a camera
        error = ppCameras[i]->Connect( &guid );
        if (error != PGRERROR_OK)
        {
            error.PrintErrorTrace();
            return ;
        }

        // Get the camera information
        CameraInfo camInfo;
        error = ppCameras[i]->GetCameraInfo( &camInfo );
        if (error != PGRERROR_OK)
        {
            error.PrintErrorTrace();
            return ;
        }

        PrintCameraInfo(&camInfo);

        // Set all cameras to a specific mode and frame rate

        //to str2 einai i cur cam
        QString tempstr1= "Dragonfly2 DR2-13S2C";
        QString tempstr2 = camInfo.modelName;


        //  error = ppCameras[i]->SetVideoModeAndFrameRate(VIDEOMODE_640x480RGB,FRAMERATE_15 );

        if(! tempstr1.compare(tempstr2))
        {//ayti einai i DR2-13s2c
            //    error = ppCameras[i]->SetVideoModeAndFrameRate(VIDEOMODE_1280x960RGB,FRAMERATE_3_75 );
            error = ppCameras[i]->SetVideoModeAndFrameRate(VIDEOMODE_640x480RGB,FRAMERATE_15 );

            //    qDebug() << "mpika "  <<tempstr2;
        }
        else // ayti einai i HICOL
        {
            camera_code=i;
            //  error = ppCameras[i]->SetVideoModeAndFrameRate(VIDEOMODE_1024x768RGB,FRAMERATE_3_75 );
            error = ppCameras[i]->SetVideoModeAndFrameRate(VIDEOMODE_640x480RGB,FRAMERATE_15 ); //mporei kai 30 alla moni tis

            //    qDebug() << "mpika 1" << tempstr2;
        }

        if (error != PGRERROR_OK)
        {
            error.PrintErrorTrace();
            cout << "Error setting custom resolution or fps on camera. " << endl;
            cout << "Press Enter to exit. " << endl;

            cin.ignore();
            return ;
        }

        error = ppCameras[i]->StartCapture();
        if (error != PGRERROR_OK)
        {
            error.PrintErrorTrace();
            cout << "Error starting cameras. " << endl;
            cout << "Press Enter to exit. " << endl;

            cin.ignore();
            return ;
        }
    }



    timer.start();

    //An einai ola kala aoxoxooaxoaoxoaoxoaoxaoxoaoaoxoa

    Mat tempora1,tempora2;


    cvNamedWindow( "DR2-13s2c" , WINDOW_NORMAL );
    cvNamedWindow( "DR2-HICOL" , WINDOW_NORMAL );
    cvNamedWindow( "Merge" , WINDOW_NORMAL );



    int j = 0;
    Image image[2];

    while(true)
    {
        j++;
        for ( unsigned int i = 0; i < numCameras; i++ )
        {
            error = ppCameras[i]->RetrieveBuffer( &image[i] );
            if (error != PGRERROR_OK)
            {
                error.PrintErrorTrace();
                return ;
            }

            //    TimeStamp timestamp = image[i].GetTimeStamp();
            //     cout << "Cam " << i << " - Frame " << j << " - TimeStamp [" << timestamp.cycleSeconds << " " << timestamp.cycleCount << "]" << endl;

        }



        tempora1 = convert_FL2_to_OpCV(image[0]);
        tempora2 = convert_FL2_to_OpCV(image[1]);
        flip(tempora1,tempora1,1);


        //ayto prin to la_focus_selection
        HEIGHT = tempora1.size().height;
        WEIGHT = tempora1.size().width;


        int prH,prW;
        prH = tempora2.size().height;
        prW = tempora2.size().width;



        /* An i hicol einai assigned sto 0 auto simainei oti i tempora1 einai tis hicol kai tempora2 tis 13s2c
         * ara 8eloume na paroume to tempora2 pou deixnei pio megali perioxi k na to kopsoume kai na to kanoume risize
         * se 640*480 alliws 8eloume na to kanoume auto sto tempora1
         * */

        if(camera_code==0)
        {//ayti einai i DR2-13s2c

            tempora2= tempora2(Rect(100,80,WEIGHT-228,HEIGHT-176));
            prH = tempora2.size().height;
            prW = tempora2.size().width;

            //      qDebug() << "mpika 0" <<camera_code;
        }
        else
        {

            tempora1= tempora1(Rect(100,80,WEIGHT-228,HEIGHT-176));
            HEIGHT = tempora1.size().height;
            WEIGHT = tempora1.size().width;

            //       qDebug() << "mpika 1" <<camera_code;
        }


        //kwdikas gia max resolution alla low fps
        if(HEIGHT > prH)
        {
            //resize(InputArray src, OutputArray dst, Size dsize, double fx=0, double fy=0, int interpolation=INTER_LINEAR )

            resize(tempora1,tempora1,Size(),(double) prH/HEIGHT,(double) prW/WEIGHT,INTER_NEAREST);
            HEIGHT = tempora1.size().height;
            WEIGHT = tempora1.size().width;
        }
        else
        {
            resize(tempora2,tempora2,Size(),(double)WEIGHT/prW,(double)HEIGHT/prH,INTER_NEAREST);
        }

        /*
interpolation –
interpolation method:

INTER_NEAREST - a nearest-neighbor interpolation
INTER_LINEAR - a bilinear interpolation (used by default)
INTER_AREA - resampling using pixel area relation. It may be a preferred method for image decimation, as it gives moire’-free results. But when the image is zoomed, it is similar to the INTER_NEAREST method.
INTER_CUBIC - a bicubic interpolation over 4x4 pixel neighborhood
INTER_LANCZOS4 - a Lanczos interpolation over 8x8 pixel neighborhood

*/

        // cout << tempora1.size().height  << tempora2.size().height <endl;

        //  fin_image = la_focus_selction(convert_FL2_to_OpCV(image[0]) ,convert_FL2_to_OpCV(image[1]));

        fin_image = la_focus_selction(tempora1 ,tempora2);


        fps_count_prev = fps_count;
        fps_count= (double) timer.elapsed()/1000 ;

        stringstream ss;
        ss << setprecision(4)<<(1/(fps_count-fps_count_prev)) << " fps";

        text = ss.str();

        putText(fin_image, text,Point2f(20,20), 5, 1 , Scalar(0,0,255,255), 1, 8);
        imshow("Merge",fin_image );

        putText(tempora1, text, Point2f(20,20), 5, 1 , Scalar(0,0,255,255), 1, 8);
        putText(tempora2, text, Point2f(20,20), 5, 1 , Scalar(0,0,255,255), 1, 8);

        if(camera_code==0)
        {
            imshow("DR2-13s2c", tempora2);
            imshow("DR2-HICOL", tempora1);
        }
        else
        {
            imshow("DR2-13s2c", tempora1);
            imshow("DR2-HICOL", tempora2);

        }


        Mat grad_x, grad_y,grad_xy;
           Mat src_gray, abs_grad_y;

        int ksize=5;//it must be 1, 3, 5, or 7.
        int x=1,y=1;
            //void Sobel(src, dst, int ddepth, dx, dy,ksize=3, double scale=1, double delta=0, int borderType=BORDER_DEFAULT )
            /// Gradient X
            Sobel( tempora1, grad_x, CV_8U , x,y, ksize, 1, 0, BORDER_DEFAULT );
            Sobel( tempora2, grad_y, CV_8U , x, y, ksize, 1, 0, BORDER_DEFAULT );
            Sobel( fin_image, grad_xy, CV_8U, x, y, ksize, 1, 0, BORDER_REFLECT_101);


            imshow("X", grad_x);
            imshow("Y", grad_y);
            imshow("XY", grad_xy);


        if( waitKey( 25 ) >= 0 )
        {break;}

        cout << "Frame # " <<j<< " @ "<< (float) timer.elapsed()/1000 << " sec" <<endl;


    }//end while




}




//ME AND MY MAIN
int main( int argc, const char** argv )
{

    //   cvNamedWindow( "image0" , WINDOW_AUTOSIZE );

    // cvNamedWindow( "image1" , WINDOW_AUTOSIZE );

    /*  HEIGHT = 480;
    WEIGHT = 640;
*/

    QString pros;
    pros.append( argv[1]);

    box_size= argc >=2 ?  pros.toInt(0,10) : 10 ;

    petaw2_capture();

    /*

    timer.start();

    //    CvCapture* capture = 0;
    Mat image1, image2 , fin_im;


    cvNamedWindow( my_win_name , WINDOW_AUTOSIZE );


    image1 = imread("mwro1.jpg",1);


    image2 = imread("mwro2.jpg",1);

    //    imshow(my_win_name, image2);




    // imshow(my_win_name, image2);
    ///cout << "mege8os" << image1.size().height;



    HEIGHT = image1.size().height;
    WEIGHT = image1.size().width;

    if(HEIGHT != image2.size().height)
    {
        cout << "Oi eikones den exoun to idio upsos";
    }

    if(WEIGHT != image2.size().width)
    {
        cout << "Oi eikones den exoun to idio platos";
    }

    fin_im = la_focus_selction(image1 ,image2);

    imshow(my_win_name, fin_im);

    cout << "ti le re "<< timer.elapsed();

    waitKey( 0 );

*/
    /*
                    capture = cvCaptureFromCAM( 0 ); //0=default, -1=any camera, 1..99=your camera
                    if(!capture) cout << "No camera detected" << endl;

                    if( capture )
                    {
                        cout << "In capture ..." << endl;
                        for(;;)
                        {
                            IplImage* iplImg = cvQueryFrame( capture );
                            frame = iplImg;
                            if( frame.empty() )
                                break;


                            cv::imshow(my_win_name, frame);


                            if( waitKey( 25 ) >= 0 )
                            {
                                cout << "eimai edw" ;
                                cvReleaseCapture( &capture );
                                //   exit(0);
                            }
                        }
                    }

                */

    // code we need ?

    return 0;

}



//http://docs.opencv.org/modules/core/doc/basic_structures.html
/*
                cout << "O # twn kanaliwn: " << frame.channels()  <<endl;
                cout << "Total num of pixels: " << frame.total()  <<endl;
                cout << "System type of matrix element ( wtf !): " << frame.type()  <<endl;
                cout << "Analusi " << frame.size()  <<endl;


                //http://docs.opencv.org/doc/user_guide/ug_mat.html
                Vec3b intensity = frame.at<Vec3b>(50,50 );
                uchar blue = intensity.val[0];
                uchar green = intensity.val[1];
                uchar red = intensity.val[2];



                //http://answers.opencv.org/question/10336/combining-multiple-cvmat-images-into-single-cvmat/

                cout << "R= " << (int)red << "G= "<<green << "B= " <<blue <<endl;

                */

//frameCopy = frame(Rect(20,20,100,100)); // kwvei tetragwnakia
//     cv::vconcat(frame,image,frameCopy);///cv::hconcat(frame,image,frameCopy);


//cvtColor(frame,image,CV_RGB2GRAY);



//if(SetMouseCallback(my_win_name,,0))
//https://github.com/Itseez/opencv/blob/master/samples/cpp/ffilldemo.cpp


