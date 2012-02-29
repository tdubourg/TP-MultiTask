#include "affichageSortie.h"

using namespace std;

static void FinProgramme(int signum)
{
	exit(EXIT_CODE);
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
    
    
    sem_t* semPtShmCompteur = sem_open (SEM_SHM_COMPTEUR, 0, 0666, 0);
    sem_t* semPtShmRequete = sem_open (SEM_SHM_REQUETE, 0, 0666, 0);
	sem_t* semPtEntree_GB = sem_open(SEM_ENTREE_GB, 0, 0666, 0);
	sem_t* semPtEntree_BP_A = sem_open(SEM_ENTREE_BP_A, 0, 0666, 0);
	sem_t* semPtEntree_BP_P = sem_open(SEM_ENTREE_BP_P, 0, 0666, 0);
    
    //Ouverture des mémoires partagées 
    int shmIdCompteur = shmget(CLEF_COMPTEUR, sizeof (int), 0666 | IPC_CREAT);
    int shmIdRequetes = shmget(CLEF_REQUETES, sizeof (requete) * NB_PORTES, 0666 | IPC_CREAT);
    
	int* shmPtCompteur;
    shmPtCompteur = (int*) shmat(shmIdCompteur, NULL, 0);
	requete* shmPtRequetes;
    shmPtRequetes = (requete*) shmat(shmIdRequetes, NULL, 0);
    
    //------------------------------------Moteur--------------------------------------- 
    
    SortirVoiture (place);
    sem_wait(semPtShmCompteur); //On prend possession de la mémoire partagée servant à compter le nombre de place dans le parking, sinon, on attend qu'elle soit disponible
    bool isFull = !shmPtCompteur;//* On le fait avant de libérer de la place /!\
	*shmPtCompteur += 1;
    sem_post(semPtShmCompteur); //On restitue l'accès à la mémoire partagée
	if(isFull) {
		sem_t* semPtToUnlock;
		//* Parcours du tableau des requêtes :
		sem_wait(semPtShmRequete);
		if(shmPtRequetes[ENTREE_P].arrivee != -1 && shmPtRequetes[ENTREE_GB].arrivee == -1) {
			//* Prof à l'netree prof et à GB y'a personne, le prof doit rentrer :
			semPtToUnlock = semPtEntree_BP_P;
		}
		//* plein d'autres ifs .... 
		sem_post(semPtShmRequete);
		//* Débloquage de la porte qui va bien :
		sem_post(semPtToUnlock);
	}
}