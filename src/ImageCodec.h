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

            int m_frequency = 30;

            // vector<int>b;
            // vector<int>g;
            // vector<int>r;
            // for(int row = 0; row < image.rows; row++) {
            //     for(int col = 0; col < image.cols; col++) {
            //         if(row == 0 || col == 0) {
            //             b.push_back(abs(image.at<Vec3b>(row, col)[0]));
            //             g.push_back(abs(image.at<Vec3b>(row, col)[1]));
            //             r.push_back(abs(image.at<Vec3b>(row, col)[2]));
            //         } else {
            //             b.push_back(abs(image.at<Vec3b>(row, col)[0] - predictorJPEG( image.at<Vec3b>(row, col - 1)[0], image.at<Vec3b>(row - 1, col)[0], image.at<Vec3b>(row - 1, col - 1)[0])));
            //             g.push_back(abs(image.at<Vec3b>(row, col)[1] - predictorJPEG( image.at<Vec3b>(row, col - 1)[1], image.at<Vec3b>(row - 1, col)[1], image.at<Vec3b>(row - 1, col - 1)[1])));
            //             r.push_back(abs(image.at<Vec3b>(row, col)[2] - predictorJPEG( image.at<Vec3b>(row, col - 1)[2], image.at<Vec3b>(row - 1, col)[2], image.at<Vec3b>(row - 1, col - 1)[2])));
            //         }
            //     }
            // }

            // int m = getIdealM(b, g, r);
            int m = 4;

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
            
            int encoded_values = 0;
            vector<int>b;
            vector<int>g;
            vector<int>r;
            // iterate over the image lines
            for(int row = 0; row < image.rows; row++) {
                // iterate over the image columns
                for(int col = 0; col < image.cols; col++) {
                    // calculate new optimal m parameter
                    
                    if(encoded_values % m_frequency == 0 && encoded_values != 0) {
                        m = getIdealM(b, g, r);

                        while(!b.empty()) {
                            b.pop_back();
                        }
                        while(!g.empty()) {
                            g.pop_back();
                        }
                        while(!r.empty()) {
                            r.pop_back();
                        }
                    }

                    string encoded_value;
                    if(row == 0 || col == 0) {
                        // apply Golomb code to each pixel value;
                        encoded_value = golomb.encodeInteger(image.at<Vec3b>(row, col)[0], m);
                        encoded_value += golomb.encodeInteger(image.at<Vec3b>(row, col)[1], m);
                        encoded_value += golomb.encodeInteger(image.at<Vec3b>(row, col)[2], m);
                        b.push_back(abs(image.at<Vec3b>(row, col)[0]));
                        g.push_back(abs(image.at<Vec3b>(row, col)[1]));
                        r.push_back(abs(image.at<Vec3b>(row, col)[2]));
                    } else {
                        // apply Golomb code to the prediction error
                        int p0 = predictorJPEG( image.at<Vec3b>(row, col - 1)[0], image.at<Vec3b>(row - 1, col)[0], image.at<Vec3b>(row - 1, col - 1)[0]);
                        int p1 = predictorJPEG( image.at<Vec3b>(row, col - 1)[1], image.at<Vec3b>(row - 1, col)[1], image.at<Vec3b>(row - 1, col - 1)[1]);
                        int p2 = predictorJPEG( image.at<Vec3b>(row, col - 1)[2], image.at<Vec3b>(row - 1, col)[2], image.at<Vec3b>(row - 1, col - 1)[2]);
                        encoded_value = golomb.encodeInteger(image.at<Vec3b>(row, col)[0] - p0, m);
                        encoded_value += golomb.encodeInteger(image.at<Vec3b>(row, col)[1] - p1, m);
                        encoded_value += golomb.encodeInteger(image.at<Vec3b>(row, col)[2] - p2, m);

                        b.push_back(abs(image.at<Vec3b>(row, col)[0] - p0));
                        g.push_back(abs(image.at<Vec3b>(row, col)[1] - p1));
                        r.push_back(abs(image.at<Vec3b>(row, col)[2] - p2));

                    }

                    int encoded[encoded_value.size()];
                    for(int i = 0; i < encoded_value.size(); i++) {
                        encoded[i] = int(encoded_value[i])-'0';
                    }
                    bit_stream.writeNBits(encoded, encoded_value.size());

                    encoded_values++;
                }
            }
            bit_stream.close();


        }


        void decodeImage(string filename) {
            Golomb golomb;

            int m_frequency = 30;
            
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

            int idx = 0;                            // keeps track of where we are on the string
            bool residual_bits_flag = false;
            
            int rows;
            int cols;

            int num_decoded_values = 0;

            int current_row = 0;
            int current_col = 0;
            
            Mat new_image;

            vector<int> b;
            vector<int> g;
            vector<int> r;

            int num_bgr_decoded = 0;

            while(idx < bits_string.length()-1 && !residual_bits_flag) {
                // calculate new optimal m parameter
                if(num_bgr_decoded == m_frequency) {
                    m = getIdealM(b, g, r);
                    golomb.createPossibleBinaryTable(m);

                    while(!b.empty()) {
                        b.pop_back();
                    }
                    while(!g.empty()) {
                        g.pop_back();
                    }
                    while(!r.empty()) {
                        r.pop_back();
                    }
                    num_bgr_decoded = 0;
                }

                // detect if the missing bits are the ones used to fill the last byte when writing in the binary file                
                if(bits_string.length() - idx < 8) {
                    int cntr_residual = 0;
                    for(int i = idx; i < bits_string.length(); i++) {
                        if(bits_string[i] == '1')
                            cntr_residual++;
                    }
                    if(cntr_residual == 0)
                        residual_bits_flag = true;

                }

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

                    // signal bit
                    char signal_bit = bits_string[idx++];

                    int res = golomb.decodeInteger(unary + "0" + binary + signal_bit, m);

                    if(num_decoded_values == 0) {
                        m = res;
                        golomb.createPossibleBinaryTable(m);
                    } else if(num_decoded_values == 1) {
                        rows = res;
                    } else if(num_decoded_values == 2) {
                        cols = res;
                        new_image.create(rows, cols, CV_8UC3);
                    } else {

                        // se estiver na primeira linha ou primeira coluna
                        if(current_row == 0 || current_col == 0) {
                            if(num_decoded_values % 3 == 0) {
                                b.push_back((abs(res)));
                                new_image.at<Vec3b>(current_row, current_col)[0] = u_char(res);
                            } else if(num_decoded_values % 3 == 1) {
                                g.push_back((abs(res)));
                                new_image.at<Vec3b>(current_row, current_col)[1] = u_char(res);
                            } else if(num_decoded_values % 3 == 2) {
                                r.push_back((abs(res)));
                                new_image.at<Vec3b>(current_row, current_col)[2] = u_char(res);
                                
                                num_bgr_decoded++;

                                current_col++;
                                if(current_col == cols) {
                                    current_row++;
                                    current_col = 0;
                                }
                            }
                        } else {
                            if(num_decoded_values % 3 == 0) {
                                b.push_back((abs(res)));
                                new_image.at<Vec3b>(current_row, current_col)[0] = u_char(res + predictorJPEG(new_image.at<Vec3b>(current_row, current_col-1)[0], new_image.at<Vec3b>(current_row-1, current_col)[0], new_image.at<Vec3b>(current_row-1, current_col-1)[0]));
                            } else if(num_decoded_values % 3 == 1) {
                                g.push_back((abs(res )));
                                new_image.at<Vec3b>(current_row, current_col)[1] = u_char(res + predictorJPEG(new_image.at<Vec3b>(current_row, current_col-1)[1], new_image.at<Vec3b>(current_row-1, current_col)[1], new_image.at<Vec3b>(current_row-1, current_col-1)[1]));
                            } else if(num_decoded_values % 3 == 2) {
                                r.push_back((abs(res )));
                                new_image.at<Vec3b>(current_row, current_col)[2] = u_char(res + predictorJPEG(new_image.at<Vec3b>(current_row, current_col-1)[2], new_image.at<Vec3b>(current_row-1, current_col)[2], new_image.at<Vec3b>(current_row-1, current_col-1)[2]));
                                
                                num_bgr_decoded++;

                                current_col++;
                                if(current_col == cols) {
                                    current_row++;
                                    current_col = 0;
                                }
                            }

                        }

                    }
                    num_decoded_values++;
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

            int res = ceil(-1/log2(average / (average+1)));
            if(res == 0) res = 1;

            return res;
        }

};
