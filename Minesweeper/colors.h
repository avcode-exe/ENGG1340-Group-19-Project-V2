#ifndef COLORS_H
#define COLORS_H

#include <iostream>
#include <unordered_map>

using namespace std;

/**
 * @brief Sets the console text color based on the provided character value.
 *
 * This function changes the console text color using ANSI escape codes
 * corresponding to the given character. The supported characters and their
 * associated colors are:
 * - '#' : Dim color
 * - 'X' : Black text on red background
 * - 'f' : Bright red text
 * - '1' : Blue text
 * - '2' : Green text
 * - '3' : Red text
 * - '4' : Bright blue text
 * - '5' : Bright magenta text
 * - '6' : Bright cyan text
 * - '7' : Yellow text
 * - '8' : Bright white text
 *
 * @param value The character representing the desired color.
 */
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

/**
 * @brief Resets the terminal text color to the default.
 * 
 * This function outputs the ANSI escape code to reset the terminal text color
 * to the default color. It is useful for ensuring that any colored text output
 * is properly reset to avoid unintended color changes in subsequent text.
 */
void resetColor() {
    cout << "\033[0m";
}

#endif
