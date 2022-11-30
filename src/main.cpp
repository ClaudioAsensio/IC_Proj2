#include "AudioCodec.h"

int main(int argc, char **argv) {

    // compress audio file
    AudioCodec codec("sample01.wav","e",500);
    codec.compress();
    // AudioCodec codec2("fileCompressed.bin","d",10);
    // codec2.decompress();
    return 0;

}
