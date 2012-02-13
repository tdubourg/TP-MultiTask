/* 
 * File:   main.cpp
 * Author: tdubourg
 *
 * Created on 13 février 2012, 15:08
 */

#include <cstdlib>
#include "Heure.h"
#include "Menu.h"
#include "Outils.h"
#include <time.h>
#include <unistd.h>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    InitialiserApplication(XTERM);
    sleep(10);
    
    TerminerApplication();
    return 0;
}

