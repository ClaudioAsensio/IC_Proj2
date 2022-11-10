#include <iostream>
#include "Golomb.h"


using namespace std;

int main(int argc, char const *argv[])
{
    // Golomb g;
    // g.encodeInteger(11, 3);

    // cout << g.decodeInteger("00111", 4) << endl;

    // handling input arguments
    if(argc < 3) {
        cout << "[USAGE]: " << argv[1] << " [-mode VALUE] optional( [-m M] )" << endl;
        return -1;
    }

    Golomb g;

    if(strcmp(argv[1], "-e") == 0) {    // calling Integer encoder
        int m = 4;

        if(argc == 5) {
            m = stoi(argv[4]);
        }

        string encoded_value = g.encodeInteger(stoi(argv[2]), m);
        cout << encoded_value << endl;

    } else if(strcmp(argv[1], "-d") == 0) {    // calling Integer encoder
        int m = 4;

        if(argc == 5) {
            m = stoi(argv[4]);
        }

        int decoded_value = g.decodeInteger(argv[2], m);
        cout << decoded_value << endl;

    }

    // } else if(strcmp(argv[2], "-d") == 0) {    // calling Integer encoder
    //     int m = 4;
    //     if(argc == 5) {
    //         m = stoi(argv[5]);
    //     }
    //     int decoded_value = g.decodeInteger(argv[3], m);

    // }

    

    return 0;
}
