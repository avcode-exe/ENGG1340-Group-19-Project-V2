#ifndef COLORS_H
#define COLORS_H

#include <iostream>
#include <unordered_map>

using namespace std;

void setColor(char value) {
    static const unordered_map<char, string> colorCodes = {
        {'#', "\033[2m"},
        {'X', "\033[30;41m"},
        {'f', "\033[91m"},
        {'1', "\033[34m"},
        {'2', "\033[32m"},
        {'3', "\033[31m"},
        {'4', "\033[94m"},
        {'5', "\033[95m"},
        {'6', "\033[96m"},
        {'7', "\033[33m"},
        {'8', "\033[97m"}
    };

    auto it = colorCodes.find(value);
    if (it != colorCodes.end()) {
        cout << it->second;
    }
}

void resetColor() {
    cout << "\033[0m";
}

#endif
