/* 
 * File:   main.cpp
 * Author: tdubourg
 *
 * Created on 13 février 2012, 15:08
 */

/*-------------------------------------------- Includes systemes ----------------------------------------------- */

#include <cstdlib>
#include <time.h>
#include <unistd.h> 
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>

/*----------------------------------------- Includes non systemes ---------------------------------------------- */

#include "Heure.h"
#include "Menu.h"
#include "Outils.h"
#include "keyboard.h"

#define CLEF_COMPTEUR 11111
#define CLEF_REQUETES 22222

const char* CANAL_KEY_ENTREE = "key_entree";
const char* CANAL_KEY_SORTIE = "key_sortie";
const char* SEM_ENTREE_GB = "/entree_GB";
const char* SEM_ENTREE_BP_P = "/entree_BPP";
const char* SEM_ENTREE_BP_A = "/entree_BPA";

using namespace std;

int main(int argc, char** argv) {

    pid_t noKeyboard;
    pid_t noEntrees;
    pid_t noSortie;

    if ((noKeyboard = fork()) == 0) {
        //Code du fils Keyboard
        keyboard();
    }
    if ((noEntrees = fork()) == 0) {
        //Code des fils Entrées
    }
    if ((noSortie = fork()) == 0) {
        //Code du fils Sortie
    } else {
        //Code de la mère

        //-----------------------------------------initialisation---------------------------------

        int shmIdCompteur;
        int shmIdRequetes;

        void* shmPtCompteur;
        void* shmPtRequetes;

        sem_t* semPtEntreeGB; //sémaphore de synchronisation pour l'entrée Gaston Berger
        sem_t* semPtEntreeBPP; //sémaphore de synchronisation pour l'entrée Blaise Pascal (Prof)
        sem_t* semPtEntreeBPA; //sémaphore de synchronisation pour l'entrée Blaise Pascal (Autre)

        //Création de l'écran et de ses protections
        InitialiserApplication(XTERM);

        //masquages des signaux
        struct sigaction action;
        action.sa_handler = SIG_IGN;
        action.sa_flags = 0;
        sigaction(SIGINT, &action, NULL);
        sigaction(SIGUSR1, &action, NULL);
        sigaction(SIGUSR2, &action, NULL);

        //Création des canaux
        mode_t mode;
        mode = S_IRUSR;
        mkfifo(CANAL_KEY_ENTREE, mode); //Canal reliant Keyboard et Entree
        mkfifo(CANAL_KEY_SORTIE, mode); //Canal reliant Keyboard et Sortie

        //Création des mémoires partagées
        if ((shmIdCompteur = shmget(CLEF_COMPTEUR, sizeof ( int), 0666 | IPC_CREAT)) < 0) {
            exit(1);
        }
        if ((shmIdRequetes = shmget(CLEF_REQUETES, sizeof ( string), 0666 | IPC_CREAT)) < 0)//Me souvient plus de quoi on mettait dans celui là... @TODO
        {
            exit(1);
        }
        if ((shmPtCompteur = shmat(shmIdCompteur, NULL, 0)) == NULL) {
            exit(1);
        }
        if ((shmPtRequetes = shmat(shmIdRequetes, NULL, 0)) == NULL) {
            exit(1);
        }

        //Création des sémaphores
        if ((semPtEntreeGB = sem_open(SEM_ENTREE_GB, O_CREAT, 0666, 0)) == SEM_FAILED) {
            exit(1);
        }
        if ((semPtEntreeBPA = sem_open(SEM_ENTREE_BP_A, O_CREAT, 0666, 0)) == SEM_FAILED) {
            exit(1);
        }
        if ((semPtEntreeBPP = sem_open(SEM_ENTREE_BP_P, O_CREAT, 0666, 0)) == SEM_FAILED) {
            exit(1);
        }

        //---------------------------------------------Moteur-------------------------------------------
        int st = -1;
        do {
            waitpid(noKeyboard, &st, 0);
        } while (st != 0);


        //----------------------------------------------Destruction-------------------------------------

        //Envoi du signal de fin d'application aux taches filles
        kill(0, SIGUSR1);

        //Attente de la fin de toutes les taches filles pour detruire correctement les canaux et sémaphores
        //@TODO


        //Destruction des canaux, il est necessaire d'attendre qu'il n'y ai plus de lecteurs ni d'écrivains. 
        unlink(CANAL_KEY_ENTREE);
        unlink(CANAL_KEY_SORTIE);

        //Destruction des mémoires partagées 
        shmdt(shmPtCompteur);
        shmdt(shmPtRequetes);

        //Destruction des sémaphores
        sem_unlink(SEM_ENTREE_BP_A);
        sem_unlink(SEM_ENTREE_BP_P);
        sem_unlink(SEM_ENTREE_GB);

        //Suppression de l'écran et de ses protections
        TerminerApplication();
    }

    return 0;
}

