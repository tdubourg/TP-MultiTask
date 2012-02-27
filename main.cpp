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
#include <sys/shm.h>

/* Includes non systemes ----------------------------------------------------------------------------- */

#include "Heure.h"
#include "Menu.h"
#include "Outils.h"
#include "keyboard.h"

#define CLEF_COMPTEUR 11111
#define CLEF_REQUETES 22222

using namespace std;


int main(int argc, char** argv) {

    InitialiserApplication(XTERM);
    
    pid_t noKeyboard;
    pid_t noEntrees;
    pid_t noSortie;
    int shmIdCompteur;
    int shmIdRequetes;
    void* shmPtCompteur;
    void* shmPtRequetes;
    
    if ((noKeyboard = fork()) == 0)
    {
        //Code du fils Keyboard
        keyboard();
    }
    if ((noEntrees = fork()) == 0)
    {
        //Code des fils Entrées
    }
    if ((noSortie = fork()) == 0)
    {
        //Code du fils Sortie
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
        sigaction(SIGUSR1, &action, NULL);
        sigaction(SIGUSR2, &action, NULL);
        
        //Création des canaux
        mode_t mode;
        const char *chemin = "key_entree";
        mode = S_IRUSR;
        mkfifo (chemin, mode);  //Canal reliant Keyboard et Entree
        chemin = "key_sortie";
        mkfifo (chemin, mode);  //Canal reliant Keyboard et Sortie
        
        //Création des mémoires partagées
        if ((shmIdCompteur = shmget (CLEF_COMPTEUR, sizeof(int), 0666 | IPC_CREAT)) < 0)
        {
            exit(1);
        }
        if ((shmIdRequetes = shmget (CLEF_REQUETES, sizeof(string), 0666 | IPC_CREAT)) < 0)//Me souvient plus de quoi on mettait dans celui là... @TODO
        {
            exit(1);
        }
        
        if ((shmPtCompteur = shmat (shmIdCompteur, NULL, 0)) == (void*)-1)
        {
            exit(1);
        }
        if ((shmPtRequetes = shmat (shmIdRequetes, NULL, 0)) == (void*)-1)
        {
            exit(1);
        }
        
        
        //---------------------------------------------Moteur-------------------------------------------
        int st = -1;
        do {
            waitpid(noKeyboard, &st, 0);
        } while(st != 0);
        
        //----------------------------------------------Destruction-------------------------------------
        
        //Envoi du signal de fin d'application aux taches filles
        kill (0, SIGUSR1);
        
        //Attente de la fin de toutes les taches filles pour detruire correctement les canaux et sémaphores
        //@TODO
        
        
        //Destruction des canaux, il est necessaire d'attendre qu'il n'y ai plus de lecteurs ni d'écrivains. 
        chemin = "key_entree";
        unlink(chemin);
        chemin = "key_sortie";
        unlink(chemin);
        
        //Destruction des mémoires partagées 
        shmdt(shmPtCompteur);
        shmdt(shmPtRequetes);
    }
    
    TerminerApplication();
    return 0;
}

