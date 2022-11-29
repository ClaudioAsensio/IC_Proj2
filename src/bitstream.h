#ifndef bitstream_H
#define bitstream_H

//class to read an write bits from a file
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fstream>
#include <iostream>

class bitstream{

private:

    std::fstream file;
    char  buffer;
    size_t buffer_size;
    std::string mode;

public:
    //mode = "r" for read, "w" for write

    bitstream(){};
    //constructor
    bitstream(std::string filename, std::string mode){
            mode = mode;
            if (mode == "r"){
                file.open(filename, std::ios::in | std::ios::binary);
                std::cout << "Bin file opened in read" << std::endl;
            }
            else if (mode == "w"){
                file.open(filename, std::ios::out | std::ios::binary);
                std::cout << "Bin file opened in write" << std::endl;
            }
            if (!file.is_open()){
                std::cerr << "Error opening file " << filename << std::endl;
                exit(1);
            }
            buffer_size = 0;
            buffer = 0;
            std::cout << "Bin file opened successfully" << std::endl;
        }

   int readBit(){
            if (file.eof()){
                return EOF;
            } else {
                if (buffer_size == 0){
                    file.read(&buffer, 1);
                    buffer_size = 8;
                }   
            }

            if (file.eof()){
                std::cout << "end of file" << std::endl;
                return EOF;
            }
            
            // int bit = (buffer >>7)& 0x01;
            int bit = (buffer & 0x80)>>7;
            // std::cout << "bit:" << bit << std::endl;
            buffer = buffer << 1;
            buffer_size--;
            return bit;
    }
    // write bit
    void writeBit(int bit){
        if (bit != 0 && bit != 1){
            std::cerr << "Error: invalid bit" << std::endl;
            exit(1);
        }
        if (buffer_size!=0){
            buffer = buffer << 1;
        }
        buffer = buffer | (bit );
        buffer_size++;
        if (buffer_size == 8){
            file.write(&buffer,1);
            buffer_size = 0;
            buffer = 0;
        }
    }
    // read N bits
    void readNBits(int n,int *val){
        for (int i = 0; i < n; i++){
            int bit = readBit();
            if (bit == EOF){
                break;
            }
            val[i] = bit;
        }
        
    }
    // writeNBits
    void writeNBits(int* val,int n){
        for (int i = 0; i <n ; i++){
            int bit = val[i];
            // printf("bit:%d\n",bit);
            writeBit(bit);
        }
    }

    unsigned char size() {
        return buffer_size;
    }
    
    
    //close file
    void close(){
        //if buffer is not empty, write buffer to file
       if (buffer_size != 0){
            buffer = buffer << (8 - buffer_size); //fill the byte to write
            file.write(&buffer, 1);
            buffer_size = 0;
            buffer = 0;
        }

        file.close();
    
    }
};


#endif