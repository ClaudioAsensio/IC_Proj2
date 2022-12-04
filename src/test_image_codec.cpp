#include "ImageCodec.h"
#include <iostream>

using namespace std;
using namespace cv;
using namespace std::chrono;

int main(int argc, char const *argv[])
{

    ImageCodec codec;
    auto start = high_resolution_clock::now();
    if(strcmp(argv[1], "-e") == 0) {
        cout << "Starting to encode..." << endl;
        Mat image = imread(argv[2], IMREAD_COLOR);

        // check if image file is empty
        if(image.empty()) {
            cout << "Could not open or find the image" << endl;
            exit(-1);
        }
        string output_file = argv[3];
        codec.encodeImage(image, output_file);
        
        // get argv[2] file size in bits
        ifstream file(argv[2], ios::binary | ios::ate);
        streamsize size = file.tellg();
        file.close();
        cout << "Original file size: " << size << " bytes" << endl;

        // get output_file file size in bits
        ifstream file2(output_file, ios::binary | ios::ate);
        streamsize size2 = file2.tellg();
        file2.close();
        cout << "Encoded file size: " << size2 << " bytes" << endl;

        cout << "Space Saving: " << 1-((float)size2/size) << endl;

        cout << "Compression Ratio: " << (float)size/size2 << endl;


    } else if (strcmp(argv[1], "-d") == 0) {
        cout << "Starting to decode..." << endl;
        string filename = argv[2];
        string output_file = argv[3];
        codec.decodeImage(filename, output_file);
        ifstream file3(filename, ios::binary | ios::ate);
        streamsize size3 = file3.tellg();
        file3.close();
        cout << "Decoded file size: " << size3 << " bytes" << endl;
    } else {
        cout << "Invalid option." << endl;
        return -1;
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: " << duration.count() << " microseconds" << endl;

    return 0;
}