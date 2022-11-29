#include <stdio.h>
#include <sndfile.h>
#include <vector>
#include <iostream>
#include <math.h>
#include "AudioFile.h"
#include <map>

#include "Golomb.h"

using namespace std;

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

       
        bool compress () {
            
            cout << "start encoding..." << endl;

            if(!mode) return 1;
            AudioFile<float> input_audio(fileName);
            int channels = input_audio.getNumChannels();
            int samples_per_channel = input_audio.getNumSamplesPerChannel();
            int sample_rate = input_audio.getSampleRate();
            // Golomb
            // m = 0;
            g = new Golomb("fileCompressed.bin", "e",m);

            g -> encodeInteger(0,m);
            g -> encodeInteger(0,channels);
            g -> encodeInteger(0,samples_per_channel);
            g -> encodeInteger(0,sample_rate);

            // keeping track of previous sample values for predictors
            int last_samples[3] = {0, 0, 0};
            // encode audio samples
            // assume wav audio with 2 channels and 16 bits data
            for(int i = 0; i < samples_per_channel; i++)
            {
                // golomb only works with integers
                // convert float to int (8 bits)
                int left = input_audio.samples[0][i] * 255;
                int right = input_audio.samples[1][i] * 255;
                // channel redundancy
                int merged_sample = floor((left + right) / 2.0);
                // apply polynomial predictors (temporal redundancy)
                int estimate = 0;
                int residual = 0;
                if(i == 0)
                {
                    // first order predictor
                    estimate = 0;
                    last_samples[0] = merged_sample;
                }
                else if(i == 1)
                {
                    // second order predictor
                    estimate = last_samples[0];
                    last_samples[1] = merged_sample;
                }
                else if(i == 2)
                {
                    // third order predictor
                    estimate = 2 * last_samples[1] - last_samples[0];
                    last_samples[2] = merged_sample;
                }
                else
                {
                    // fourth order predictor
                    estimate = 3 * last_samples[2] - 3 * last_samples[1] + last_samples[0];
                    last_samples[0] = last_samples[1];
                    last_samples[1] = last_samples[2];
                    last_samples[2] = merged_sample;
                }

                residual = merged_sample - estimate;
                g -> encodeInteger(residual,0);
            }

            g -> close();
            cout << "Compressing finished" << endl;
            return 0;
        }

    
        // decompress the given audio file
        bool decompress()
        {
            cout << "Decompressing...." << endl;
            if(mode) return 1;
            AudioFile<float> output_audio;
            bool error = 0;
            int value = 0;
             g = new Golomb(fileName, "d",m);
            // decode golomb parameter
            error = g->decodeInteger("",m);
            int decoded_m = value;
            if(m != decoded_m)
                return 1;
            // decode file metadata
            error = g->decodeInteger("",value);
            int channels = value;
            error = g->decodeInteger("",value);
            int samples_per_channel = value;
            error = g->decodeInteger("",value);
            int sample_rate = value;
            if(error)
                return 1;
            output_audio.setNumChannels(channels);
            output_audio.setNumSamplesPerChannel(samples_per_channel);
            output_audio.setSampleRate(sample_rate);
            // keeping track of previous sample values for predictors
            int last_samples[3] = {0, 0, 0};
            // decode audio samples 
            // assume wav audio with 2 channels and 16 bits data
            for(int i = 0; i < samples_per_channel; i++)
            {
                // decode next residual
                error = g->decodeInteger("",value);
                if(error)
                    continue;
                int residual = value;
                int estimate = 0;
                int merged_sample = 0;
                if(i == 0)
                {
                    // first order predictor
                    estimate = 0;
                    merged_sample = residual + estimate;
                    last_samples[0] = merged_sample;
                }
                else if(i == 1)
                {
                    // second order predictor
                    estimate = last_samples[0];
                    merged_sample = residual + estimate;
                    last_samples[1] = merged_sample;
                }
                else if(i == 2)
                {
                    // third order predictor
                    estimate = 2 * last_samples[1] - last_samples[0];
                    merged_sample = residual + estimate;
                    last_samples[2] = merged_sample;
                }
                else
                {
                    // fourth order predictor
                    estimate = 3 * last_samples[2] - 3 * last_samples[1] + last_samples[0];
                    merged_sample = residual + estimate;
                    last_samples[0] = last_samples[1];
                    last_samples[1] = last_samples[2];
                    last_samples[2] = merged_sample;
                }
                // channel redundancy
                output_audio.samples[0][i] = merged_sample / 255.0f;
                output_audio.samples[1][i] = merged_sample / 255.0f;
            }
            output_audio.save("output.wav");
            cout << "Decompressing finished with no errors" << endl;
            return 0;
        }

};