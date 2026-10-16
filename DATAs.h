#pragma once
#include<iostream>

struct Hnode{
    Hnode* left;
    Hnode* right;
    unsigned char data;
    unsigned int frequency;
};
class HBuild{
    private:
    Hnode* root;
    public:
    HBuild();
    void MakeTree();
    void Read();
    bool isTxt();
    void 

};