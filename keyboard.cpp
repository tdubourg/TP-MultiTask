#include "keyboard.h"

using namespace std;
void keyboard() {
    string in;
    do {
        getline(cin, in);
    } while(in != "e" && in != "E");
    exit(0);
}
