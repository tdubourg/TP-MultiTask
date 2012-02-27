/* 
 * File:   main.cpp
 * Author: fdevriese
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
#include "sortie.h"
#include "Config.h"

#define CLEF_COMPTEUR 11111
#define CLEF_REQUETES 22222

using namespace std;

int main(int argc, char** argv) {

    //Création de l'écran et de ses protections
    InitialiserApplication(XTERM);
    
    bool error = false; //Booléen permettant de vérifier si une erreur s'est produite lors de l'initialisation de l'application

    pid_t noKeyboard;
    pid_t noEntreeGB;
    pid_t noEntreeBPP;
    pid_t noEntreeBPA;
    pid_t noSortie;
    pid_t noHeure;

    if ((noKeyboard = fork()) == 0) 
    {
	//Code du fils Keyboard
	keyboard();
    }
    else if (noKeyboard == -1) 
    {
	error = true;
    }
    else if ((noHeure = fork()) == 0) 
    {
	//Code de l'heure
	ActiverHeure();
    }
    else if (noHeure == -1) 
    {
	error = true;
    }
    else if ((noEntreeGB = fork()) == 0) 
    {
	//Code du fils entree Gaston Berger
    }
    else if (noEntreeGB == -1) 
    {
	error = true;
    }
    else if ((noEntreeBPP = fork()) == 0) 
    {
	//Code du fils entree Blaise Pascal (Prof)
    }
    else if (noEntreeBPP == -1) 
    {
	error = true;
    }
    else if ((noEntreeBPA = fork()) == 0) 
    {
	//Code du fils entree Blaise Pascal (Autres)
    }
    else if (noEntreeBPA == -1) 
    {
	error = true;
    }
    else if ((noSortie = fork()) == 0) 
    {
	//Code du fils Sortie
    }
    else if (noSortie == -1) 
    {
	error = true;
    }
    else 
    {
	//Code de la mère

	//-----------------------------------------initialisation---------------------------------

	int shmIdCompteur;
	int shmIdRequetes;

	void* shmPtCompteur;
	void* shmPtRequetes;

	sem_t* semPtEntreeGB; //sémaphore de synchronisation pour l'entrée Gaston Berger
	sem_t* semPtEntreeBPP; //sémaphore de synchronisation pour l'entrée Blaise Pascal (Prof)
	sem_t* semPtEntreeBPA; //sémaphore de synchronisation pour l'entrée Blaise Pascal (Autre)
	sem_t* semPtShmCompteur; //sémaphore de protection de la mémoire partagée "compteur"
	sem_t* semPtShmRequete; //sémaphore de protection de la mémoire partagée "requete"

	//masquages des signaux
	struct sigaction action;
	action.sa_handler = SIG_IGN;
	action.sa_flags = 0;
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGUSR1, &action, NULL);
	sigaction(SIGUSR2, &action, NULL);

	//Création des canaux
	if (mkfifo(CANAL_KEY_ENTREE_BP_A, 0666) == -1 && !error) //Canal reliant Keyboard et Entree blaise pascal autres
	{	    
	    error = true;
	}
	if (mkfifo(CANAL_KEY_ENTREE_BP_P, 0666) == -1 && !error) //Canal reliant Keyboard et Entree blaise pascal profs
	{	    
	    error = true;
	}
	if (mkfifo(CANAL_KEY_ENTREE_GB, 0666) == -1 && !error) //Canal reliant Keyboard et Entree gaston berger
	{	    
	    error = true;
	}
	if (mkfifo(CANAL_KEY_SORTIE, 0666) == -1 && !error) //Canal reliant Keyboard et Sortie
	{
	    error = true;
	}

	//Création des mémoires partagées
	if ((shmIdCompteur = shmget(CLEF_COMPTEUR, sizeof ( int), 0666 | IPC_CREAT)) < 0 && !error) {
	    error = true;
	}
	if ((shmIdRequetes = shmget(CLEF_REQUETES, sizeof ( string), 0666 | IPC_CREAT)) < 0 && !error)//Me souvient plus de quoi on mettait dans celui là... @TODO
	{
	    error = true;
	}
	if ((shmPtCompteur = shmat(shmIdCompteur, NULL, 0)) == NULL && !error) {
	    error = true;
	}
	if ((shmPtRequetes = shmat(shmIdRequetes, NULL, 0)) == NULL && !error) {
	    error = true;
	}

	//Création des sémaphores
	if ((semPtEntreeGB = sem_open(SEM_ENTREE_GB, O_CREAT, 0666, 0)) == SEM_FAILED && !error) {
	    error = true;
	}
	if ((semPtEntreeBPA = sem_open(SEM_ENTREE_BP_A, O_CREAT, 0666, 0)) == SEM_FAILED && !error) {
	    error = true;
	}
	if ((semPtEntreeBPP = sem_open(SEM_ENTREE_BP_P, O_CREAT, 0666, 0)) == SEM_FAILED && !error) {
	    error = true;
	}
	if ((semPtShmCompteur = sem_open(SEM_SHM_COMPTEUR, O_CREAT, 0666, 0)) == SEM_FAILED && !error) {
	    error = true;
	}
	if ((semPtShmRequete = sem_open(SEM_SHM_REQUETE, O_CREAT, 0666, 0)) == SEM_FAILED && !error) {
	    error = true;
	}

	
	//---------------------------------------------Moteur-------------------------------------------
	int st = -1;
	do {
	    waitpid(noKeyboard, &st, 0);  //Attend la fin de la tache fille Keyboard
	} while (st != 0 && !error);


	//----------------------------------------------Destruction-------------------------------------

	if (error) //S'il ya eu une erreur pendant l'initialisation, on demande à Keyboard de se détruire
	{
	    kill(noKeyboard, SIGUSR2);
	}
	
	//Envoi du signal de fin d'application aux taches filles et attente de la fin de celles ci
	kill(noEntreeBPA, SIGUSR2);
	do {
	waitpid(noEntreeBPA, &st, 0);
	} while (st !=0 && !error);
	
	kill(noEntreeBPP, SIGUSR2);
	do {
	waitpid(noEntreeBPP, &st, 0);
	} while (st !=0 && !error);
	
	kill(noEntreeGB, SIGUSR2);
	do {
	waitpid(noEntreeGB, &st, 0);
	} while (st !=0 && !error);
	
	kill(noHeure, SIGUSR2);
	do {
	waitpid(noHeure, &st, 0);
	} while (st !=0 && !error);
	
	kill(noSortie, SIGUSR2);
	do {
	waitpid(noSortie, &st, 0);
	} while (st !=0 && !error);

	//Destruction des canaux, il est necessaire d'attendre qu'il n'y ai plus de lecteurs ni d'écrivains. 
	unlink(CANAL_KEY_ENTREE_BP_A);
	unlink(CANAL_KEY_ENTREE_BP_P);
	unlink(CANAL_KEY_ENTREE_GB);
	unlink(CANAL_KEY_SORTIE);

	//Destruction des mémoires partagées 
	shmdt(shmPtCompteur);
	shmdt(shmPtRequetes);

	//Destruction des sémaphores
	sem_unlink(SEM_ENTREE_BP_A);
	sem_unlink(SEM_ENTREE_BP_P);
	sem_unlink(SEM_ENTREE_GB);
	sem_unlink(SEM_SHM_COMPTEUR);
	sem_unlink(SEM_SHM_REQUETE);

	//Suppression de l'écran et de ses protections
	TerminerApplication();
    }

    if (error)
    {
	return 1;
    }
    else
    {
	return 0;
    }
}

