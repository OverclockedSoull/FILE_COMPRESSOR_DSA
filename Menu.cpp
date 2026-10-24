#include <iostream>
#include "Menu.h"
#include "DATAs.h"
using namespace std;

Menu::Menu() {}

void Menu::run() {
    MainMenu();
}

void Menu::MainMenu() {
    int action = -1;
    while (action != 0) {
        cout << "\n======================\n";
        cout << "\tFile Compressor\n";
        cout << "\tSelect action:\n";
        cout << "[1] Compress a file\n";
        cout << "[2] Decompress a file\n";
        cout << "[0] Exit program\n";
        cout << "======================\n";
        cout << "Enter choice: ";
        
        cin >> action;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (action) {
            case 1: handleCompress(); break;
            case 2: handleDecompress(); break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Please enter a valid action.\n";
        }
    }
}

void Menu::handleCompress() {
    HBuild engine;
    string inputFile, outputFile;

    cout << "Enter the name of the .txt file to compress (e.g., test.txt): ";
    cin >> inputFile;

    outputFile = inputFile;
    size_t dotPos = outputFile.find_last_of('.');
    if (dotPos != string::npos) {
        outputFile = outputFile.substr(0, dotPos);
    }
    outputFile += ".oops";

    engine.Compress(inputFile, outputFile);
}

void Menu::handleDecompress() {
    HBuild engine;
    string inputFile, outputFile;

    cout << "Enter the name of the .oops file to decompress (e.g., test.oops): ";
    cin >> inputFile;

    outputFile = inputFile;
    size_t dotPos = outputFile.find_last_of('.');
    if (dotPos != string::npos) {
        outputFile = outputFile.substr(0, dotPos);
    }
    outputFile += "_decompressed.txt";

    engine.Decompress(inputFile, outputFile);
    
   
    string originalFile = inputFile;
    size_t dotPos2 = originalFile.find_last_of('.');
    if (dotPos2 != string::npos) originalFile = originalFile.substr(0, dotPos2) + ".txt";
    
    if (engine.verifyIntegrity(originalFile, outputFile)) {
        cout << "SUCCESS: The decompressed file matches the original perfectly!\n";
    } else {
        cout << "WARNING: Files do not match or original file not found to compare.\n";
    }
}

void Menu::DisplayFileSummary() {}