#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include <fstream>
#include <optional>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>

#include <boost/format.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/map.hpp>

// Class to represent count of character occurrences in text file.
class CountTable {
public:

    // Constructor to make count table for text in file at filepath.
    CountTable(std::string filepath) {
        std::ifstream inputFile = std::ifstream(filepath);
        if(!inputFile.is_open()) {
                std::cerr << "Could not open file " + filepath << std::endl;
        }

        // Reading file character by character.
        char currentChar;
        while(inputFile.get(currentChar)) {
            if(currentChar < 0 || currentChar > 127) {
                std::cerr << "Detected non-ASCII character in " + filepath + " Please note that non-ASCII characters are not supported and so the uncompressed output may not match the input." << std::endl;
                continue;
            }

            if(!charCounts.count(currentChar)) {
                charCounts[currentChar] = 1;
            }
            else {
                charCounts[currentChar] += 1;
            }

        }

        inputFile.close();
    }

    // Gets character count of particular character.
    int getCount(char c) {
        return (this -> charCounts)[c];
    }

    // Get character counts of all characters in text as a std::map.
    std::map<char, int> getCounts() {
        return this -> charCounts;  // Returns copy of map.
    }

    // Pretty prints character counts to stdout.
    void printCounts() {
        int widthCol1 = 10;
        int widthCol2 = 10;

        std::cout << std::string(widthCol1 + widthCol2, '-') << std::endl;
        std::cout << std::left << std::setw(widthCol1) << "Char" << std::setw(widthCol2) << "Count" << std::endl;
        std::cout << std::string(widthCol1 + widthCol2, '-') << std::endl;
        for(auto [character, count] : this -> charCounts) {
            if(character == 9) {
                std::cout << std::left << std::setw(widthCol1) << "TAB" << std::setw(widthCol2) << count << std::endl;
            }
            else if(character == 10) {
                std::cout << std::left << std::setw(widthCol1) << "LF" << std::setw(widthCol2) << count << std::endl;
            }
            else if(character == 11) {
                std::cout << std::left << std::setw(widthCol1) << "VT" << std::setw(widthCol2) << count << std::endl;
            }
            else if(character == 13) {
                std::cout << std::left << std::setw(widthCol1) << "CR" << std::setw(widthCol2) << count << std::endl;
            }
            else if(character < 32) {
                std::cout << std::left << std::setw(widthCol1) << " " << std::setw(widthCol2) << count << std::endl;
            }
            else {
                std::cout << std::left << std::setw(widthCol1) << character << std::setw(widthCol2) << count << std::endl;
            }
        }
    }

    std::string filepath;               // Filepath of input text file.

private:

    std::map<char, int> charCounts;     // Actual data structure that stores the counts.
};

class HuffmanNode {
public:

    // Constructor for leaf node of Huffman tree.
    HuffmanNode(char character, int count) {
        this -> character = std::make_optional<char>(character);
        this -> count = count;
        this -> leftChild = nullptr;
        this -> rightChild = nullptr;
    }

    // Constructor for internal node of Huffman tree.
    HuffmanNode(int count, HuffmanNode* leftChild, HuffmanNode* rightChild) {
        this -> character = std::nullopt;   // Internal nodes do not have an associated character.
        this -> count = count;
        this -> leftChild = leftChild;
        this -> rightChild = rightChild;
    }

    // Returns character corresponding to leaf node.
    char getCharacter() const {
        try {
            return (this -> character).value();
        }
        catch(std::bad_optional_access e) {
            throw std::runtime_error("Nodes initialized with HuffmanNode(int count) (internal nodes) do not have attribute 'character'.");
        }
    }

    int getCount() const {
        return this -> count;
    }

    bool isLeaf() const {
        if(this -> leftChild == nullptr && this -> rightChild == nullptr) {
            return true;
        }
        // Sanity check
        else if((this -> leftChild == nullptr) && (this -> rightChild != nullptr)) {
            // Has right child but no left child.
            throw std::runtime_error("Error in building Huffman tree. Huffman trees must be full binary trees.");
        }
        // Sanity check
        else if((this -> leftChild != nullptr) && (this -> rightChild == nullptr)) {
            // Has left child but no right child.
            throw std::runtime_error("Error in building Huffman tree. Huffman trees must be full binary trees.");
        }
        else {
            return false;
        }
    }

    // Returns pretty representation of Huffman node as a std::string.
    std::string toString() const {
        if((this -> character).has_value()) {
            std::string character;
            int ascii = (this -> character).value();
            if((this -> character).value() == 9) {
                character = "TAB";
            }
            else if((this -> character).value() == 10) {
                character = "LF";
            }
            else if((this -> character).value() == 11) {
                character = "VT";
            }
            else if((this -> character).value() == 13) {
                character = "CR";
            }
            else if((this -> character).value() < 32) {
                character = " ";
            }
            else {
                character = std::string(1, (this -> character).value());
            }

            return boost::str(boost::format(
                    "(type=leaf, character=%s, ascii=%d, count=%d)"
                )
                % character
                % ascii
                % this -> count
            );
        }
        else {
            return boost::str(boost::format(
                    "(type=internal, count=%d)"
                )
                % this -> count
            );
        }
    }

    ~HuffmanNode() {
        // Recursively deleting all child nodes of this node before deleting this node to avoid memory leak.
        delete this -> leftChild;
        delete this -> rightChild;
        // std::cout << "Deleting " << this -> toString() << std::endl;
    }

    HuffmanNode* leftChild;
    HuffmanNode* rightChild;

private:

    // All fields are immutable.
    int count;
    std::optional<char> character;
    // Using raw pointers as building tree bottom up (child nodes are created before and hence cannot use std::unique_ptr). Refer https://stackoverflow.com/questions/28502067/how-to-assign-the-address-of-an-existing-object-to-a-smart-pointer.
};

// Comparator class to compare priorities of two Huffman nodes.
class HuffmanNodeComparator {
public:

    bool operator()(HuffmanNode* node1, HuffmanNode* node2) {
        // Priority for object with lower count is a larger number as STL uses max priority queue.
        return (node1 -> getCount()) > (node2 -> getCount());
    }
};

// Class to represent a Huffman tree.
class HuffmanTree {
public:

    // Builds a Huffman tree using the character counts in file at inputFilepath and saves the decoding table as XML file at decodeTableFilepath.
    HuffmanTree(std::string inputFilepath, std::string decodeTableFilepath) {
        CountTable countTable = CountTable(inputFilepath);

        // Populating priority queue of nodes. Initially contains only leaf nodes.
        for(auto [character, count] : countTable.getCounts()) {
            HuffmanNode* currentNode = new HuffmanNode(character, count);
            (this -> nodeQueue).push(currentNode);
        }

        this -> buildTree();
        std::cout << "Built tree." << std::endl;
        this -> preorderEncode(this -> root, "");
        std::cout << "Generated Huffman codes." << std::endl;
        this -> serializeDecodeTable(this -> getDecodeTable(), decodeTableFilepath);
        std::cout << "Serialized decode table." << std::endl;
    }

    std::map<char, std::string> getEncodeTable() {
        return this -> encodeTable;
    }

    std::map<std::string, char> getDecodeTable() {
        // Map is one-to-one, so inversion is straightforward.
        std::map<std::string, char> decodeTable;

        for(auto [character, code] : (this -> encodeTable)) {
            decodeTable[code] = character;
        }

        return decodeTable;
    }

    std::string getMinLengthCode() {
        std::string mincode;
        int minlen = INT_MAX;
        for(auto [character, code] : this -> encodeTable) {
            if(code.size() < minlen) {
                mincode = (this -> encodeTable)[character];
                minlen = code.size();
            }
        }

        return mincode;
    }

    std::string getMaxLengthCode() {
        std::string maxcode;
        int maxlen = 0;
        for(auto [character, code] : this -> encodeTable) {
            if(code.size() > maxlen) {
                maxcode = (this -> encodeTable)[character];
                maxlen = code.size();
            }
        }

        return maxcode;
    }

    void printEncodeTable() {
        int widthCol1 = 10;
        int widthCol2 = std::max<int>((this -> getMaxLengthCode()).size(), 4);
        std::cout << std::string(widthCol1 + widthCol2, '-') << std::endl;
        std::cout << std::left << std::setw(widthCol1) << "Char" << std::setw(widthCol2) << "Code" << std::endl;
        std::cout << std::string(widthCol1 + widthCol2, '-') << std::endl;
        for(auto [character, code] : this -> encodeTable) {
            if(character == 9) {
                std::cout << std::left << std::setw(widthCol1) << "TAB" << std::setw(widthCol2) << code << std::endl;
            }
            else if(character == 10) {
                std::cout << std::left << std::setw(widthCol1) << "LF" << std::setw(widthCol2) << code << std::endl;
            }
            else if(character == 11) {
                std::cout << std::left << std::setw(widthCol1) << "VT" << std::setw(widthCol2) << code << std::endl;
            }
            else if(character == 13) {
                std::cout << std::left << std::setw(widthCol1) << "CR" << std::setw(widthCol2) << code << std::endl;
            }
            else if(character < 32) {
                std::cout << std::left << std::setw(widthCol1) << " " << std::setw(widthCol2) << code << std::endl;
            }
            else {
                std::cout << std::left << std::setw(widthCol1) << character << std::setw(widthCol2) << code << std::endl;
            }
        }
    }

private:

    void buildTree() {
        while((this -> nodeQueue).size() >= 2) {
            // Extracting node with smallest count from priority queue.
            HuffmanNode* smallestFirst = (this -> nodeQueue).top();
            (this -> nodeQueue).pop();
            // Extracting node with second smallest count from priority queue.
            HuffmanNode* smallestSecond = (this -> nodeQueue).top();
            (this -> nodeQueue).pop();
            // Pointing root to new node created from nodes with smallest and second smallest counts.
            this -> root = new HuffmanNode(smallestFirst -> getCount() + smallestSecond -> getCount(), smallestFirst, smallestSecond);
            // Pushing current root onto priority queue.
            (this -> nodeQueue).push(this -> root);
        }
    }

    // Generates encodings of all leaf nodes (characters) that are children of root by performing a preorder traversal.
    void preorderEncode(HuffmanNode* root, std::string accumulator) {
        if(root -> isLeaf()) {
            encodeTable[root -> getCharacter()] = accumulator;
            return;
        }
        // 0 for left branch.
        preorderEncode(root -> leftChild, accumulator + "0");
        // 1 for right branch.
        preorderEncode(root -> rightChild, accumulator + "1");
    }

    // outputFilepath must be an XML file.
    void serializeDecodeTable(std::map<std::string, char> decodeTable, std::string outputFilepath) {
        std::ofstream ofs = std::ofstream(outputFilepath);

        cereal::XMLOutputArchive oarchive = cereal::XMLOutputArchive(ofs);
        oarchive(decodeTable);
    }

    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HuffmanNodeComparator> nodeQueue;           // Priority queue to select nodes with smallest counts on each iteration.
    HuffmanNode* root;                          // Root node of Huffman tree.
    std::map<char, std::string> encodeTable;    // Map from character to code.
};

// Generates Huffman encoded output.
void encode(std::string inputFilepath, std::string outputFilepath, std::map<char, std::string> encodeTable);

// Decodes Huffman encoded data.
void decode(std::string inputFilepath, std::string outputFilepath, std::map<std::string, char> decodeTable);

// inputFilepath must be an XML file. The decoding table is loaded in decodeTable.
void deserializeDecodeTable(std::map<std::string, char>& decodeTable, std::string inputFilestream);

#endif