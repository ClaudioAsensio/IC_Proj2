#include <cmath>
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <stdio.h>
#include <map>
#include <iterator>
#include <memory>
#include "bitstream.h"


class Golomb {

    private:
        char buffer;
        size_t buffer_size;
        std::fstream file;
        std::map<int, std::string> table;
        int m;
        int b;
        bitstream Gfile; 

    public:

        Golomb(){};

        Golomb(std::string fileName, std::string mode, int mIn) {
            if (mode != "d" && mode != "e") {
                std::cout << "ERROR: invalid mode!" << std::endl;
                exit(EXIT_FAILURE);
            }
            if(mode == "d")
                Gfile = bitstream(fileName, "r");
            else
                Gfile = bitstream(fileName, "w");

            m = mIn;
            b = ceil(log2(m));

        }

        //encoding an integer into string of bits
        std::string encodeInteger(int n, int m) {
            // int n;
            // if(x >= 0) {
            //     n = 2*x+1;
            // } else {
            //     n = 2*(-x);
            // }

            std::string signal_bit = "0";
            if(n < 0) {
                signal_bit = "1";
                n = -n;
            }

            // std::cout << "Value of n: " << n << std::endl;

            int q = std::floor(n/m);
            int r = n % m;

            // createPossibleBinaryTable(m);

            // binary part
            std::string rem = "";
            // m is a power of 2
            if((m != 0) && ((m & (m - 1)) == 0)) {
                // parse r to binary
                // std::cout << "r: " << r << std::endl;
                int aux = r;
                if(aux == 0) {
                    rem += "0";
                } else {
                    while(aux != 0) {
                        rem += (aux % 2 == 0 ? '0' : '1');
                        aux /= 2;
                    }
                }
                // add '0' when rem length is less than log2(m)
                while(rem.length() < std::log2(m)) {
                    rem += "0";
                }

            } else {    // if m is not a power of 2
                int b = std::ceil(std::log2(m));
                int word_length;
                int value;  // value to be encoded on binary part

                // Encode the first 2^b − m values of r using the first 2^b − m binary codewords of b − 1 bits.
                if(r < pow(2, b) - m) {
                    word_length = b-1;
                    value = r;
                } else {    // Encode the remainder values of r by coding the number r + 2^b − m in binary codewords of b bits.
                    word_length = b;
                    value = r + pow(2, b) - m;
                    // std::cout << "value: " << value << std::endl;
                }

                while(value != 0) {
                    rem += (value % 2 == 0 ? '0' : '1');
                    value /= 2;
                    word_length--;
                }

                while(word_length != 0) {
                    rem += '0';
                    word_length--;
                }


            }

            std::reverse(rem.begin(), rem.end());
            // std::cout << "[Binary]: " << rem << std::endl;

            // unary part
            // m is a power of 2
            std::string quo = "";
            for(int i = 0; i < q; i++)
                quo += "1";
            quo += "0";
            // std::cout << "[Unary]: " << quo << std::endl;
            // std::cout << "[Signal Bit]: " << signal_bit << std::endl;

            return quo + rem + signal_bit;

        }

        // void Golomb::encode(int* value) {

        //     int q, r;

        //     unsigned int absolute_value = abs(*value);

        //     q = floor(absolute_value / m); // quociente -> unario
        //     r = absolute_value % m;  // resto -> binario

        //     // write sign bit (if value is 0, no sign bit is used)
        //     if(*value < 0)
        //         Gfile.writeBit(1);
        //     else if(*value > 0)
        //         Gfile.writeBit(0);
        // }

        int decodeInteger(std::string codeword, int m) {
            int b = std::ceil(std::log2(m));
            std::string unary_string = "";
            int j = 0;

            // unary part
            // read bits until first '0'
            while(true) {
                if(codeword[j++] == '0')
                    break;
                unary_string += "1";                
            }

            // std::cout << "Unary: " << unary_string << std::endl;

            int q = unary_string.length();

           
            // binary part
            int remaining_codeword_bits = codeword.length() - q - 2;
            // std::cout << "Remaining bits: " << remaining_codeword_bits << std::endl;

            // read the binary part
            createPossibleBinaryTable(m);
            std::map<int, std::string> table = getTable();    // get table of possible binary values
            
            std::string binary = "";
            for(int i = 0; i < remaining_codeword_bits; i++) {
                binary += codeword[j++];
            }

            // std::cout << "Binary: " << binary << std::endl;
            
            int res;
            // if m is power of two
            if((m != 0) && ((m & (m - 1)) == 0)) {
                // convert binary to decimal
                int decimal_value = 0;
                for(int i = 0; i < binary.length(); i++) {
                    if(binary[i] == '1')
                        decimal_value += pow(2, binary.length()-i-1);
                }

                res = q*m + decimal_value;
            } else {
                int threshold = std::pow(2, b) - m;
                int decimal_value = 0;
                for(int i = remaining_codeword_bits-1; i >= 0; i--) {
                    decimal_value += (int(binary[remaining_codeword_bits-1-i])-'0')*std::pow(2, i);
                }
                int r;
                if(decimal_value >= threshold) {
                    r = decimal_value - threshold;
                } else {
                    r = decimal_value;
                }
                res = q*m + r;
            }

            if(codeword[codeword.length()-1] == '1') {
                res = -res;
            }

            return res;
        }

        void createPossibleBinaryTable(int m) {
            std::map<int, std::string> auxTable;
            // if m is power of 2
            if((m != 0) && ((m & (m - 1)) == 0)) {
                for(int i = 0; i <= m-1; i++) {
                    std::string bin = ""; 
                    int aux = i;
                    if(aux == 0) {
                        bin += "0";
                    } else {
                        while(aux != 0) {
                            bin += (aux % 2 == 0 ? '0' : '1');
                            aux /= 2;
                        }
                    }
                    if(bin.length() < std::ceil(std::log2(m))) {
                        int diff = std::ceil(std::log2(m)) - bin.length();
                        for(int j = 0; j < diff; j++) {
                            bin += "0";
                        }
                    }
                    std::reverse(bin.begin(), bin.end());
                    auxTable.insert(std::pair<int, std::string>(i, bin));
                }
            } else {
                for(int i = 0; i <= m-1; i++) {
                    std::string bin = "";
                    int b = std::ceil(std::log2(m));
                    int word_length;
                    int value;  // value to be encoded on binary part

                    // Encode the first 2^b − m values of r using the first 2^b − m binary codewords of b − 1 bits.
                    if(i < pow(2, b) - m) {
                        word_length = b-1;
                        value = i;
                    } else {    // Encode the remainder values of r by coding the number r + 2^b − m in binary codewords of b bits.
                        word_length = b;
                        value = i + pow(2, b) - m;
                    }

                    while(value != 0) {
                        bin += (value % 2 == 0 ? '0' : '1');
                        value /= 2;
                        word_length--;
                    }

                    while(word_length != 0) {
                        bin += '0';
                        word_length--;
                    }
                    std::reverse(bin.begin(), bin.end());
                    auxTable.insert(std::pair<int, std::string>(i, bin));
                }
            }
            setTable(auxTable);
        }

        std::map<int, std::string> getTable() {
            return table;
        }


        void setTable(std::map<int, std::string> t) {
            table = std::move(t);
            // std::map<int, std::string>::iterator itr;
            // std::cout << "---------" << std::endl;
            // for(itr = table.begin(); itr != table.end(); ++itr) {
            //     std::cout << '\t' << itr->first << '\t' << itr-> second << '\n';
            // }
            // std::cout << "---------" << std::endl;
        }

};