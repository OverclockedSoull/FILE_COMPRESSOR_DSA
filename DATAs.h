#pragma once
#include <iostream>
#include <string>
#include <fstream>

struct Hnode {
    Hnode* left;
    Hnode* right;
    unsigned char data;
    unsigned int frequency;

    Hnode(){}
    Hnode(unsigned int d, unsigned int f){}
};

class PriorityQ {
private:
    Hnode** array;
    int capacity;
    int size;

    void heapifyUp(int index);
    void heapifyDown(int index);
    void swapNodes(Hnode*& a, Hnode*& b);

public:
    PriorityQ(int cap);
    ~PriorityQ();
    
    void insert(Hnode* node);
    Hnode* extractMin();
    Hnode* getMin();
    bool isEmpty();
    int getSize();
};

class BitWriter {
private:
    std::ofstream& outFile;
    unsigned char buffer;
    int bitCount;

public:
    BitWriter(std::ofstream& out);
    void writeBit(int bit);
    int flush();
};

class BitReader {
private:
    std::ifstream& inFile;
    unsigned char buffer;
    int bitCount;

public:
    BitReader(std::ifstream& in);
    int readBit();
};

class HBuild {
private:
    Hnode* root;
    unsigned int freq[256];
    std::string codeTable[256];
    uint64_t originalSize;
    int paddingBits;

    void buildTree();
    void generateCodes(Hnode* node, std::string currentCode);
    void deleteTree(Hnode* node);
    void countFrequencies(const std::string& filename);

    void writeHeader(std::ofstream& out);
    void readHeader(std::ifstream& in);

public:
    HBuild();
    ~HBuild();

    bool doesFileExist(const std::string& filename);
    bool isTxt(const std::string& filename);
    bool isOutputSafe(const std::string& filename);
    bool isTreeEmpty();

    void Compress(const std::string& inputFile, const std::string& outputFile);
    void Decompress(const std::string& inputFile, const std::string& outputFile);
    
    void displayStats(uint64_t originalSize, uint64_t compressedSize);
    bool verifyIntegrity(const std::string& file1, const std::string& file2);
};