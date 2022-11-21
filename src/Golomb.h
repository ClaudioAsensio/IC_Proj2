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


class Golomb {

    private:
        char buffer;
        size_t buffer_size;
        std::fstream file;
        std::map<int, std::string> table;

    public:

        //encoding an integer into string of bits
        std::string encodeInteger(int x, int m) {

            int n;
            if(x >= 0) {
                n = 2*x+1;
            } else {
                n = 2*(-x);
            }

            std::cout << "Value of n: " << n << std::endl;

            int q = std::floor(n/m);
            int r = n % m;

            createPossibleBinaryTable(m);

            // binary part
            std::string rem = "";
            // m is a power of 2
            if((m != 0) && ((m & (m - 1)) == 0)) {
                // parse r to binary
                std::cout << "r: " << r << std::endl;
                int aux = r;
                if(aux == 0) {
                    rem += "0";
                } else {
                    while(aux != 0) {
                        rem += (aux % 2 == 0 ? '0' : '1');
                        aux /= 2;
                    }
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
            std::cout << "[Binary]: " << rem << std::endl;

            // unary part
            // m is a power of 2
            std::string quo = "";
            for(int i = 0; i < q; i++)
                quo += "1";
            quo += "0";
            std::cout << "[Unary]: " << quo << std::endl;

            // return signal_bit + quo + rem;
            return quo + rem;

        }

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

            int q = unary_string.length();

           
            // binary part
            int remaining_codeword_bits = codeword.length() - q - 1;

            char binary[b];
            for(int i = j; i < codeword.length(); i++) {
                binary[i-j] = codeword[i];
            }

            int res;
            // if m is a power of two
            if((m != 0) && ((m & (m - 1)) == 0)) {
                int decimal_value = 0;
                for(int i = remaining_codeword_bits-1; i >= 0; i--) {
                    decimal_value += (int(binary[remaining_codeword_bits-1-i])-'0')*std::pow(2, i);
                }

                int r = decimal_value;

                // std::cout << "decimal value " << decimal_value << std::endl;
                
                res = q*m + r;

            } else {
                int threshold = std::pow(2, b) - m;
                // std::cout << threshold << std::endl;
                // std::cout << remaining_codeword_bits << std::endl;
                if(remaining_codeword_bits >= threshold) {

                    // binary to decimal of the remaining bits
                    int decimal_value = 0;
                    for(int i = remaining_codeword_bits-1; i >= 0; i--) {
                        decimal_value += (int(binary[remaining_codeword_bits-1-i])-'0')*std::pow(2, i);
                    }
                    // std::cout << decimal_value << std::endl;
                    int r = decimal_value - threshold;
                    res =  q*m + r;
                } else {
                    // binary to decimal of the remaining bits
                    int decimal_value = 0;
                    for(int i = remaining_codeword_bits-1; i >= 0; i--) {
                        decimal_value += (int(binary[remaining_codeword_bits-1-i])-'0')*std::pow(2, i);
                    }
                    int r = decimal_value;
                    res = q*m + r;
                }


            }

            // if(signal_bit == '3')   res = res * (-1);

            std::cout << res << std::endl;

            // return res;

            if(res % 2 == 0) {
                return -res/2;
            } else {
                return (res-1)/2;
            }
        }

        void createPossibleBinaryTable(int m) {
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
                    
                    std::reverse(bin.begin(), bin.end());
                    table.insert(std::pair<int, std::string>(i, bin));
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
                    table.insert(std::pair<int, std::string>(i, bin));
                }
            }
            std::map<int, std::string>::iterator itr;
            std::cout << "---------" << std::endl;
            for(itr = table.begin(); itr != table.end(); ++itr) {
                std::cout << '\t' << itr->first << '\t' << itr-> second << '\n';
            }
            std::cout << "---------" << std::endl;

        }

        std::map<int, std::string> getTable() {
            return table;
        }

};
