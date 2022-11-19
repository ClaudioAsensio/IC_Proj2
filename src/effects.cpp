#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char const *argv[])
{
    // receive filenames from args
    string source = argv[1];
    string destination = argv[2];

    cout << "Source: " << source << endl;
    cout << "Destination: " << destination << endl;

    // read image pixel by pixel
    
    Mat image = imread(source, IMREAD_COLOR);
        // check if image is empty
    if (image.empty())
    {
    cout << "Could not open or find the image" << endl;
    return -1;
    }

    string op=argv[3];
    cout << "Operation: " << op << endl;
    //m->invert image vertically
    // R->rotate
    //N->negative
    //l->lighten or darken
    //c->copy

    Mat tmp(image.rows, image.cols, image.type());
    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            tmp.at<Vec3b>(i, j)[0]=image.at<Vec3b>(i, j)[0];
            tmp.at<Vec3b>(i, j)[1]=image.at<Vec3b>(i, j)[1];
            tmp.at<Vec3b>(i, j)[2]=image.at<Vec3b>(i, j)[2];
          
        }
    }
    imwrite(destination, image);

    if (op=="m")
    {
        //Mat tmp = image.clone();
        // Mat tmp(image.rows, image.cols, image.type());
        cout<<"Vertically->v or Horizontally->h"<<endl;
        string o;
        cin>> o;

        if (o=="v"){
            for (int i = 0; i < image.rows; i++)
            {
                for (int j = 0; j < image.cols; j++)
                 {

                image.at<Vec3b>(i, j)[0] = tmp.at<Vec3b>(i, image.cols - j - 1)[0];
                image.at<Vec3b>(i, j)[1] = tmp.at<Vec3b>(i, image.cols - j - 1)[1];
                image.at<Vec3b>(i, j)[2] = tmp.at<Vec3b>(i, image.cols - j - 1)[2];
            
                }
            }
        }else if (o=="h"){
            for (int i = 0; i < image.rows; i++)
            {
                for (int j = 0; j < image.cols; j++)
                 {

                image.at<Vec3b>(i, j)[0] = tmp.at<Vec3b>(image.rows - i - 1, j)[0];
                image.at<Vec3b>(i, j)[1] = tmp.at<Vec3b>(image.rows - i - 1, j)[1];
                image.at<Vec3b>(i, j)[2] = tmp.at<Vec3b>(image.rows - i - 1, j)[2];
            
                }
            }
        }
        
        
        imwrite(destination, image);
        
    }else if (op=="r")
    {
        cout<<"Enter angle,multiple of 90: ";
        int angle;
        cin>>angle;

        if (angle%90!=0)
        {
            cout<<"Invalid angle";
            return -1;
        }

        if (angle>360)
        {
            angle=angle%360;
            cout<<"Angle reduced to: "<<angle<<endl;
        }
        //rotate image angle degrees
        
        //rotate pixel by pixel
        Mat tmp2( image.cols, image.rows,image.type()); // tmp img to with new size (cols in the rows and vice versa)
        for (int i = 0; i < image.rows; i++)
        {
            for (int j = 0; j < image.cols; j++)
            {
                if(angle==90){
                    tmp2.at<Vec3b>(j, image.rows - i - 1)[0]=image.at<Vec3b>(i, j)[0];
                    tmp2.at<Vec3b>(j, image.rows - i - 1)[1]=image.at<Vec3b>(i, j)[1];
                    tmp2.at<Vec3b>(j, image.rows - i - 1)[2]=image.at<Vec3b>(i, j)[2];
                }else if(angle==180){
                    image.at<Vec3b>(i, j)[0] = tmp.at<Vec3b>(image.rows - i - 1, image.cols - j - 1)[0];
                    image.at<Vec3b>(i, j)[1] = tmp.at<Vec3b>(image.rows - i - 1, image.cols - j - 1)[1];
                    image.at<Vec3b>(i, j)[2] = tmp.at<Vec3b>(image.rows - i - 1, image.cols - j - 1)[2];
                }else if(angle==270){
                    tmp2.at<Vec3b>(image.cols - j - 1, i)[0]=image.at<Vec3b>(i, j)[0];
                    tmp2.at<Vec3b>(image.cols - j - 1, i)[1]=image.at<Vec3b>(i, j)[1];
                    tmp2.at<Vec3b>(image.cols - j - 1, i)[2]=image.at<Vec3b>(i, j)[2];
                }
            }
        }
        if (angle==90)
        {
            imwrite(destination, tmp2);
        }else if (angle==180)
        {
            imwrite(destination, image);
        }else if (angle==270)
        {
            imwrite(destination, tmp2);
        }

        
    }
    else if (op=="n")
    {

        
        //negative image
        for (int i = 0; i < image.rows; i++)
        {
            for (int j = 0; j < image.cols; j++)
            {
                image.at<Vec3b>(i, j)[0] = 255 - tmp.at<Vec3b>(i, j)[0];
                image.at<Vec3b>(i, j)[1] = 255 - tmp.at<Vec3b>(i, j)[1];
                image.at<Vec3b>(i, j)[2] = 255 - tmp.at<Vec3b>(i, j)[2];
            }
        }
        imwrite(destination, image);
    }
    else if (op=="l")
    {
        cout<<"Enter value to lighten or darken: ";
        int value;
        cin>>value;

        // Mat tmp(image.rows, image.cols, image.type());
        //lighten or darken image
        for (int i = 0; i < image.rows; i++)
        {
            for (int j = 0; j < image.cols; j++)
            {
                image.at<Vec3b>(i, j)[0] = tmp.at<Vec3b>(i, j)[0]+value;
                image.at<Vec3b>(i, j)[1] = tmp.at<Vec3b>(i, j)[1]+value;
                image.at<Vec3b>(i, j)[2] = tmp.at<Vec3b>(i, j)[2]+value;
            }
        }
        imwrite(destination, image);
    }else{
        cout<<"Invalid operation"<<endl;
    }


    return 0;
}
