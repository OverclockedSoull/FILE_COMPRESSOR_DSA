#include "DATA.h"
#include <iostream>
#include <fstream>
using namespace std;


Hnode::Hnode(): left(nullptr), right(nullptr), data(0), frequency(0) {}

Hnode::Hnode(unsigned char d, unsigned int f): left(nullptr), right(nullptr), data(d), frequency(f) {}

BitWriter::BitWriter(std::ofstream& out): outFile(out), buffer(0), bitCount(0) {}



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