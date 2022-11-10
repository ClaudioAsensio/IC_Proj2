#include <cmath>
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <algorithm>

class Golomb {
    public:

        //encoding an integer into string of bits
        std::string encodeInteger(int n, int m) {
            int q = std::floor(n/m);
            int r = n % m;

            // binary part
            int b = std::ceil(std::log2(m));
            int k = std::pow(2, b) - m;

            
            int num_bin_bits;   // number of bits to binary part
            int value;          // value to be encoded on binary part

            if(r < pow(2, b) - m) {
                num_bin_bits = b - 1;
                value = r;
            } else  {
                num_bin_bits = b;
                value = r + pow(2, b) - m;
            }
            
            std::string rem = "";
            int num_bits = num_bin_bits;

            while(value != 0) {
                rem += (value % 2 == 0 ? '0' : '1');
                value /= 2;
                num_bits--;
            }

            while(num_bits != 0) {
                rem += '0';
                num_bits--;
            }

            std::reverse(rem.begin(), rem.end());

            // encoding unary part
            //if m is power of 2
            std::string quo = "";
            int size = num_bits + 1;
            if((m != 0) && (std::ceil(std::log2(m)) == std::floor(std::log2(m)))) {
                for(int i = 0; i < q; i++) {
                    quo += "0";
                }
                quo += "1";
            } else {
                for(int i = 0; i < q; i++) {
                    quo += "1";
                }
                quo += "0";
            }

            return quo + rem;
        }

        int decodeInteger(std::string codeword, int m) {
            int b = std::ceil(std::log2(m));
            std::string unary_string = "";
            int j = 0;
            if((m != 0) && (std::ceil(std::log2(m)) == std::floor(std::log2(m)))) {
                // read unary part until first 1
                while(true) {
                    if(codeword[j++] == '1')
                        break;
                    unary_string += "0";                
                }
            } else {
                // read unary part until first 0
                while(true) {
                    if(codeword[j++] == '0')
                        break;
                    unary_string += "1";                
                }
            }
            int q = unary_string.length();
            
            // binary part
            int remaining_codeword_bits = codeword.length() - 1 - q;

            char binary[b];
            for(int i = j; i < codeword.length(); i++) {
                binary[i-j] = codeword[i];
            }

            // if m is a power of two
            if((m != 0) && (std::ceil(std::log2(m)) == std::floor(std::log2(m)))) {
                int decimal_value = 0;
                for(int i = remaining_codeword_bits-1; i >= 0; i--) {
                    decimal_value += (int(binary[remaining_codeword_bits-1-i])-'0')*std::pow(2, i);
                }

                int r = decimal_value;

                return q*m + r;

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
                    return q*m + r;
                } else {
                    // binary to decimal of the remaining bits
                    int decimal_value = 0;
                    for(int i = remaining_codeword_bits-1; i >= 0; i--) {
                        decimal_value += (int(binary[remaining_codeword_bits-1-i])-'0')*std::pow(2, i);
                    }
                    int r = decimal_value;
                    return q*m + r;
                }


            }

            return 0;
        }
};
