#include "ImageCodec.h"
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char const *argv[])
{

    ImageCodec codec;

    if(strcmp(argv[1], "-e") == 0) {
        cout << "Starting to encode..." << endl;
        Mat image = imread(argv[2], IMREAD_COLOR);

        // check if image file is empty
        if(image.empty()) {
            cout << "Could not open or find the image" << endl;
            exit(-1);
        }
        codec.encodeImage(image);
        cout << image.rows << endl;
        cout << image.cols << endl;
    } else if (strcmp(argv[1], "-d") == 0) {
        cout << "Starting to decode..." << endl;
        string filename = argv[2];
        Mat image = imread(argv[3], IMREAD_COLOR);
        codec.decodeImage(filename);
    } else {
        cout << "Invalid option." << endl;
        return -1;
    }

    return 0;
}