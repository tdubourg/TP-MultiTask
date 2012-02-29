#include "affichageSortie.h"

using namespace std;

static void FinProgramme(int signum)
{
    
}

void affichageSortie(unsigned int place)
{
    //--------------------------------Initialisation------------------------------------
    
    bool error = false;
    
    //Association du signal SIGUSR2 à  la fin du programme
    struct sigaction action;
    action.sa_handler = FinProgramme;
    sigaction(SIGUSR2, &action, NULL);
    
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
    int nbPlaces;
    
    SortirVoiture (place);
    
    sem_wait(semPtShmCompteur); //On prend possession de la mémoire partagée servant à compter le nombre de place dans le parking, sinon, on attend qu'elle soit disponible
    
    nbPlaces = *shmPtCompteur;
    *shmPtCompteur = (nbPlaces + 1);
    
    sem_post(semPtShmCompteur); //On restitue l'accès à la mémoire partagée
}