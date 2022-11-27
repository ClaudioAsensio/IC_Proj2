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

    public:

        void encodeImage(Mat image) {

            Golomb golomb;

            // int m = 4;
            int sum = 0;
            int cntr = 0;
            vector<int>b;
            vector<int>g;
            vector<int>r;
            for(int row = 0; row < image.rows; row++) {
                for(int col = 0; col < image.cols; col++) {
                    b.push_back(image.at<Vec3b>(row, col)[0]);
                    g.push_back(image.at<Vec3b>(row, col)[1]);
                    r.push_back(image.at<Vec3b>(row, col)[2]);
                    cntr++;
                }
            }

            int m = getIdealM(b, g, r);

            golomb.createPossibleBinaryTable(m);

            bitstream bit_stream("output_file", "w");
            // encode m
            string encoded_m = golomb.encodeInteger(m, 4);
            int enc_m[encoded_m.length()];
            for(int i = 0; i < encoded_m.length(); i++) {
                enc_m[i] = int(encoded_m[i]) - '0';
            }
            bit_stream.writeNBits(enc_m, encoded_m.length());
            // encode the image rows and columns
            string encoded_rows = golomb.encodeInteger(image.rows, m);
            string encoded_cols = golomb.encodeInteger(image.cols, m);
            int enc_rows[encoded_rows.size()];
            int enc_cols[encoded_cols.size()];
            for(int i = 0; i < encoded_rows.size(); i++) {
                enc_rows[i] = int(encoded_rows[i])-'0';
            }
            for(int i = 0; i < encoded_cols.size(); i++) {
                enc_cols[i] = int(encoded_cols[i])-'0';
            }
            bit_stream.writeNBits(enc_rows, encoded_rows.size());
            bit_stream.writeNBits(enc_cols, encoded_cols.size());

            // iterate over the image lines
            for(int row = 0; row < image.rows; row++) {
                // iterate over the image columns
                for(int col = 0; col < image.cols; col++) {
                    string encoded_value;
                    if(row == 0 || col < 1) {
                        // apply Golomb code to each pixel value;
                        encoded_value = golomb.encodeInteger(image.at<Vec3b>(row, col)[0], m);       // using m = 4
                        encoded_value += golomb.encodeInteger(image.at<Vec3b>(row, col)[1], m);
                        encoded_value += golomb.encodeInteger(image.at<Vec3b>(row, col)[2], m);
                    } else {
                        // apply Golomb code to the prediction error
                        int p0 = predictorJPEG( image.at<Vec3b>(row, col - 1)[0], image.at<Vec3b>(row - 1, col)[0], image.at<Vec3b>(row - 1, col - 1)[0]);
                        int p1 = predictorJPEG( image.at<Vec3b>(row, col - 1)[1], image.at<Vec3b>(row - 1, col)[1], image.at<Vec3b>(row - 1, col - 1)[1]);
                        int p2 = predictorJPEG( image.at<Vec3b>(row, col - 1)[2], image.at<Vec3b>(row - 1, col)[2], image.at<Vec3b>(row - 1, col - 1)[2]);
                        encoded_value = golomb.encodeInteger(image.at<Vec3b>(row, col)[0] - p0, m);       // using m = 4
                        encoded_value += golomb.encodeInteger(image.at<Vec3b>(row, col)[1] - p1, m);
                        encoded_value += golomb.encodeInteger(image.at<Vec3b>(row, col)[2] - p2, m);
                    }

                    int encoded[encoded_value.size()];
                    for(int i = 0; i < encoded_value.size(); i++) {
                        encoded[i] = int(encoded_value[i])-'0';
                    }
                    bit_stream.writeNBits(encoded, encoded_value.size());
                }
            }
            bit_stream.close();

            cout << "M: " << m << endl;
        }


        void decodeImage(string filename) {
            Golomb golomb;
            
            int m = 4;
            golomb.createPossibleBinaryTable(m);

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
            bool residual_bits_flag = false;
            
            int rows;
            int cols;

            int current_row = -1; 
            int current_col = 0;
            int num_decoded_values = 0;
            while(idx < bits_string.length()-1 && !residual_bits_flag) {
                if(idx % rows == 0) {
                    current_row++;
                }
                current_col = idx % rows;

                std::cout << "bits_string length: " << bits_string.length() << std::endl;
                if(bits_string.length() - idx < 20) {
                    std::cout << "----missing bits----" << std::endl;
                    for(int i = idx; i < bits_string.length(); i++) {
                        std::cout << bits_string[i];
                    }
                    std::cout << "--------" << std::endl;
                    if(bits_string[idx] == '0' && bits_string[idx+1] == '0' && bits_string[idx+2] == '0') {
                        std::cout << "bits a mais -> parar loop" << std::endl;
                        residual_bits_flag = true;
                    }
                }


                std::cout << "idx: " << idx << std::endl;
                if(!residual_bits_flag) {
                    // read unary string
                    char value;
                    string unary = "";
                    while((value = bits_string[idx]) != '0') {
                        unary += value;
                        idx++;
                    }

                    //separator bit '0'
                    idx++;

                    // read the binary part
                    map<int, string> table = golomb.getTable();    // get table of possible binary values
                    string binary = "";
                    
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

                    std::cout << "--------------" << std::endl;

                    // signal bit
                    char signal_bit = bits_string[idx++];
                    std::cout << "Signal bit: " << signal_bit << std::endl;

                    std::cout << "res: " << unary + "0" + binary + signal_bit << std::endl;
                    int res = golomb.decodeInteger(unary + "0" + binary + signal_bit, m);

                    if(num_decoded_values == 0) {
                        m = res;
                    } else if(num_decoded_values == 1) {
                        rows = res;
                    } else if(num_decoded_values == 2) {
                        cols = res;
                    } else {
                        stored_values.push_back(res);
                        std::cout << "Value: " << res << std::endl;
                    }
                    std::cout << "--------------" << std::endl;
                    num_decoded_values++;
                }

                cout << "M: " << m << endl;
                               
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
                    if(row == 0 || col == 0) {
                        new_image.at<Vec3b>(row, col)[0] = stored_values_0[k];
                        new_image.at<Vec3b>(row, col)[1] = stored_values_1[k];
                        new_image.at<Vec3b>(row, col)[2] = stored_values_2[k];
                    } else {
                        int p0 = predictorJPEG(new_image.at<Vec3b>(row, col-1)[0], new_image.at<Vec3b>(row-1, col)[0], new_image.at<Vec3b>(row-1, col-1)[0]);
                        int p1 = predictorJPEG(new_image.at<Vec3b>(row, col-1)[1], new_image.at<Vec3b>(row-1, col)[1], new_image.at<Vec3b>(row-1, col-1)[1]);
                        int p2 = predictorJPEG(new_image.at<Vec3b>(row, col-1)[2], new_image.at<Vec3b>(row-1, col)[2], new_image.at<Vec3b>(row-1, col-1)[2]);
                        new_image.at<Vec3b>(row, col)[0] = p0 + stored_values_0[k];
                        new_image.at<Vec3b>(row, col)[1] = p1 + stored_values_1[k];
                        new_image.at<Vec3b>(row, col)[2] = p2 + stored_values_2[k];
                    }
                    k++;
                }
            }

        
            cout << "Writing image" << endl;
            imwrite("new_image.ppm", new_image);

            cout << "Finished decoding" << endl;
        }


        int predictorJPEG(int a, int b, int c) {
            if (c >= max(a, b))
                return min(a, b);
            else if (c <= min(a, b))
                return max(a, b);
            else
                return a + b - c;
        }

        int getIdealM(vector<int>b, vector<int>g, vector<int>r) {
            map<int, int> aux;
            double average = 0;
            for(auto i : b) aux[i]++;
            for(auto i : g) aux[i]++;
            for(auto i : r) aux[i]++;
            int samples = 0;
            for(auto i : aux)
                samples += i.second;

            for(auto i : aux) {
                average += i.first * ((double)i.second / samples);
            }

            return ceil(-1/log2(average / (average+1)));
        }

};
