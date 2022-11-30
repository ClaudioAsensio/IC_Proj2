#include <stdio.h>
#include <sndfile.h>
#include <vector>
#include <iostream>
#include <math.h>
#include "AudioFile.h"
#include <sndfile.hh>
#include <map>

#include "Golomb.h"

using namespace std;
constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames
class AudioCodec {
    private:
        string fileName;
        unsigned char mode;
        int m;
        Golomb* g;

    public:
        AudioCodec(){}

        AudioCodec(std::string fileName, std::string mode, int m){
            
            this->fileName = fileName;
            this->m = m;
            if (mode == "e"){
                this-> mode = 1;
            } else {
                this-> mode = 0;
            }
        }

        //read audio file and return a vector of samples

        vector<short> readAudioFile(){
            cout << "Audio file name: " << fileName << endl;

            SndfileHandle sndFile { "sample01.wav" };
            if (sndFile.error()) {
                cout << "Error: " << sndFile.strError() << endl;
                exit(EXIT_FAILURE);
            }
            size_t nFrames;
	        vector<short> samples(sndFile.frames() * sndFile.channels());

            nFrames = sndFile.readf(samples.data(), sndFile.frames());
            samples.resize(nFrames * sndFile.channels());
                
            
            return samples;
        }

        //predictor function receives a vector of samples and returns a vector 


        vector<short >predictor(){
            vector<short> samples = readAudioFile();
            vector<short> predicted;
            int predictedSample;
            //formula u = 3*sample[2] - 3*sample[1] + sample[0]
            //real = samples[i]
            predicted.push_back(samples[0]);
            predicted.push_back(samples[1]);
            predicted.push_back(samples[2]);
            //for to iterate over the vector of samples
                for (int i = 0; i < samples.size(); i++){
                    int u=3*samples[i-1]-3*samples[i-2]+samples[i-3];
                    predictedSample = u;
                    predicted.push_back(predictedSample);
                }
            return predicted;

        }
       
        bool compress () {
            vector<short> samples = readAudioFile();

            cout << "start encoding..." << endl;

            if(!mode) return 1;
            AudioFile<float> input_audio(fileName);
            int channels = input_audio.getNumChannels();
            int samples_per_channel = input_audio.getNumSamplesPerChannel();
            int sample_rate = input_audio.getSampleRate();
            // Golomb
            // m = 0;
            g = new Golomb("fileCompressed.bin", "e",m);
            bitstream b = bitstream("fileCompressed.bin", "w");

            g -> encodeInteger(m,m);
            g -> encodeInteger(channels,m);
            g -> encodeInteger(samples_per_channel,m);
            g -> encodeInteger(sample_rate,m);
            //encode prediction array
            vector<short> prediction = predictor();
            cout<<"prediction size: "<<prediction.size()<<endl;
            string encoded_data;
            for (int i = 0; i < prediction.size(); i++){
                
                encoded_data = (g->encodeInteger(samples[i]-prediction[i],m));
                cout<<"encoded data size: "<<encoded_data.size()<<endl;
                int encoded_d[encoded_data.size()];
                cout<<"debug"<<endl;
                for (int i = 0; i < encoded_data.size(); i++)
                {
                    encoded_d[i]=int(encoded_data[i])-'0';
                }
                
                b.writeNBits(encoded_d,encoded_data.size());
            }
            cout << "Compressing finished" << endl;
            return 0;
        }

    
        // decompress the given audio file
        bool decompress()
        {

            Golomb golomb;

            int m_frequency = 50;
            
            int m = 4;
            golomb.createPossibleBinaryTable(m);

            // reading bits from encoded file
            bitstream bit_stream(fileName, "r");

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
            
         

            int num_decoded_values = 0;

         
            
  

            int num_bgr_decoded = 0;


            while(idx < bits_string.length()-1 && !residual_bits_flag) {
                // calculate new optimal m parameter
                // if(num_bgr_decoded == m_frequency) {
                //     golomb.createPossibleBinaryTable(m);

                //     while(!b.empty()) {
                //         b.pop_back();
                //     }
                //     while(!g.empty()) {
                //         g.pop_back();
                //     }
                //     while(!r.empty()) {
                //         r.pop_back();
                //     }
                //     num_bgr_decoded = 0;
                // }

                if(bits_string.length() - idx < 8) {
                    int cntr_residual = 0;
                    for(int i = idx; i < bits_string.length(); i++) {
                        if(bits_string[i] == '1')
                            cntr_residual++;
                    }
                    if(cntr_residual > 0)
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

            // cout << "Decompressing...." << endl;
            // if(mode) return 1;
            // AudioFile<float> output_audio;
            // bool error = 0;
            // int value = 0;
            // g = new Golomb(fileName, "d",m);
            // // decode golomb parameter
            // error = g->decodeInteger("",m);
            // int decoded_m = value;
            // if(m != decoded_m)
            //     return 1;
            // // decode file metadata
            // error = g->decodeInteger("",value);
            // int channels = value;
            // error = g->decodeInteger("",value);
            // int samples_per_channel = value;
            // error = g->decodeInteger("",value);
            // int sample_rate = value;

            // //decode fileCompressed.bin
            // vector<short> prediction = predictor();
            // vector<short> samples;
            // for (int i = 0; i < prediction.size(); i++){
            //     error = g->decodeInteger(str(prediction[i]),m);
            //     samples.push_back(value+prediction[i]);
            // }
            // //ler ficheiro to do


            // if(error)
            //     return 1;

            // //output file using sndfile library
            // SndfileHandle sndFile { "fileDecompressed.wav", SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, channels, sample_rate };
            // if (sndFile.error()) {
            //     cout << "Error: " << sndFile.strError() << endl;
            //     exit(EXIT_FAILURE);
            // }
            // sndFile.write(samples.data(), samples.size());
            // cout << "Decompressing finished" << endl;



            // // output_audio.
            // // output_audio.setNumChannels(channels);
            // // output_audio.setNumSamplesPerChannel(samples_per_channel);
            // // output_audio.setSampleRate(sample_rate);
            // // output_audio.save("output.wav");
            // // cout << "Decompressing finished with no errors" << endl;
            // return 0;
        }

};
            // // keeping track of previous sample values for predictors
            // int last_samples[3] = {0, 0, 0};
            // // decode audio samples 
            // // assume wav audio with 2 channels and 16 bits data
            // for(int i = 0; i < samples_per_channel; i++)
            // {
            //     // decode next residual
            //     error = g->decodeInteger("",value);
            //     if(error)
            //         continue;
            //     int residual = value;
            //     int estimate = 0;
            //     int merged_sample = 0;
            //     if(i == 0)
            //     {
            //         // first order predictor
            //         estimate = 0;
            //         merged_sample = residual + estimate;
            //         last_samples[0] = merged_sample;
            //     }
            //     else if(i == 1)
            //     {
            //         // second order predictor
            //         estimate = last_samples[0];
            //         merged_sample = residual + estimate;
            //         last_samples[1] = merged_sample;
            //     }
            //     else if(i == 2)
            //     {
            //         // third order predictor
            //         estimate = 2 * last_samples[1] - last_samples[0];
            //         merged_sample = residual + estimate;
            //         last_samples[2] = merged_sample;
            //     }
            //     else
            //     {
            //         // fourth order predictor
            //         estimate = 3 * last_samples[2] - 3 * last_samples[1] + last_samples[0];
            //         merged_sample = residual + estimate;
            //         last_samples[0] = last_samples[1];
            //         last_samples[1] = last_samples[2];
            //         last_samples[2] = merged_sample;
            //     }
            //     // channel redundancy
            //     output_audio.samples[0][i] = merged_sample / 255.0f;
            //     output_audio.samples[1][i] = merged_sample / 255.0f;
            // }