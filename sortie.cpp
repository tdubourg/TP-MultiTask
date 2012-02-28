#include "sortie.h"

using namespace std;


static void FinProgramme(int signum)
{
    
}

void Sortie()
{	
    
    //--------------------------------Initialisation------------------------------------
    
    bool error = false;
    
    //Association du signal SIGUSR2 à  la fin du programme
    struct sigaction action;
    action.sa_handler = FinProgramme;
    sigaction(SIGUSR2, &action, NULL);
    
    //Ouverture du canal de communication en lecture
    if (open (CANAL_KEY_SORTIE, O_RDONLY) == -1)
    {
	error = true;
    }   
    
    //Ouverture des sémaphore de protection 
    sem_t* semPtShmCompteur;
    sem_t* semPtShmRequete;
    
    semPtShmCompteur = sem_open (SEM_SHM_COMPTEUR, 0, 0666, 0);
    semPtShmRequete = sem_open (SEM_SHM_REQUETE, 0, 0666, 0);
    
    //Ouverture des mémoires partagées 
    int shmIdCompteur;
    int shmIdRequetes;
    
    int* shmPtCompteur;
    requete* shmPtRequetes;
    
    shmIdCompteur = shmget(CLEF_COMPTEUR, sizeof (int), 0666 | IPC_CREAT);
    shmIdRequetes = shmget(CLEF_REQUETES, sizeof (requete) * NB_PORTES, 0666 | IPC_CREAT);
    
    shmPtCompteur = (int*) shmat(shmIdCompteur, NULL, 0);
    shmPtRequetes = (requete*) shmat(shmIdRequetes, NULL, 0);
    
    //------------------------------------Moteur---------------------------------------
    
    
    
    //--------------------------------Destruction------------------------------------
}