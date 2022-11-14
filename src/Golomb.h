#include <cmath>
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <algorithm>

class Golomb {
    public:

        //encoding an integer into string of bits
        std::string encodeInteger(int x, int m) {

            // map values to handle negative numbers
            int n;
            if(x >= 0) {
                n = 2*x+1;
            } else {
                n = 2*(-x);
            }

            std::cout << "Mapped to: " << n << std::endl;

            int q = std::floor(n/m);
            int r = n % m;

            // binary part
            std::string rem = "";
            // m is a power of 2
            if((m != 0) && ((m & (m - 1)) == 0)) {
                // parse r to binary
                int aux = r;
                while(aux != 0) {
                    rem += (aux % 2 == 0 ? '0' : '1');
                    aux /= 2;
                }
                std::cout << "[Binary]: " << rem << std::endl;
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

                std::reverse(rem.begin(), rem.end());

            }

            // unary part
            // m is a power of 2
            std::string quo = "";
            for(int i = 0; i < q; i++)
                quo += "0";
            quo += "1";
            std::cout << "[Unary]: " << quo << std::endl;

            return quo + rem;

        }

        int decodeInteger(std::string codeword, int m) {
            int b = std::ceil(std::log2(m));
            std::string unary_string = "";
            int j = 0;
            // if((m != 0) && (std::ceil(std::log2(m)) == std::floor(std::log2(m)))) {
                // read unary part until first 1
                while(true) {
                    if(codeword[j++] == '1')
                        break;
                    unary_string += "0";                
                }
            // } else {
            //     // read unary part until first 0
            //     while(true) {
            //         if(codeword[j++] == '0')
            //             break;
            //         unary_string += "1";                
            //     }
            // }
            int q = unary_string.length();
            
            // binary part
            int remaining_codeword_bits = codeword.length() - 1 - q;

            char binary[b];
            for(int i = j; i < codeword.length(); i++) {
                binary[i-j] = codeword[i];
            }

            int res;
            // if m is a power of two
            if((m != 0) && (std::ceil(std::log2(m)) == std::floor(std::log2(m)))) {
                int decimal_value = 0;
                for(int i = remaining_codeword_bits-1; i >= 0; i--) {
                    decimal_value += (int(binary[remaining_codeword_bits-1-i])-'0')*std::pow(2, i);
                }

                int r = decimal_value;

                
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

            std::cout << res << std::endl;
            if(res % 2 == 0) {
                return res / 2 * -1;
            } else {
                return (res-1) / 2;
            }
        }
};
