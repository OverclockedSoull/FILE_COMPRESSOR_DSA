#include "DATAs.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <iomanip>
using namespace std;

// ---------- HNODE ----------
Hnode::Hnode() : left(nullptr), right(nullptr), data(0), frequency(0) {}
Hnode::Hnode(unsigned char d, unsigned int f) : left(nullptr), right(nullptr), data(d), frequency(f) {}

// ---------- PRIORITY QUEUE ----------
PriorityQ::PriorityQ(int cap) {
    capacity = cap;
    size = 0;
    array = new Hnode*[capacity];
}
PriorityQ::~PriorityQ() {
    delete[] array;
}
void PriorityQ::swapNodes(Hnode*& a, Hnode*& b) {
    Hnode* temp = a; a = b; b = temp;
}
void PriorityQ::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (array[index]->frequency < array[parent]->frequency) {
            swapNodes(array[index], array[parent]);
            index = parent;
        } else break;
    }
}
void PriorityQ::heapifyDown(int index) {
    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int smallest = index;
        if (left < size && array[left]->frequency < array[smallest]->frequency) smallest = left;
        if (right < size && array[right]->frequency < array[smallest]->frequency) smallest = right;
        if (smallest == index) break;
        swapNodes(array[index], array[smallest]);
        index = smallest;
    }
}
void PriorityQ::insert(Hnode* node) {
    if (size >= capacity) return;
    array[size] = node;
    size++;
    heapifyUp(size - 1);
}
Hnode* PriorityQ::extractMin() {
    if (size == 0) return nullptr;
    Hnode* min = array[0];
    size--;
    if (size > 0) {
        array[0] = array[size];
        heapifyDown(0);
    }
    return min;
}
Hnode* PriorityQ::getMin() { return size > 0 ? array[0] : nullptr; }
bool PriorityQ::isEmpty() { return size == 0; }
int PriorityQ::getSize() { return size; }

// ---------- BIT WRITER & READER ----------
BitWriter::BitWriter(std::ofstream& out) : outFile(out), buffer(0), bitCount(0) {}
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

BitReader::BitReader(ifstream& in) : inFile(in), buffer(0), bitCount(0) {}
int BitReader::readBit() {
    if (bitCount == 0) {
        int c = inFile.get();
        if (c == EOF) return -1;
        buffer = static_cast<unsigned char>(c);
        bitCount = 8;
    }
    int bit = (buffer >> 7) & 1;
    buffer = buffer << 1;
    bitCount--;
    return bit;
}

// ---------- HBUILD CLASS ----------
HBuild::HBuild() : root(nullptr), originalSize(0) {
    for (int i = 0; i < 256; i++) freq[i] = 0;
}
HBuild::~HBuild() {
    deleteTree(root);
}

void HBuild::Compress(const std::string& inputFile, const std::string& outputFile) {
    if (!isTxt(inputFile)) {
        cout << "Error: Please select a .txt file.\n";
        return;
    }
    if (!doesFileExist(inputFile)) {
        cout << "Error: File does not exist or invalid name.\n";
        return;
    }

    std::ifstream in(inputFile, std::ios::binary);
    in.seekg(0, std::ios::end);
    originalSize = in.tellg();
    in.seekg(0, std::ios::beg);

    if (originalSize == 0) {
        std::cout << "File is empty.\n";
        return;
    }

    std::ofstream out(outputFile, std::ios::binary);

    // Fallback for tiny files
    if (originalSize < 500) {
        std::cout << "File is too small to compress. Storing as raw data.\n";
        unsigned char flag = 0; 
        out.put(flag);
        int c;
        while ((c = in.get()) != EOF) out.put(static_cast<char>(c));
        in.close(); out.close();
        displayStats(originalSize, originalSize + 1);
        return; 
    }
    
    // Normal Compression
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
    
    // Get compressed size for stats
    uint64_t compSize = out.tellp();
    out.close();
    
    cout << "Successfully compressed to " << outputFile << "\n";
    displayStats(originalSize, compSize); 
}

void HBuild::Decompress(const string& inputFile, const std::string& outputFile) {
    std::ifstream in(inputFile, ios::binary);
    if (!in.is_open()) {
        cout << "Error: Could not open input file for decompression.\n";
        return;
    }

    unsigned char flag;
    in.get(reinterpret_cast<char&>(flag)); 

    if (flag == 0) {
        std::ofstream out(outputFile, std::ios::binary);
        int c;
        while ((c = in.get()) != EOF) out.put(static_cast<char>(c));
        in.close(); out.close();
        cout << "Decompression complete (raw copy).\n";
        return; 
    }

    readHeader(in);
    buildTree();
    if (root == nullptr) {
        cout << "Error: Huffman tree is empty.\n";
        in.close(); return;
    }

    if (root->left == nullptr && root->right == nullptr) {
        std::ofstream out(outputFile, std::ios::binary);
        for (uint64_t i = 0; i < originalSize; i++) out.put(static_cast<char>(root->data));
        out.close(); in.close();
        cout << "Decompression complete (single character).\n";
        return;
    }

    std::ofstream out(outputFile, ios::binary);
    BitReader reader(in);
    Hnode* currentNode = root;
    uint64_t bytesWritten = 0;

    while (bytesWritten < originalSize) {
        int bit = reader.readBit();
        if (bit == -1) break;

        if (bit == 0) currentNode = currentNode->left;
        else currentNode = currentNode->right;

        if (currentNode == nullptr) {
            cout << "Error: Invalid bit sequence.\n";
            break;
        }
        if (currentNode->left == nullptr && currentNode->right == nullptr) {
            out.put(static_cast<char>(currentNode->data));
            bytesWritten++;
            currentNode = root;
        }
    }

    out.close(); in.close();
    deleteTree(root);
    root = nullptr;
    cout << "Successfully decompressed to " << outputFile << "\n";
}

void HBuild::writeHeader(std::ofstream& out) {
    out.write(reinterpret_cast<char*>(&originalSize), sizeof(originalSize));
    out.write(reinterpret_cast<char*>(freq), sizeof(freq));
}

void HBuild::readHeader(std::ifstream& in) {
    in.read(reinterpret_cast<char*>(&originalSize), sizeof(originalSize));
    for (int i = 0; i < 256; i++) {
        in.read(reinterpret_cast<char*>(&freq[i]), sizeof(freq[i]));
    }
}

void HBuild::generateCodes(Hnode* node, string currentCode) {
    if (node == nullptr) return;
    if (node->left == nullptr && node->right == nullptr) {
        codeTable[node->data] = currentCode.empty() ? "0" : currentCode;
        return;
    }
    generateCodes(node->left, currentCode + "0");
    generateCodes(node->right, currentCode + "1");
}

void HBuild::countFrequencies(const string& filename) {
    for (int i = 0; i < 256; i++) freq[i] = 0;
    ifstream in(filename, ios::binary);
    if (!in.is_open()) return;
    int c;
    while ((c = in.get()) != EOF) freq[c]++;
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
        if (freq[i] > 0) Prio.insert(new Hnode((unsigned char)i, freq[i]));
    }
    if (Prio.isEmpty()) { root = nullptr; return; }
    if (Prio.getSize() == 1) { root = Prio.getMin(); return; }

    while (Prio.getSize() > 1) {
        Hnode* a = Prio.extractMin();
        Hnode* b = Prio.extractMin();
        Hnode* parent = new Hnode();
        parent->left = a;
        parent->right = b;
        parent->frequency = a->frequency + b->frequency;
        Prio.insert(parent);
    }
    root = Prio.extractMin();
}

void HBuild::displayStats(uint64_t originalSize, uint64_t compressedSize) {
    cout << "\n--- FILE SUMMARY ---\n";
    cout << "Original size:   " << originalSize << " bytes\n";
    cout << "Compressed size: " << compressedSize << " bytes\n";

    long long reduced = (long long)originalSize - (long long)compressedSize;
    double percentage = 0.0;
    if (originalSize > 0) percentage = ((double)reduced / (double)originalSize) * 100.0;

    if (reduced > 0) {
        cout << "Space Saved:     " << reduced << " bytes\n";
        cout << "Ratio:           " << fixed << setprecision(2) << percentage << "%\n";
    } else {
        cout << "Space Saved:     0 bytes (overhead)\n";
    }
    cout << "--------------------\n";
}

bool HBuild::isTxt(const string& filename) {
    if (filename.length() < 4) return false;
    string extension = filename.substr(filename.length() - 4);
    for (int i = 0; i < 4; i++) extension[i] = tolower(extension[i]);
    return extension == ".txt";
}

bool HBuild::doesFileExist(const string& filename) {
    ifstream in(filename);
    return in.good();
}

bool HBuild::isOutputSafe(const string& filename) {
    if (!doesFileExist(filename)) return true;
    return false;
}

bool HBuild::isTreeEmpty() {
    return (root == nullptr || (root->left == nullptr && root->right == nullptr));
}

bool HBuild::verifyIntegrity(const string& file1, const string& file2) {
    std::ifstream in1(file1, std::ios::binary);
    std::ifstream in2(file2, std::ios::binary);
    if (!in1.is_open() || !in2.is_open()) return false;

    int c1, c2;
    while (true) {
        c1 = in1.get();
        c2 = in2.get();
        if (c1 == EOF && c2 == EOF) break;
        if (c1 != c2) {
            in1.close(); in2.close();
            return false;
        }
    }
    in1.close(); in2.close();
    return true;
}