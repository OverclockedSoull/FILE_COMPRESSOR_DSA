#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
using namespace std;

struct Hnode {
    Hnode* left;
    Hnode* right;
    unsigned char data;
    unsigned int frequency;

    Hnode();
    Hnode(unsigned char d, unsigned int f);
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
    ofstream& outFile;
    unsigned char buffer;
    int bitCount;

public:
    BitWriter(std::ofstream& out);
    void writeBit(int bit);
    int flush();
};

class BitReader {
private:
    ifstream& inFile;
    unsigned char buffer;
    int bitCount;

public:
    BitReader(ifstream& in);
    int readBit();
};

class HBuild {
private:
    Hnode* root;
    unsigned int freq[256];
    string codeTable[256];
    uint64_t originalSize;

    void buildTree();
    void generateCodes(Hnode* node, string currentCode);
    void deleteTree(Hnode* node);
    void countFrequencies(const string& filename);
    void writeHeader(ofstream& out);
    void readHeader(ifstream& in);

public:
    HBuild();
    ~HBuild();

    bool doesFileExist(const string& filename);
    bool isTxt(const string& filename);
    bool isOutputSafe(const string& filename);
    bool isTreeEmpty();

    void Compress(const string& inputFile, const string& outputFile);
    void Decompress(const string& inputFile, const string& outputFile);
    
    void displayStats(uint64_t originalSize, uint64_t compressedSize);
    bool verifyIntegrity(const string& file1, const string& file2);
};