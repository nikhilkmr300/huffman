#include <iostream>
#include <map>

#include "huffman.hpp"

int main(int argc, char** argv) {
    if(argc != 4) {
        std::cout << "Usage: ./decompress <input_path> <target_path> <decode_table_path>"  << std::endl;
        exit(0);
    }

    std::string encodedFilepath = argv[1];
    std::string decodedFilepath = argv[2];
    std::string decodeTableFilepath = argv[3];

    std::map<std::string, char> decodeTable;
    deserializeDecodeTable(decodeTable, decodeTableFilepath);

    decode(encodedFilepath, decodedFilepath, decodeTable);
}