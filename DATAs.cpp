#include "DATAs.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include<iomanip>
using namespace std;


Hnode::Hnode(): left(nullptr), right(nullptr), data(0), frequency(0) {}

Hnode::Hnode(unsigned char d, unsigned int f): left(nullptr), right(nullptr), data(d), frequency(f) {}

BitWriter::BitWriter(std::ofstream& out): outFile(out), buffer(0), bitCount(0) {}

void HBuild::Compress(const std::string& inputFile, const std::string& outputFile) {
   
    if(!isTxt(inputFile)){
        cout<<"please select a .txt file \n";
        return;
    }

    if(!doesFileExist(inputFile)){
        cout<<"Error, file does not exist or ivalid name.\n";
        return;
    }

    std::ifstream in(inputFile, std::ios::binary);
    if (!in.is_open()) {
        std::cout << "Error: Could not open input file.\n";
        return;
    }

  
    in.seekg(0, std::ios::end);
    originalSize = in.tellg();
    in.seekg(0, std::ios::beg);

    if (originalSize == 0) {
        std::cout << "File is empty.\n";
        return;
    }

   
    countFrequencies(inputFile);
    buildTree();
    generateCodes(root, "");


    std::ofstream out(outputFile, std::ios::binary);

        if (originalSize < 500) {
        std::cout << "File is too small to compress. Storing as raw data.\n";
        
     
        unsigned char flag = 0; 
        out.put(flag);
        
        
        int c;
        while ((c = in.get()) != EOF) {
            out.put(static_cast<char>(c));
        }
        
        in.close();
        out.close();
        displayStats(originalSize, originalSize + 1);
        return; 
    }
   

    
    unsigned char flag = 1;
    out.put(flag); 

    countFrequencies(inputFile);
    buildTree();
    generateCodes(root, "");
   
    writeHeader(out);

    
    BitWriter writer(out);
    int c;
    
   
    while ((c = in.get()) != EOF) {
        std::string code = codeTable[c]; 
        
  
        for (int i = 0; i < code.length(); i++) {
            
            int bit = code[i] - '0'; 
            writer.writeBit(bit);
        }
    }

    
    writer.flush();

  
    in.close();
    out.close();
    
    std::cout << "Successfully compressed to " << outputFile << "\n";
}

void HBuild::writeHeader(std::ofstream& out) {
    
    out.write(reinterpret_cast<char*>(&originalSize), sizeof(originalSize));
    
   
    out.write(reinterpret_cast<char*>(freq), sizeof(freq));
}
void BitWriter::writeBit(int bit) {

    buffer = (buffer << 1) | (bit & 1);
    bitCount++;

   
    if (bitCount == 8) {
        outFile.put(static_cast<char>(buffer));
        buffer = 0;
        bitCount = 0;
    }
}

int BitWriter::flush() {
    if (bitCount == 0) return 0;

   
    buffer = buffer << (8 - bitCount);
    outFile.put(static_cast<char>(buffer));

    int padding = 8 - bitCount;
    buffer = 0;
    bitCount = 0;
    return padding;
}

void HBuild::generateCodes(Hnode* node, string currentCode) {
    if (node == nullptr) return;

    if (node->left == nullptr && node->right == nullptr) {
        codeTable[node->data] = currentCode.empty() ? "0" : currentCode;
        return;
    }

    generateCodes(node->left,  currentCode + "0");
    generateCodes(node->right, currentCode + "1");
}

void HBuild::countFrequencies(const string& filename) {
    for (int i = 0; i < 256; i++) freq[i] = 0;

    ifstream in(filename, ios::binary);
    if (!in.is_open()) return;

    int c;
    while ((c = in.get()) != EOF) {
        freq[c]++;
    }
    in.close();
}

void HBuild::deleteTree(Hnode* node) {
    if (node == nullptr) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

void HBuild::buildTree() {
    PriorityQ Prio(256);

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            Prio.insert(new Hnode((unsigned char)i, freq[i]));
        }
    }

    if (Prio.isEmpty()) {
        root = nullptr;
        return;
    }
    if (Prio.getSize() == 1) {
        root = Prio.getMin();
        return;
    }

    while (Prio.getSize() > 1) {
        Hnode* a = Prio.extractMin();
        Hnode* b = Prio.extractMin();

        Hnode* parent = new Hnode();
        parent->left  = a;
        parent->right = b;
        parent->frequency = a->frequency + b->frequency;

        Prio.insert(parent);
    }

    root = Prio.extractMin();
}

void HBuild::Decompress(const string& inputFile, const std::string& outputFile) {
    std::ifstream in(inputFile, ios::binary);
    if (!in.is_open()) {
        cout<< "Error: Could not open input file for decompression.\n";
        return;
    }

    unsigned char flag;
    in.get(reinterpret_cast<char&>(flag)); // Read the 1st byte

    if (flag == 0) {
       
        std::ofstream out(outputFile, std::ios::binary);
        int c;
        while ((c = in.get()) != EOF) {
            out.put(static_cast<char>(c));
        }
        in.close();
        out.close();
        cout << "Decompression complete (raw copy).\n";
        return; 
    }

    
    in.read(reinterpret_cast<char*>(&originalSize), sizeof(originalSize));
    for (int i = 0; i < 256; i++) {
        in.read(reinterpret_cast<char*>(&freq[i]), sizeof(freq[i]));
    }

    // Rebuild Huffman tree
    buildTree();
    if (root == nullptr) {
        cout << "Error: Huffman tree is empty.\n";
        in.close();
        return;
    }

    std::ofstream out(outputFile, ios::binary);
    if (!out.is_open()) {
        cout<< "Error: Could not open output file for writing.\n";
        deleteTree(root);
        in.close();
        return;
    }

    // Decode bits and traverse tree
    BitReader reader(in);
    Hnode* currentNode = root;
    uint64_t bytesWritten = 0;

    while (bytesWritten < originalSize) {
        int bit = reader.readBit();
        if (bit == -1) break;

        if (bit == 0) {
            currentNode = currentNode->left;
        } else {
            currentNode = currentNode->right;
        }

        if (currentNode == nullptr) {
            cout<<"Error: Invalid bit sequence encountered during traversal.\n";
            break;
        }

        if (currentNode->left == nullptr && currentNode->right == nullptr) {
            out.put(static_cast<char>(currentNode->data));
            bytesWritten++;
            currentNode = root;
        }
    }

    out.close();
    in.close();
    deleteTree(root);
    root = nullptr;
}

void Hbuild::displayStats(uint64_t originalSize, uint64_t compressedSize){
    cout<<"---FILE SUMMARY---\n";
    cout<<"Original size: "<< originalSize << "bytes\n";
    cout<<"Compressed size: "<< compressedSize << "bytes\n";

    long long reduced = (long long)originalSize - (long long)compressedSize;

    cout<<"Saved spaced: "<<reduced <<"bytes\n";
    cout<<"---END OF LINE---\n";
}
bool HBuild::isTxt(const string& filename){
   
    // filename length - 4 
    // start at .Txt = 4
    //  ex filename length = 7
    // 7 - 4, we start readerng in "." then we check if its txt
    if(filename.length() < 4){
        return false;
    }


    //.subtr = like an array but lets us use a existing string then only read a specific line
    string extension = filename.subtr(filename.length() - 4);

    for(int i = 0; i < 4; i++){
        extension[i] = tolower(extension[i]);
    }

    if(extension == ".txt"){
        return true;
    }

    return false;
}

bool HBuild::doesFileExist(const string& filename){
    //.good = pang check if na open ung file / read
    ifstream in(filename);
    return in.good();
}

bool HBuild:: isOutputSafe(const string& filename){
    if(!doesFileExist(filename)){
        return true;
    }
}

bool Hbuild:: isTreeEmpty(){
    if(root->left == nullptr && root->right == nullptr){
        return false;
    }
}