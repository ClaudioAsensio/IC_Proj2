#include <iostream>
#include <opencv2/opencv.hpp>
#include "Golomb.h"
#include "bitstream.h"
#include <fstream>
#include <map>
#include <iterator>
#include <vector>

using namespace std;
using namespace cv;

void encodeImage(Mat image);

void decodeImage(string image_name);


class ImageCodec {
    private:
        int image_cols = 722;
        int image_rows = 471;

    public:
        void encodeImage(Mat image) {
            image_cols = image.cols;
            image_rows = image.rows;

            Golomb golomb;

            int m = 4;
            golomb.createPossibleBinaryTable(m);

            bitstream bit_stream("output_file", "w");
            // iterate over the image lines
            for(int row = 0; row < image.rows; row++) {
                // iterate over the image columns
                for(int col = 0; col < image.cols; col++) {
                    // apply Golomb code to each pixel value;
                    string encoded_value = golomb.encodeInteger(image.at<Vec3b>(row, col)[0], m);       // using m = 4
                    encoded_value += golomb.encodeInteger(image.at<Vec3b>(row, col)[1], m);
                    encoded_value += golomb.encodeInteger(image.at<Vec3b>(row, col)[2], m);

                    int encoded[encoded_value.size()];
                    for(int i = 0; i < encoded_value.size(); i++) {
                        encoded[i] = int(encoded_value[i])-'0';
                    }
                    bit_stream.writeNBits(encoded, encoded_value.size());
                }
            }

            bit_stream.close();


        }

        void decodeImage(string filename, int rows, int cols) {
            Golomb golomb;
            golomb.createPossibleBinaryTable(4);

            // reading bits from encoded file
            bitstream bit_stream(filename, "r");

            // read file and store it in a string variable
            // ofstream ofs("decode_file");
            string bits_string = "";
            int bit;
            while((bit = bit_stream.readBit()) != EOF) {
                // ofs << bit << endl;
                bits_string += to_string(bit);
            }
            cout << "Calculated" << endl;

            // group encoded data into vector
            vector<int> stored_values;
            int idx = 0;                            // keeps track of where we are on the string
            while(idx < bits_string.length()-1) {
                // // read signal bit
                // char signal_bit = bits_string[idx++];
                cout << idx << endl;
                cout << bits_string.length() << endl;
                // read unary string
                char value;
                string unary = "";
                while((value = bits_string[idx]) != '0') {
                    unary += value;
                    idx++;
                }

                // ingnore the separator bit '0'
                idx++;

                // read the binary part
                string binary = "";
                map<int, string> table = golomb.getTable();    // get table of possible binary values
                // while value not in table keep reading
                bool not_in_table = true;
                while(not_in_table) {
                    binary += bits_string[idx++];
                    for(auto &v : table) {
                        if(v.second == binary) {
                            not_in_table = false;
                        }
                    }
                }

                // while value still in table
                bool in_table = true;
                while(in_table) {
                    string tmp = binary + bits_string[idx];
                    int counter = 0;                            // increments when value is on table
                    for(auto &v : table) {
                        if(v.second == tmp) {
                            counter++;
                        }
                    }
                    if(counter == 0) {                          // if not in table
                        in_table = false;
                    } else {
                        binary += bits_string[idx++];
                    }
                }

                // convert binary value to decimal
                int r = 0;
                for(int i = binary.length()-1; i >= 0; i--) {
                    r += (int(binary[i])-'0')*std::pow(2,i);
                }

                // calculate q
                int q = unary.length();

                int mapped_value = q*4+r;
                int real_value;
                if(mapped_value % 2 == 0) {
                    real_value = -mapped_value/2;
                } else {
                    real_value = (mapped_value-1)/2;
                }

                stored_values.push_back(real_value);
                
                
            }

            int rgb_idx = 0;
            vector<uchar> stored_values_0;
            vector<uchar> stored_values_1;
            vector<uchar> stored_values_2;
            for(int i = 0; i < stored_values.size(); i++) {
                if(rgb_idx == 3) {
                    rgb_idx = 0;
                }
                if(rgb_idx == 0) {
                    stored_values_0.push_back(u_char(stored_values[i]));
                } else if(rgb_idx == 1) {
                    stored_values_1.push_back(u_char(stored_values[i]));
                } else if(rgb_idx == 2) {
                    stored_values_2.push_back(u_char(stored_values[i]));
                }
                rgb_idx++;
            }
            
            // create new image
            Mat new_image(rows, cols, CV_8UC3);
            
            int k = 0;
            for(int row = 0; row < rows; row++) {
                for(int col = 0; col < cols; col++) {
                    new_image.at<Vec3b>(row, col)[0] = stored_values_0[k];
                    new_image.at<Vec3b>(row, col)[1] = stored_values_1[k];
                    new_image.at<Vec3b>(row, col)[2] = stored_values_2[k];
                    k++;
                }
            }

        
            cout << "Writing image" << endl;
            imwrite("new_image.ppm", new_image);

            cout << "Finished decoding" << endl;
        }




};
