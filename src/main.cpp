#include "AudioCodec.h"

int main(int argc, char **argv) {

    // compress audio file
    cout<<"Audio Compression"<<endl;
    //ask for input file
    string input_file;
    cout<<"Enter input file name: ";
    cin>>input_file;

    AudioCodec codec(input_file + ".wav","e",100);
    codec.compress();
    AudioCodec codec2("fileCompressed.bin","d",100);
    codec2.decompress();
    return 0;

}
