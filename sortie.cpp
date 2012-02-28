#include "sortie.h"

using namespace std;


static void FinProgramme(int signum)
{
    exit(0);
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
    int canalKeySortie = open (CANAL_KEY_SORTIE, O_RDONLY);
    if (canalKeySortie == -1)
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
    unsigned int place;
    int noAffSortie;
    
    for (;;)
    {
    read (canalKeySortie, &place, sizeof(unsigned int)); //On lis dans le canal tant qu'il y a des éléments à lire, sinon, on attend qu'il y en ai de nouveau

     if((noAffSortie = fork ()) == 0){
        //Code du fils affichageSortie
         affichageSortie (place);
         exit(0);
    }
    
    }
    
    //--------------------------------Destruction------------------------------------
}