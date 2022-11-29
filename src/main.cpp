#include "AudioCodec.h"

int main(int argc, char **argv) {

    // compress audio file
    AudioCodec codec("sample01.wav","e",10);
    codec.compress();
    AudioCodec codec2("fileCompressed","d",15);
    codec2.decompress();
    return 0;

}
