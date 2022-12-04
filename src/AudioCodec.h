#include <stdio.h>
#include <sndfile.h>
#include <vector>
#include <iostream>
#include <math.h>
#include "wav_quant.h"
#include <sndfile.hh>
#include <map>
#include <chrono>
#include "Golomb.h"

// using namespace std;
constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames
class AudioCodec {
    private:
        std::string fileName;
        unsigned char mode;
        int m;

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
        // std::vector<short> readAudioFile(){
        //     std::cout << "Audio file name: " << fileName << std::endl;

        //     SndfileHandle sndFile { "sample004.wav" };
        //     if (sndFile.error()) {
        //         std::cout << "Error: " << sndFile.strError() << std::endl;
        //         exit(EXIT_FAILURE);
        //     }
        //     size_t nFrames;
	    //     std::vector<short> samples(sndFile.frames() * sndFile.channels());

        //     nFrames = sndFile.readf(samples.data(), sndFile.frames());
        //     samples.resize(nFrames * sndFile.channels());
                
            
        //     return samples;
        // }

      
       
        bool compress () {

            std::cout << "Audio file name: " << fileName <<std::endl;

            SndfileHandle sndFile { fileName };
            if (sndFile.error()) {
                std::cout << "Error: " << sndFile.strError() << std::endl;
                exit(EXIT_FAILURE);
            }

            size_t nFrames;
	        std::vector<short> samples(sndFile.frames() * sndFile.channels());

            nFrames = sndFile.readf(samples.data(), sndFile.frames());
            samples.resize(nFrames * sndFile.channels());

            //if lossy compression with wave_quant



            int m_frequency = 4096;

            Golomb g;

            bitstream bit_stream("fileCompressed.bin", "w");

            // encoding header
            std::cout<<"Encoding header"<<std::endl;
            // cout<<"m: "<<m<<endl;
            std::string encoded_m = g.encodeInteger(this->m, 100);
            int enc_m[encoded_m.length()];
            for(int i = 0; i < encoded_m.length(); i++) {
                enc_m[i] = int(encoded_m[i]) - '0';
            }
            // cout << "encoded m: " << encoded_m << endl;
            //quantize m
            //code here
            bit_stream.writeNBits(enc_m, encoded_m.length());

            // cout<<"channels: "<<sndFile.channels()<<endl;
            std::string encoded_channels = g.encodeInteger(sndFile.channels(), 100);
            int enc_channels[encoded_channels.length()];
            for(int i = 0; i < encoded_channels.length(); i++) {
                enc_channels[i] = int(encoded_channels[i]) - '0';
            }
            // cout << "encoded channels: " << encoded_channels << endl;
            bit_stream.writeNBits(enc_channels, encoded_channels.length());


            //endcode sample rate
            std::string encoded_rate = g.encodeInteger(sndFile.samplerate(), 100);
            // cout<<"rate: "<<sndFile.samplerate()<<endl;
            // string encoded_rate = g.encodeInteger(sndFile.channels(), 100);
            int enc_rate[encoded_rate.length()];
            for(int i = 0; i < encoded_rate.length(); i++) {
                enc_rate[i] = int(encoded_rate[i]) - '0';
            }
            // cout << "encoded rate: " << encoded_rate << endl;
            bit_stream.writeNBits(enc_rate, encoded_rate.length());

            std::string encoded_nframe = g.encodeInteger(sndFile.frames(), 100);
            // cout<<"rate: "<<sndFile.samplerate()<<endl;
            // string encoded_rate = g.encodeInteger(sndFile.channels(), 100);
            // cout<<"nframe: "<<sndFile.frames()<<endl;
            int nframes[encoded_nframe.length()];
            for(int i = 0; i < encoded_nframe.length(); i++) {
                nframes[i] = int(encoded_nframe[i]) - '0';
            }
            // cout << "encoded rate: " << encoded_rate << endl;
            bit_stream.writeNBits(nframes, encoded_nframe.length());

            // encoding data

            std::vector<short> left;
            std::vector<short> right;
            for(int i = 0; i < samples.size(); i++) {
                if(i % 2 == 0) {
                    left.push_back(samples[i]);
                } else {
                    right.push_back(samples[i]);
                }
            }
            std::cout <<"want lossy compression? (y/n)" << std::endl;
            //quantize left channel
            char lossy;
            std::cin >> lossy;
            if(lossy=='y'){
                std::cout << "Lossy compression" << std::endl;
               std:: cout<<"how many bits do you want to quantize?"<<std::endl;
                int bits;
                std::cin >> bits;

                WAVQuant quant(left.size(), bits);
                quant.quantization(left);
                left=quant.getQuantizedVector();
                quant.quantization(right);
                right=quant.getQuantizedVector();
            }

            std::vector<short> l;
            std::vector<short> r;

            std::vector<short> idealm_l;
            std::vector<short> idealm_r;

            std::string encoded_value;

            int sum = 0;
            int cnt = 0;
            for(int i = 0; i < left.size(); i++) {
                

                if(i % m_frequency == 0 && i != 0) {

                    double avg = sum / cnt;
                    // cout << "sum: " << sum << endl;
                    // cout << "cnt: " << cnt << endl;
                    // cout << "avg: " << avg << endl;
                    // cout<<"i:"<<i<<endl;
                    this->m = getIdealM(avg);
                    sum = 0;
                    cnt = 0;
                    // cout << "new m: " << this->m << endl;
                }

                if(i < 3) {
                    encoded_value = g.encodeInteger(left[i], m);
                    l.push_back(left[i]);
                    sum += abs(left[i]);
                    encoded_value += g.encodeInteger(right[i], m);
                    r.push_back(right[i]);
                    sum += abs(right[i]);
                    // cout<<"left: "<<left[i]<<endl;
                    // cout<<"right: "<<right[i]<<endl;
                } else {


                    short pl = predictor(left[i-3], left[i-2], left[i-1]);
                    short pr = predictor(right[i-3], right[i-2], right[i-1]);
                    encoded_value = g.encodeInteger(left[i] - pl, m);
                    l.push_back(left[i] - pl);
                    sum += abs(left[i] - pl);
                    encoded_value += g.encodeInteger(right[i] - pr, m);
                    r.push_back(right[i] - pr);
                    idealm_r.push_back(right[i] - pr);
                    sum += abs(right[i] - pr);
                }

                cnt++;

                int encoded[encoded_value.size()];
                for(int j = 0; j < encoded_value.size(); j++) {
                    encoded[j] = encoded_value[j] - '0';
                }
                bit_stream.writeNBits(encoded, encoded_value.size());

            }

            bit_stream.close();


            // cout<<"left  smaples: "<<endl;
            // for(int i = 0; i < l.size(); i++) {
            //     cout<<left[i]<<" ";
            // }

            // cout<<endl;

            // cout<<"right  smaples: "<<endl;

            // for(int i = 0; i < r.size(); i++) {
            //     cout<<right[i]<<" ";
            // }

            // cout<<endl;





            




            std::cout << "Compressing finished" << std::endl;
            return 0;
        }

        
        bool decompress () {
            //ask for output file name
            std::string output_file;
            std::cout << "Enter output file name: " << std::endl;
            std::cin >> output_file;

            std::cout << "Decompressing..." << std::endl;
            //golomb
            Golomb golomb;
            
            int res;
            int m_frequency = 4096;
            int avg = 0;
            int count = 0;
            //channels var 
            int channels;
            //sample rate var
            int sample_rate;
            int nframes;
            
            // int m = 4;
            golomb.createPossibleBinaryTable(m);

            // reading bits from encoded file
            bitstream bit_stream(fileName, "r");

            // read file and store it in a string variable
            // ofstream ofs("decode_file");
            std::string bits_string = "";
            int bit;
            while((bit = bit_stream.readBit()) != EOF) {
                // ofs << bit << endl;
                bits_string += std::to_string(bit);
            }
            //pointer to the bits_string
            char* ptr =&bits_string[0];




            //reading channels

            // cout<<"bits string: "<<bits_string<<endl;
            // cout << "Calculated" << endl;

            int idx = 0;                            // keeps track of where we are on the string
            bool residual_bits_flag = false;
            

            int num_decoded_values = 0;

            std::vector<short> l;//store decoded values for left channel
            std::vector<short> r;//store decoded valeus for right channel
           
            
            //audio file
            SndfileHandle sndFile2 ;

            



            //while pointer is not at the end of the string

            // std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            while(*ptr !='\0' && !residual_bits_flag) {
                
                // cout<<"bits_string.length()-1:"<<bits_string.length()-1<<endl;
                // cout<<"m"<<m<<endl;
                // // cout<<"flag:"<<residual_bits_flag<<endl;

                if(count % m_frequency==0 && count != 0) {
                    // cout<<"num_decoded_values:"<<num_decoded_values<<endl;
                    // cout<<"avg: "<<avg/(count)<<endl;
                    this->m = getIdealM(avg/count);
                    golomb.createPossibleBinaryTable(this->m);
                    avg=0;
                    count=0;
                    // cout<<"new m: "<<this->m<<endl;

                }
                if(bits_string.length() - idx < 8) {
                    int cntr_residual = 0;
                    for(int i = idx; i < bits_string.length(); i++) {
                        if(ptr[i] == '1')
                            cntr_residual++;
                    }
                    if(cntr_residual == 0)
                        residual_bits_flag = true;
                }

                if(!residual_bits_flag) {
                    // read unary string
                    char value;
                    std::string unary = "";
                    while((value = ptr[idx]) != '0') {
                        unary += value;
                        idx++;
                    }

                    //separator bit '0'
                    idx++;

                    // read the binary part
                    std::map<int, std::string> table = golomb.getTable();    // get table of possible binary values
                    std::string binary = "";
                    
                    // while value not in table keep reading
                    bool not_in_table = true;
                    while(not_in_table) {
                        binary += ptr[idx++];
                        for(auto &v : table) {
                            if(v.second == binary) {
                                not_in_table = false;
                            }
                        }
                    }

                    // while value still in table
                    bool in_table = true;
                    while(in_table) {
                        std::string tmp = binary + ptr[idx];
                        int counter = 0;                            // increments when value is on table
                        for(auto &v : table) {
                            if(v.second == tmp) {
                                counter++;
                            }
                        }
                        if(counter == 0) {                          // if not in table
                            in_table = false;
                        } else {
                            binary += ptr[idx++];
                        }
                    }

                    // signal bit
                    char signal_bit = ptr[idx++];
                    
                    //if are the first 3 values
                    if(num_decoded_values < 4) {  //header
                        if (num_decoded_values == 0) {
                            //decode but use pointer to get the value
                            res = golomb.decodeInteger(unary + "0" + binary + signal_bit, 100);
                            this->m = res;
                            // cout<<"m:"<<unary + "0" + binary + signal_bit<<endl;
                            // cout<<"decoded m"<<m<<endl;
                        } else if (num_decoded_values == 1) {
                            res = golomb.decodeInteger(unary + "0" + binary + signal_bit, 100);
                            // cout<<"channels"<<unary + "0" + binary + signal_bit<<endl;
                            channels = res;
                            // cout<<"decoded channels"<<endl;
                        } else if (num_decoded_values == 2) {
                            res = golomb.decodeInteger(unary + "0" + binary + signal_bit, 100);
                            sample_rate = res;
                            // cout<<"sample rate"<<unary + "0" + binary + signal_bit<<endl;
                            // cout<<"decoded sample rate"<<endl;
                            //create audio file
                            sndFile2 = SndfileHandle(output_file+".wav", SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16 , channels, sample_rate);
                        } else if (num_decoded_values == 3) {
                            res = golomb.decodeInteger(unary + "0" + binary + signal_bit, 100);
                            nframes = res;
                            std::cout<<"Decoded header"<<std::endl;   
                            
                        }
                        // cout << "Decoded value: " << res << endl;
                        
                    } else if (num_decoded_values <10 ) {
                        res = golomb.decodeInteger(unary + "0" + binary + signal_bit, this->m);
                        if(num_decoded_values % 2 == 0) {
                            l.push_back(res); //first 3 values
                            // cout<<"l:"<<unary + "0" + binary + signal_bit<<endl;
                            // cout<<"l:"<<l[l.size()-1]<<endl;
                        } else {
                            // cout<<"r:"<<unary + "0" + binary + signal_bit<<endl;
                            r.push_back(res); //first 3 values
                            count++;
                            avg=avg+abs(res)+abs(l[l.size()-1]);
                            // cout<<"r:"<<r[r.size()-1]<<endl;
                        }
                    
                    }
                    else {
                        res = golomb.decodeInteger(unary + "0" + binary + signal_bit, this->m);
                        int L;
                        int R;
                        if(num_decoded_values % 2 == 0) {
                            // cout<<"size of l:"<<l.size()<<endl;
                            // l.push_back(res);
                            // cout<<"decoded value"<<endl;
                            L=res;
                            l.push_back(res + predictor(l[l.size()-3],l[l.size()-2],l[l.size()-1]));
                        } else {
                            r.push_back(res + predictor(r[r.size()-3],r[r.size()-2],r[r.size()-1]));
                            // cout<<"size of r:"<<r.size()<<endl; 
                            // r.push_back(res);
                            R=res;
                            count++;
                            avg=avg+abs(R)+abs(L);
                            // cout<<"decoded value"<<endl;
                        }
                      
                    }
                   
                    // cout<<"debug"<<endl;

                    
                    // cout << "Decoded value: " << res << endl;
                }
                // cout<<"debug2"<<endl;
                
                num_decoded_values++;
                
                // cout<<"idx:"<<idx<<endl;
                //print l and r

                               
            }
            //end time
            // std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;
            // std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

            // cout<<"debug3"<<endl;
            // cout<<"start writing"<<endl;
            // //put samples in audio file
            // cout<<"l:"<<endl;
            // for(int i=0;i<l.size();i++){
            //     cout<<l[i]<<endl;
            // }

            
            // cout<<"r:"<<endl;
            // for(int i=0;i<r.size();i++){
            //     cout<<r[i]<<endl;
            // }

            std::vector<short> samples;
            
            for(int i = 0; i < r.size(); i++) {
                samples.push_back(l[i]);
                samples.push_back(r[i]);
            }

            //print samples
            // cout<<"samples:"<<endl;
            // for(int i = 0; i < samples.size(); i++) {
            //     cout<<samples[i]<<endl;
            // }
            
            // cout<<"size of samples:"<<samples.size()<<endl;
            // cout<<"nframes:"<<nframes<<endl;
            int x= sndFile2.writef(samples.data(), nframes);
            // cout<<"x:"<<x<<endl;
            //read audio file

            SndfileHandle sndFile3("out2.wav");
            std::vector<short> samples2;
            samples2.resize(sndFile3.frames() * sndFile3.channels());
            sndFile3.readf(samples2.data(), sndFile3.frames());
            // cout<<"samples2:"<<endl;
            // for(int i = 0; i < samples2.size(); i++) {
            //     cout<<samples2[i]<<endl;
            // }
            // return 0;
            std::cout<<"Decompression done"<<std::endl;
            return 0;
           

            // cout << "Finished decoding" << endl;
            // return true;
        }


            
        
        short predictor(short sample1, short sample2, short sample3) {
            return 3*sample3 - 3*sample2 + sample1;
        }

        int getIdealM(double avg){
            double alfa = avg / (avg + 1);
            if (ceil(-1/log2(alfa))!=0) {
                return ceil(-1/log2(alfa));
            }
            else {
                return 2;
            }
            
        }

};
