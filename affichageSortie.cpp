#include "affichageSortie.h"

using namespace std;

static int noAff;

static void FinProgramme(int signum)
{
    kill(noAff, SIGUSR2);
    exit (EXIT_CODE);
}

void affichageSortie(unsigned int place)
{
#ifdef MAP
	std::ofstream f("affichageSortie.log", ios_base::app);
	f << "Début de affichageSortie(" << place << ")" << std::endl;
#endif
    //--------------------------------Initialisation------------------------------------
    
    bool error = false;
    
    //Association du signal SIGUSR2 à  la fin du programme
    struct sigaction action;
    action.sa_handler = FinProgramme;
    sigaction(SIGUSR2, &action, NULL);
    
    //Ouverture des sémaphore de protection 
 
    
    sem_t* semPtShmCompteur = sem_open (SEM_SHM_COMPTEUR, 0, 0666, 0);
    sem_t* semPtShmRequete = sem_open (SEM_SHM_REQUETE, 0, 0666, 0);
	sem_t* semPtShmParking = sem_open (SEM_SHM_PARKING, 0, 0666, 0);
	
	sem_t* semPtEntree_GB = sem_open(SEM_ENTREE_GB, 0, 0666, 0);
	sem_t* semPtEntree_BP_A = sem_open(SEM_ENTREE_BP_A, 0, 0666, 0);
	sem_t* semPtEntree_BP_P = sem_open(SEM_ENTREE_BP_P, 0, 0666, 0);
    
    //Ouverture des mémoires partagées 
    int shmIdCompteur = shmget(CLEF_COMPTEUR, sizeof (int), 0666 | IPC_CREAT);
    int shmIdRequetes = shmget(CLEF_REQUETES, sizeof (requete) * NB_PORTES, 0666 | IPC_CREAT);
	int shmIdParking = shmget(CLEF_PARKING, sizeof (requete) * CAPACITE_PARKING, 0666);
    
	int* shmPtCompteur;
    shmPtCompteur = (int*) shmat(shmIdCompteur, NULL, 0);
	requete* shmPtRequetes;
    shmPtRequetes = (requete*) shmat(shmIdRequetes, NULL, 0);
	requete * shmPtParking;
    shmPtParking = (requete*) shmat(shmIdParking, NULL, 0);
    
    //------------------------------------Moteur--------------------------------------- 
    
    noAff = SortirVoiture (place);
    
    int st;
    waitpid(noAff, &st, 0);
    
#ifdef MAP
	f << "Demande de lock sur le semaphore ShmCompteur" << std::endl;
#endif

	sem_wait(semPtShmCompteur); //On prend possession de la mémoire partagée servant à compter le nombre de place dans le parking, sinon, on attend qu'elle soit disponible

#ifdef MAP
	f << "Demande de lock sur le semaphore ShmCompteur RÉUSSIE" << std::endl;
#endif
	
	bool isFull = !shmPtCompteur;//* On le fait avant de libérer de la place /!\
	*shmPtCompteur += 1;
	
#ifdef MAP
	f << "Relâchement du lock sur le semaphore ShmCompteur après décrément. Valeur actuelle :" << *shmPtCompteur << std::endl;
#endif
	sem_post(semPtShmCompteur); //On restitue l'accès à la mémoire partagée
	
#ifdef MAP
	f << "Affichage de la sortie effectuée :" << std::endl;
#endif
	
	AfficherSortie (PROF, 2, 2, 2);// @TODO : Implement that !
	
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
#ifdef MAP
	f.close();
#endif
	//* If, for any reason, we get here, make sure everything closes properly :
	FinProgramme(0);
}