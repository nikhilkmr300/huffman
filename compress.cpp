#include <iostream>

#include "huffman.hpp"

int main(int argc, char** argv) {
    if(argc != 4) {
        std::cout << "Usage: ./compress <input_path> <target_path> <decode_table_path>" << std::endl;
        exit(0);
    }

    std::string inputFilepath = argv[1];
    std::string encodedFilepath = argv[2];
    std::string decodeTableFilepath = argv[3];

    HuffmanTree huffmanTree = HuffmanTree(inputFilepath, decodeTableFilepath);

    encode(inputFilepath, encodedFilepath, huffmanTree.getEncodeTable());
}