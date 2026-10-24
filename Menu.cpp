#include<iostream>
#include "Menu.h"
#include "DATAs.h"
using namespace std;


Menu :: Menu(){}
void Menu::MainMenu(){
    
    int action = -1;
    
    while(action != 0){
    cout<<"======================\n";
    cout<<"\tFile compressor\n";
    cout<<"\tSelect action: \n";
    cout<<"[1] Enter file to compress\n";
    cout<<"[2] Decompress file \n";
    cout<<"======================\n";

    switch(action)
    case 1: handleCompress();
    break;

    case 0: handleDecompress();
    break;

    default:
    cout<<"Please Enter a valid action\n";

    }
    
}

void Menu::run(){
    MainMenu();
}

void Menu::handleCompress() {
    HBuild engine;
    string inputFile, outputFile;

    cout << "Enter the name of the .txt file to compress (e.g., test.txt): ";
    cin >> inputFile;

  
    outputFile = inputFile;
    size_t dotPos = outputFile.find_last_of('.');

    // para lang manahanp ung . position kasi cinopy natin yung file na gustong icompress sa output, then rename ung .tct to huff para pag ddecompress, ung i dedecompress lang is ung may .huff
    if (dotPos != string::npos) {
        outputFile = outputFile.substr(0, dotPos);
    }
    outputFile += ".huff";

    cout << "Compressing " << inputFile << " to " << outputFile << "...\n";
    engine.Compress(inputFile, outputFile);
}
