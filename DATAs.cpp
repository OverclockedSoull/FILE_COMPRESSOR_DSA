#include "DATA.h"
#include <iostream>
#include <fstream>
#include <cctype>
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