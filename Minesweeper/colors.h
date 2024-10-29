#ifndef COLORS_H
#define COLORS_H

#include <iostream>
using namespace std;

void setColor(char value) {
	switch (value) {
		case '#':
			cout << "\033[2m";
			break;
		case 'X':
			cout << "\033[30;41m";
			break;
		case 'f':
			cout << "\033[91m";
			break;
		case '1':
			cout << "\033[34m";
			break;
		case '2':
			cout << "\033[32m";
			break;
		case '3':
			cout << "\033[31m";
			break;
		case '4':
			cout << "\033[94m";
			break;
		case '5':
			cout << "\033[95m";
			break;
		case '6':
			cout << "\033[96m";
			break;
		case '7':
			cout << "\033[33m";
			break;
		case '8':
			cout << "\033[97m";
			break;
	};
}

void resetColor() {
	cout << "\033[0m";
}

#endif
