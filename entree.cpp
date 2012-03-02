#include "entree.h"

static pidvect tachesFilles;
static pid_t pidAttenteFinGarage;
static int PorteNum = -1;
static int canalDesc = -1;


static void FinProgramme(int signum) {
#ifdef MAP
	std::ofstream f("entree_finprog.log", ios_base::app);
	f << "Lancement de FinProgramme pour Entrée" << PorteNum << std::endl;
	f << "Début du massacre des tâches filles." << std::endl;
#endif
	//* Dabord, on tue toutes les tâches filles :
	pidvect::iterator it;
	for (it = tachesFilles.begin(); it != tachesFilles.end(); ++it) {
		int ret = kill(*it, SIGUSR2);
		if(ret != -1) {
			waitpid(*it, &ret, 0);
		}
#ifdef MAP
		f << "Tâche fille " << (*it) << "massacrée." << ((!ret) ? "correctement" : "pas correctement") << std::endl;
#endif
	}
#ifdef MAP
	f << "Fin du massacre des tâches filles." << std::endl;
	f << "FinProgramme (entrée=" << PorteNum << " début de fermeture des canaux" << std::endl;
#endif
	if(canalDesc != -1) {
		close(canalDesc);
	}
	
#ifdef MAP
	f << "FinProgramme (entrée=" << PorteNum << "Fin de fermeture des canaux" << std::endl;
	f.close();
#endif
	exit(EXIT_CODE);
}

static void FinAttenteFinGarage(int signum) {
	//* On tue la tâche fille puis on se termine :
#ifdef MAP
	std::ofstream f("entree_FinAttenteFinGarage.log", ios_base::app);
	f << "FinAttenteFinGarage: Lancement de FinAttenteFinGarage pour une Entrée (porte_num=" << PorteNum << "). Pid=" << pidAttenteFinGarage << std::endl;
#endif
	
	int ret = kill(pidAttenteFinGarage, SIGUSR2);
	
#ifdef MAP
	f << "FinAttenteFinGarage: Envoi de SIGUSR2 à " << pidAttenteFinGarage << " effectué. Code retour = " << ret << ". Errno=" << errno << std::endl;
	f.close();
#endif
	exit(EXIT_CODE);
}

pid_t Garage(requete req, TypeBarriere barriere, voiture tuture, sem_t* semPtShmParking, requete* shmPtParking) {
#ifdef MAP
	std::ofstream f("entree_entreeAttenteFinGarage.log", ios_base::app);
#endif
	//* Lancement d'une tâche fille d'attente de la fin du garage pour affichage
	pid_t noFille;
	if ((noFille = fork()) == 0) {
		//* Code de la fille qui attend la fin de GarerVoiture
		unsigned char place = entreeAttenteFinGarage(barriere, tuture.type, req.arrivee, req.plaque);
		//* On enregistre la voiture dans le parking : 
#ifdef MAP
		f << "ENTREE=" << PorteNum << " Début d'attente sur le séma Parking" << std::endl;
#endif
		sem_wait(semPtShmParking);
#ifdef MAP
		f << "ENTREE=" << PorteNum << " Fin d'attente sur le séma Parking" << std::endl;
#endif
		shmPtParking[place - 1] = req; //* Note : "place" commence à 1 alors que le tableau à 0 !
#ifdef MAP
		f << "ENTREE=" << PorteNum << " Début de débloquage du sémaphore sur le séma Parking" << std::endl;
#endif
		sem_post(semPtShmParking);
#ifdef MAP
		f << "ENTREE=" << PorteNum << " Fin du débloquage sur le séma Parking" << std::endl;
#endif
		exit(EXIT_CODE);
	}
	tachesFilles.push_back(noFille);
	sleep(1);
	
	return noFille;
}
unsigned char entreeAttenteFinGarage(TypeBarriere barriere, TypeUsager usager, time_t arrivee, unsigned int numVoiture) {
#ifdef MAP
	std::ofstream f("entree_entreeAttenteFinGarage.log", ios_base::app);
#endif
	pid_t pidGarage = GarerVoiture(barriere);
	if (pidGarage == -1) {
#ifdef MAP
		f << "Erreur, GarerVoiture() a renvoyé '-1' en tant que PID pour la tâche fille." << std::endl;
#endif
		return 0;
	}
#ifdef MAP
	f << "entreeAttenteFinGarage: Début d'une entreeAttenteFinGarage sur le pid " << pidGarage << std::endl;
#endif
	pidAttenteFinGarage = pidGarage;
	struct sigaction action;
	action.sa_handler = FinAttenteFinGarage;
	sigaction(SIGUSR2, &action, NULL);
	
#ifdef MAP
	f << "entreeAttenteFinGarage : Début d'attente de la fin de la tâche fille GarerVoiture() ayant le pid " << pidGarage << std::endl;
#endif
	
	int st = -1;
	do {
		waitpid(pidGarage, &st, 0); //Attend la fin de la tache fille GarerVoiture()
#ifdef MAP
		f << "entreeAttenteFinGarage (pid=" << pidGarage << ") : Status reçu=" << st << std::endl;
#endif
	} while (st < 0);
	//* Transforming status to the place number :
	unsigned char place = (st& 0xFF00)>>8;
	
	
#ifdef MAP
	f << "entreeAttenteFinGarage : fin de la tâche fille " << pidGarage << " avec le status " << st << "correspondant à la place " << (int)place <<", lancement de l'affichage" << std::endl;
#endif
	
	//* Une fois celle-ci terminée, on affiche la place où elle s'est garée :
	AfficherPlace(place, usager, numVoiture, arrivee);
	
#ifdef MAP
	f.close();
#endif
	
	return place;
}

void entree(int porte_num) {
#ifdef MAP
	//cout << "Pouet";
	std::stringstream fname_tmp;
	fname_tmp << "entree" << porte_num << ".log";
	std::string fname = fname_tmp.str();
	std::ofstream f(fname.c_str());
	f << "Lancement de Entrée avec porte_num = ";
	f << porte_num << std::endl;
#endif
	PorteNum = porte_num;

	//Association du signal SIGUSR2 à  la fin du programme
	struct sigaction action;
	action.sa_handler = FinProgramme;
	sigaction(SIGUSR2, &action, NULL);

#ifdef MAP
	f << "DEBUT récupération des mémoires partagées" << std::endl;
#endif
	int shmIdRequetes = shmget(CLEF_REQUETES, sizeof (requete) * NB_PORTES, 0666 | 0);
	requete *shmPtRequetes = (requete*) shmat(shmIdRequetes, NULL, 0);
	int shmIdCompteur = shmget(CLEF_COMPTEUR, sizeof (unsigned int), 0666 | 0);
	unsigned int * shmPtCompteur = (unsigned int *) shmat(shmIdCompteur, NULL, 0);
	int shmIdParking = shmget(CLEF_PARKING, sizeof (requete) * CAPACITE_PARKING, 0666 | 0);
	requete* shmPtParking = (requete*) shmat(shmIdParking, NULL, 0);
	
#ifdef MAP
	f << "FIN récupération des mémoires partagées" << std::endl;
	f << "Actuellement, il y a " << *shmPtCompteur << " places libres ds le parking." << std::endl;
#endif

	//* On essaie d'ouvrir le sema, si ça marche pas, on réessaie ! Car de toutes façons, le programme ne peut pas fonctionner
	//* Sans ce sémaphore, attendons donc que qq'un l'initialise !
#ifdef MAP
	f << "DEBUT récupération du séma pour Compteur" << std::endl;
#endif
	sem_t* semPtShmCompteur;
	if((semPtShmCompteur = sem_open(SEM_SHM_COMPTEUR, 0, 0666, 1)) == SEM_FAILED) {
		Afficher(MESSAGE, "Erreur d'ouverture d'un sémaphore dans l'une des Entrée, terminaison de la tâche");
		FinProgramme(EXIT_CODE);
	}
#ifdef MAP
	f << "FIN récupération du séma pour Compteur" << std::endl;
#endif
	
#ifdef MAP
	f << "DEBUT récupération du séma pour Parking" << std::endl;
#endif
	sem_t* semPtShmParking;
	if((semPtShmParking = sem_open(SEM_SHM_PARKING, 0, 0666, 1)) == SEM_FAILED) {
		Afficher(MESSAGE, "Erreur d'ouverture d'un sémaphore dans l'une des Entrée, terminaison de la tâche");
		FinProgramme(EXIT_CODE);
	}
#ifdef MAP
	f << "FIN récupération du séma pour Parking" << std::endl;
#endif
	const char *cname;
	TypeBarriere barriere;
	const char * sem_key;
	switch (porte_num) {
		case ENTREE_P:
			cname = CANAL_KEY_ENTREE_BP_P;
			barriere = PROF_BLAISE_PASCAL;
			sem_key = SEM_ENTREE_BP_P;
			break;
		case ENTREE_A:
			cname = CANAL_KEY_ENTREE_BP_A;
			barriere = AUTRE_BLAISE_PASCAL;
			sem_key = SEM_ENTREE_BP_A;
			break;
		case ENTREE_GB:
			cname = CANAL_KEY_ENTREE_GB;
			barriere = ENTREE_GASTON_BERGER;
			sem_key = SEM_ENTREE_GB;
			break;
	}
#ifdef MAP
	f << "DEBUT récupération du séma pour Compteur" << std::endl;
#endif
	sem_t* semPtEntree;
	if((semPtEntree = sem_open(sem_key, 0, 0666, 1)) == SEM_FAILED) {
		Afficher(MESSAGE, "Erreur d'ouverture d'un sémaphore dans l'une des Entrée, terminaison de la tâche");
		FinProgramme(EXIT_CODE);
	}
	
	
	
#ifdef MAP
	f << "FIN récupération du séma pour Compteur" << std::endl;
#endif
#ifdef MAP
	f << "Debut d'ouverture du canal " << cname << endl;
#endif
	
	canalDesc = open(cname, O_RDONLY);
	
#ifdef MAP
	f << "Fin d'ouverture du canal " << cname << endl;
#endif
	if (canalDesc == -1) {//* L'ouverture du canal a échoué, on laisse tomber
		return;
	}


#ifdef MAP
	f << "Entrée : Debut de lecture du canal" << std::endl;
#endif
	voiture tuture;
	for(;;) {//* Phase moteur
		read(canalDesc, &tuture, sizeof (voiture));
		DessinerVoitureBarriere(barriere, tuture.type);
#ifdef MAP
		f << "Entrée=" << PorteNum << " : Valeur lue sur le canal : voiture id=" << tuture.id << std::endl;
		f << "Entrée=" << PorteNum << " : Il y a actuellement " << (*shmPtCompteur) << "places libres dans le parking." << std::endl;
#endif
		
		//* On formule une requête d'entrée :
		requete req;
		req.arrivee = (int) time(NULL);
		req.type = tuture.type;
		req.plaque = tuture.plaque;
		if ((*shmPtCompteur) > 0) {
			//* Y'a de la place, on se gare :
			//* On décrémente le compteur avant :
			//* Avec gestion du sémaphore
			sem_wait(semPtShmCompteur);
			*shmPtCompteur -= 1; //* On décrémente le nb de places dispo !
			sem_post(semPtShmCompteur);

			Garage(req, barriere, tuture, semPtShmParking, shmPtParking);
		} else {//* Le parking est plein :
			shmPtRequetes[porte_num] = req;
			//* On affiche cette requête :
			AfficherRequete (barriere, tuture.type, req.arrivee);
			//* Puis on attends l'autorisation de faire entrer la voiture :
#ifdef MAP
		f << "Entrée=" << PorteNum << " : Mise en attente sur le sémaphore" << std::endl;
#endif
			sem_wait(semPtEntree);
#ifdef MAP
		f << "Entrée=" << PorteNum << " : Semaphore débloqué !" << std::endl;
#endif
			//* Et hop, jolie répétition de code ! @TODO
			//* Y'a de la place, on se gare :
			//* On décrémente le compteur avant :
			//* Avec gestion du sémaphore
			sem_wait(semPtShmCompteur);
			*shmPtCompteur -= 1; //* On décrémente le nb de places dispo !
			sem_post(semPtShmCompteur);

			Garage(req, barriere, tuture, semPtShmParking, shmPtParking);
		}
	}
#ifdef MAP
	f.close();
#endif
	//* If, for any reason, we get here, make sure everything closes properly :
	FinProgramme(0);
}