#include "AudioCodec.h"
#include<chrono>

using namespace std;

int main(int argc, char **argv) {

   //if no args are passed, print error message

    
    
    //if arg is d decompress
    if(strcmp(argv[1], "-d") == 0) {
        chrono::steady_clock::time_point begin2 = chrono::steady_clock::now();
        AudioCodec codec2("fileCompressed.bin","d",100);
        codec2.decompress();
        chrono::steady_clock::time_point end2 = chrono::steady_clock::now();
        cout << "Time to Decompress = " << chrono::duration_cast<chrono::seconds>(end2 - begin2).count() << "[s]" << endl;
        cout << "Time to Decompress = " << chrono::duration_cast<chrono::nanoseconds> (end2 - begin2).count() << "[ns]" << endl;
    } else if (strcmp(argv[1], "-c") == 0) {
        //if arg is c compress
        string input_file;
        cout<<"Enter sound file name: ";
        cin>>input_file;
        AudioCodec codec(input_file+".wav","e",100);        
        chrono::steady_clock::time_point begin = chrono::steady_clock::now();
        codec.compress();
        chrono::steady_clock::time_point end = chrono::steady_clock::now();
        cout << "Time to Compress = " << chrono::duration_cast<chrono::seconds>(end - begin).count() << "[s]" << endl;
        cout << "Time to Compress = " << chrono::duration_cast<chrono::nanoseconds> (end - begin).count() << "[ns]" << endl;
    }else{
        cout<<"Invalid operation"<<endl;
        exit(-1);
    }

    
   
   

    return 0;

}
