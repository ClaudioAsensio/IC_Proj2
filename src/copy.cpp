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

    // string op=argv[3];
    // cout << "Operation: " << op << endl;
    //m->invert image horizontally
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
            // image.at<Vec3b>(i, j)[1] = tmp.at<Vec3b>(i, j)[1];
            // image.at<Vec3b>(i, j)[2] = tmp.at<Vec3b>(i, j)[2];
        }
    }
    imwrite(destination, image);

   
    



    return 0;
}
