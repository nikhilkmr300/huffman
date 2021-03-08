#include <iostream>
#include <fstream>
#include <cmath>

#include "huffman.hpp"
#include <cereal/archives/xml.hpp>
#include <cereal/types/map.hpp>

void encode(std::string inputFilepath, std::string outputFilepath, std::map<char, std::string> encodeTable) {
    std::ifstream inputFile = std::ifstream(inputFilepath);
    if(!inputFile.is_open()) {
        std::cerr << "Could not open file " + inputFilepath << std::endl;
    }
    std::ofstream outputFile = std::ofstream(outputFilepath);
    if(!outputFile.is_open()) {
        std::cerr << "Could not open file " + outputFilepath << std::endl;
    }

    char currentChar;       // Current character being read.
    std::string buffer;     // Buffer to store bits of Huffman code until a complete byte is generated.
    while(inputFile.get(currentChar)) {
        std::string code = encodeTable[currentChar];    // Huffman code corresponding to current character (may be any length).
        buffer += code;

        // Reading characters until buffer becomes equal or greater than 8 bits.
        while(buffer.size() < 8) {
            inputFile.get(currentChar);
            code = encodeTable[currentChar];
            buffer += code;
        }

        // Writing first 8 bits and moving remaining bits to front of buffer to be processed.
        unsigned char byteToFlush = 0;
        for(int i = 0; i < 8; i++) {
            if(buffer[i] == '1') {
                byteToFlush += pow(2, 7 - i);
            }
        }
        // Writing to file in units of bytes.
        outputFile << byteToFlush;
        buffer = buffer.substr(8);
    }

    inputFile.close();
    outputFile.close();
}

void decode(std::string inputFilepath, std::string outputFilepath, std::map<std::string, char> decodeTable) {
    std::ifstream inputFile = std::ifstream(inputFilepath);
    if(!inputFile.is_open()) {
        std::cerr << "Could not open file " + inputFilepath << std::endl;
    }
    std::ofstream outputFile = std::ofstream(outputFilepath);
    if(!outputFile.is_open()) {
        std::cerr << "Could not open file " + outputFilepath << std::endl;
    }

    char currentChar;           // Current hex byte being read from encoded file.
    std::string buffer;         // What is left to decode in current read. May contain one or more bytes.

    while(inputFile.get(currentChar)) {
        std::string byte;       // Current char represented a std::string of 0s and 1s.
        // Reading file byte by byte.
        for(int i = 0; i < 8; i++) {
            char bit = (currentChar >> i) & 0x01;
            if(bit) {
                byte.append("1");
            }
            else {
                byte.append("0");
            }
        }
        // Bit shifting above results in reverse order of bits in byte.
        std::reverse(byte.begin(), byte.end());

        // Adding byte to buffer to be processed.
        buffer += byte;

        // Decoding all possible characters in buffer.
        std::string frame;          // Section of buffer to be matched.
        int matchlen = 0;           // Number of bits matched in buffer.
        for(int i = 0; i < buffer.size(); i++) {
            frame += buffer[i];

            // Looking for a match in decodeTable.
            if(decodeTable.count(frame) > 0) {
                // frame matched in decodeTable, found valid Huffman code.
                char decodedChar = decodeTable[frame];
                outputFile << decodedChar;
                // Adding length of frame to matchlen.
                matchlen += frame.size();
                // Flushing frame for next match.
                frame = "";
            }
        }
        // Removing all matched bits from buffer.
        buffer = buffer.substr(matchlen);
    }

    inputFile.close();
    outputFile.close();
}

void deserializeDecodeTable(std::map<std::string, char>& decodeTable, std::string inputFilepath) {
    std::ifstream ifs = std::ifstream(inputFilepath);

    cereal::XMLInputArchive iarchive = cereal::XMLInputArchive(ifs);
    iarchive(decodeTable);
}