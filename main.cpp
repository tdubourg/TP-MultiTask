/* 
 * File:   main.cpp
 * Author: tdubourg
 *
 * Created on 13 février 2012, 15:08
 */

/* Includes systemes ----------------------------------------------------------------------------- */

#include <cstdlib>
#include <time.h>
#include <unistd.h> 
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

/* Includes non systemes ----------------------------------------------------------------------------- */
#include "Heure.h"
#include "Menu.h"
#include "Outils.h"


using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    InitialiserApplication(XTERM);
    
    pid_t noKeyboard;
    
    if ((noKeyboard = fork()) == 0)
    {
        //Code du fils Keyboard
        
    }
    else 
    {
        //Code de la mère
        int st = -1;
        do {
            waitpid(noKeyboard, &st, 0);
        } while(st != 0);
       // sigaction (SIGINT,)
    }
    
    TerminerApplication();
    return 0;
}

