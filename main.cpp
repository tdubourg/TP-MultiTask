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
#include <sys/stat.h>

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
        
        //-----------------------------------------initialisation---------------------------------
        
        //masquages des signaux
        struct sigaction action;
        action.sa_handler = SIG_IGN;
        action.sa_flags = 0;
        sigaction (SIGINT, &action, NULL);
        sigaction (SIGUSR1, &action, NULL);
        sigaction (SIGUSR2, &action, NULL);
        
        //Création des canaux
        mode_t mode;
        char *chemin = "key_entree";
        mode = S_IRUSR;
        mkfifo (chemin, mode);  //Canal reliant Keyboard et Entree
        char *chemin = "key_sortie";
        mkfifo (chemin, mode);  //Canal reliant Keyboard et Sortie
        
        
        
        //---------------------------------------------Moteur-------------------------------------------
        int st = -1;
        do {
            waitpid(noKeyboard, &st, 0);
        } while(st != 0);
        
        //----------------------------------------------Destruction-------------------------------------
        
    }
    
    TerminerApplication();
    return 0;
}

