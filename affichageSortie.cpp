#include "affichageSortie.h"

using namespace std;

static int noAff = -1;

static int* shmPtCompteur;
static requete* shmPtRequetes;
static requete * shmPtParking;

static sem_t* semPtShmCompteur;
static sem_t* semPtShmRequete;
static sem_t* semPtShmParking;
static sem_t* semPtEntree_GB;
static sem_t* semPtEntree_BP_A;
static sem_t* semPtEntree_BP_P;

static void FinProgramme (int signum){
#ifdef MAP
	std::ofstream f ("affichageSortie.log", ios_base::app);
	f << "on recoie le signal ..." << std::endl;
#endif
	if(noAff != -1){
#ifdef MAP
		f << "on envoie le signal a la fifille" << std::endl;
#endif
		kill (noAff, SIGUSR2);
		int st = -1;
		waitpid (noAff, &st, 0);
	}

	//Detachement des mémoires partagées 
	shmdt (shmPtCompteur);
	shmdt (shmPtRequetes);
	shmdt (shmPtParking);

	//Fermeture des semaphore
	sem_close (semPtEntree_GB);
	sem_close (semPtEntree_BP_A);
	sem_close (semPtEntree_BP_P);
	sem_close (semPtShmCompteur);
	sem_close (semPtShmParking);
	sem_close (semPtShmRequete);

	exit (EXIT_CODE);
}

void affichageSortie (unsigned int place){
#ifdef MAP
	std::ofstream f ("affichageSortie.log", ios_base::app);
	f << "Début de affichageSortie(" << place << ")" << std::endl;
#endif
	//--------------------------------Initialisation------------------------------------


	//Association du signal SIGUSR2 à  la fin du programme
	struct sigaction action;
	action.sa_handler = FinProgramme;
	sigaction (SIGUSR2, &action, NULL);

	//Ouverture des sémaphore de protection  
	sem_t* semPtShmCompteur = sem_open (SEM_SHM_COMPTEUR, 0, 0666, 0);
	sem_t* semPtShmRequete = sem_open (SEM_SHM_REQUETE, 0, 0666, 0);
	sem_t* semPtShmParking = sem_open (SEM_SHM_PARKING, 0, 0666, 0);
	sem_t* semPtEntree_GB = sem_open (SEM_ENTREE_GB, 0, 0666, 0);
	sem_t* semPtEntree_BP_A = sem_open (SEM_ENTREE_BP_A, 0, 0666, 0);
	sem_t* semPtEntree_BP_P = sem_open (SEM_ENTREE_BP_P, 0, 0666, 0);

	//Ouverture des mémoires partagées 
	int shmIdCompteur = shmget (CLEF_COMPTEUR, sizeof(int), 0666 | 0);
	int shmIdRequetes = shmget (CLEF_REQUETES, sizeof(requete) * NB_PORTES, 0666 | 0);
	int shmIdParking = shmget (CLEF_PARKING, sizeof(requete) * CAPACITE_PARKING, 0666);

	shmPtCompteur = (int*) shmat (shmIdCompteur, NULL, 0);
	shmPtRequetes = (requete*) shmat (shmIdRequetes, NULL, 0);
	shmPtParking = (requete*) shmat (shmIdParking, NULL, 0);

	//------------------------------------Moteur--------------------------------------- 

	noAff = SortirVoiture (place);
#ifdef MAP
	f << "lancement d'un sortir voiture avec le pid : " << noAff << endl;
#endif

	int st = -1;
	do{
		waitpid (noAff, &st, 0);
#ifdef MAP
		f << "waitpid(" << noAff << ") status récupéré : " << st << endl;
#endif		
	}while(st < 0);

#ifdef MAP
	f << "Demande de lock sur le semaphore ShmCompteur" << std::endl;
#endif

	sem_wait (semPtShmCompteur); //On prend possession de la mémoire partagée servant à compter le nombre de place dans le parking, sinon, on attend qu'elle soit disponible

#ifdef MAP
	f << "Demande de lock sur le semaphore ShmCompteur RÉUSSIE" << std::endl;
#endif

	bool isFull = !(*shmPtCompteur); //* /!\ On le fait avant de libérer de la place /!\ 

#ifdef MAP
	f << "Valeur compteur avant incrément : " << *shmPtCompteur << std::endl;
#endif

	*shmPtCompteur = (*shmPtCompteur) + 1;

#ifdef MAP
	f << "Relâchement du lock sur le semaphore ShmCompteur après incrément. Valeur actuelle :" << *shmPtCompteur << std::endl;
#endif
	sem_post (semPtShmCompteur); //On restitue l'accès à la mémoire partagée

#ifdef MAP
	f << "Affichage de la sortie effectuée :" << std::endl;
#endif
	sem_wait (semPtShmParking);
	AfficherSortie (shmPtParking[place].type, shmPtParking[place].plaque, shmPtParking[place].arrivee, time (NULL));
	sem_post (semPtShmParking);
	if(isFull){

#ifdef MAP
		f << "SORTIE : Parking was full just before this car exit, so let's check if they are requests to be satisfied." << std::endl;
#endif
		sem_t* semPtToUnlock;
		//* Parcours du tableau des requêtes :
		sem_wait (semPtShmRequete);
		if(shmPtRequetes[ENTREE_P].type != AUCUN) //* S'il y a un prof à l'entree prof...
		{
			if(shmPtRequetes[ENTREE_GB].type != PROF) //*... Et s'il n'y a pas d'autre prof a l'entree de tous, le prof rentre
			{
				semPtToUnlock = semPtEntree_BP_P;
				AfficherRequete (PROF_BLAISE_PASCAL, AUCUN, 0);
			}
			else //*... Mais s'il y a un prof a l'entree de tous, le premier arrive doit rentrer
			{
				if(shmPtRequetes[ENTREE_P].arrivee < shmPtRequetes[ENTREE_GB].arrivee) //* si le prof a l'entree Prof est arrive avant l'autre, il rentre
				{
					semPtToUnlock = semPtEntree_BP_P;
					AfficherRequete (PROF_BLAISE_PASCAL, AUCUN, 0);
				}
				else //*Sinon, l'autre prof rentre
				{
					semPtToUnlock = semPtEntree_GB;
					AfficherRequete (ENTREE_GASTON_BERGER, AUCUN, 0);
				}
			}
		}
		else //*...Sinon s'il n'y a pas de prof à l'entree prof ...
		{
			if(shmPtRequetes[ENTREE_GB].type == PROF) //*... Si il y a un prof a l'entree de tous, il rentre
			{
				semPtToUnlock = semPtEntree_GB;
				AfficherRequete (ENTREE_GASTON_BERGER, AUCUN, 0);
			}
			else if(shmPtRequetes[ENTREE_GB].type == AUTRE)//* Si il y a un autre a l'entree de tous ...
			{
				if(shmPtRequetes[ENTREE_A].type != AUCUN) //*... Si il y a un autre a l'entree des autres , le premier arrive doit rentrer
				{
					if(shmPtRequetes[ENTREE_A].arrivee < shmPtRequetes[ENTREE_GB].arrivee) //* si l'autre a l'entree autre est arrivee avant l'autre a l'entree de tous, il rentre
					{
						semPtToUnlock = semPtEntree_BP_A;
						AfficherRequete (AUTRE_BLAISE_PASCAL, AUCUN, 0);
					}
					else //*Sinon, c'est l'autre de l'entree de tous qui rentre
					{
						semPtToUnlock = semPtEntree_GB;
						AfficherRequete (ENTREE_GASTON_BERGER, AUCUN, 0);
					}
				}
			}
			else //*Sinon, le dernier cas possible etant qu'il y ai seulement un autre a l'entree autre, on le fait rentrer
			{
				semPtToUnlock = semPtEntree_BP_A;
				AfficherRequete (AUTRE_BLAISE_PASCAL, AUCUN, 0);
			}
		}

		//* Débloquage du sema sur les requêtes
		sem_post (semPtShmRequete);
		//* Débloquage de la porte qui va bien :
		sem_post (semPtToUnlock);
	}
#ifdef MAP
	f.close ();
#endif
	//* If, for any reason, we get here, make sure everything closes properly :
	FinProgramme (0);
}
