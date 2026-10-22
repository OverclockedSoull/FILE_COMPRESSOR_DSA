#pragma once
#include<iostream>
#include "DATAs.h"
using namespace std;


class Menu{
    public:
    void MainMenu();
    void DisplayFileSummary();
    void handleCompress();
    void handleDecompress();
    Menu();
    void run();
};