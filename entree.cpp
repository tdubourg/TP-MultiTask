/*************************************************************************
                           entree  -  description
                             -------------------
	début                : 16/02/2012
	copyright            : (C) 2012-2042 par tdubourg
	e-mail               : theo.dubourg@insa-lyon.fr
*************************************************************************/

//---------- Réalisation de la tâche <Entree> (fichier entree.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système

//------------------------------------------------------ Include personnel
#include "entree.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static pidvect tachesFilles;
static pid_t pidAttenteFinGarage;
static int PorteNum = -1;
static int canalDesc = -1;

//------------------------------------------------------ Fonctions privées

/**
 * Mode d'emploi / description : 
 * FinProgramme : fonction de terminaison de la tâche courante 
 * + handler du signal SIGUSR2 (signal de terminaison de la tâche courante !)
 * Mode d'emploi :
 * <signum> : Numéro de signal
 * 
 * Cette fonction se charge de la fermeture de toutes les ressources utilisées 
 * par la tâche courante ainsi que de la propagation du signal SIGUSR2 aux tâches filles
 * possiblement lancées par la tâche courante.
 * Une fois la phase de destruction de la tâche courante effectuée, cette fonction
 * termine la tâche courante.
 * 
 * *RAPPEL* : Cette tâche est susceptible d'être appelée à n'importe quel moment
 * par l'envoi de SIGUSR2 par la Mère.
 * C'est pourquoi on y prend certaines précautions.
 * 
 * Contrat :
 * <signum> : Ce paramètre est ignoré
 */
static void FinProgramme(int signum) {
#ifdef MAP
	std::ofstream f("entree_finprog.log", ios_base::app);
	f << "Lancement de FinProgramme pour Entrée" << PorteNum << std::endl;
	f << "Début du massacre des tâches filles." << std::endl;
#endif
	//* Dabord, on tue toutes les tâches filles : "propagation de la fin"
	/** Note importante : 
	 * Le vecteur utilisé ici est très susceptible de contenir des pid de tâches
	 * déjà terminées, le cas échéant, kill n'effectuera juste aucune action
	 * ce qui n'est pas un problème en soi....
	 */
	pidvect::iterator it;
	for (it = tachesFilles.begin(); it != tachesFilles.end(); ++it) {
		kill(*it, SIGUSR2);
		int ret = -1;
		waitpid(*it, &ret, 0);
#ifdef MAP
		f << "Tâche fille " << (*it) << "massacrée." << ((!ret) ? "correctement" : "pas correctement") << std::endl;
#endif
	}
#ifdef MAP
	f << "Fin du massacre des tâches filles." << std::endl;
	f << "FinProgramme (entrée=" << PorteNum << " début de fermeture des canaux" << std::endl;
#endif
	//* Si le canal a été ouvert correctement, on le ferme correctement aussi :
	if(canalDesc != -1) {
		close(canalDesc);
	}
	
#ifdef MAP
	f << "FinProgramme (entrée=" << PorteNum << "Fin de fermeture des canaux" << std::endl;
	f.close();
#endif
	exit(EXIT_CODE);
}

/**
 * Mode d'emploi / description : 
 * FinAttenteFinGarage : fonction de terminaison de la sous-tâche d'attente de fin de Garage courante 
 * lors de l'envoi du signal SIGUSR2 (signal de terminaison de la tâche courante !)
 * Mode d'emploi :
 * <signum> : Numéro de signal
 * 
 * Cette fonction se charge de la terminaison de la sous-tâche voiturier lancée.
 * 
 * *RAPPEL* : Cette tâche est susceptible d'être appelée à n'importe quel moment
 * par l'envoi de SIGUSR2 par Entree.
 * 
 * Contrat :
 * <signum> : Ce paramètre est ignoré
 */
static void FinAttenteFinGarage(int signum) {
	//* On tue la tâche fille puis on se termine :
#ifdef MAP
	std::ofstream f("entree_FinAttenteFinGarage.log", ios_base::app);
	f << "FinAttenteFinGarage: Lancement de FinAttenteFinGarage pour une Entrée (porte_num=" << PorteNum << "). Pid=" << pidAttenteFinGarage << std::endl;
#endif
	//* Terminaison du voiturier pour cette sous-tâche donnée :
	int ret = kill(pidAttenteFinGarage, SIGUSR2);
	
#ifdef MAP
	f << "FinAttenteFinGarage: Envoi de SIGUSR2 à " << pidAttenteFinGarage << " effectué. Code retour = " << ret << ". Errno=" << errno << std::endl;
	f.close();
#endif
	exit(EXIT_CODE);
}

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

/**
 * Mode d'emploi : cf. entree.h
 */
pid_t Garage(requete req, TypeBarriere barriere, voiture tuture, sem_t* semPtShmParking, requete* shmPtParking) {
#ifdef MAP
	std::ofstream f("entree_EntreeAttenteFinGarage.log", ios_base::app);
#endif
	//* Lancement d'une tâche fille d'attente de la fin du garage pour affichage
	pid_t noFille;
	if ((noFille = fork()) == 0) {
		//* CODE DE LA FILLE qui attend la fin de GarerVoiture
		unsigned char place = EntreeAttenteFinGarage(barriere, tuture.type, req.arrivee, req.plaque);
		//* On enregistre la voiture dans le parking : 
#ifdef MAP
		f << "ENTREE=" << PorteNum << " Début d'attente sur le séma Parking" << std::endl;
#endif
		//* Synchronisation d'accès à la mémoire partagée
		sem_wait(semPtShmParking);
#ifdef MAP
		f << "ENTREE=" << PorteNum << " Fin d'attente sur le séma Parking" << std::endl;
#endif
		//* Mise à jour de la mémoire partagée : informations sur les voitures garées dans le parking
		shmPtParking[place - 1] = req; //* Note : "place" commence à 1 alors que le tableau à 0 !
#ifdef MAP
		f << "ENTREE=" << PorteNum << " Début de débloquage du sémaphore sur le séma Parking" << std::endl;
#endif
		//* Synchronisation d'accès à la mémoire partagée
		sem_post(semPtShmParking);
#ifdef MAP
		f << "ENTREE=" << PorteNum << " Fin du débloquage sur le séma Parking" << std::endl;
#endif
		//* Terminaison de la TACHE FILLE
		exit(EXIT_CODE);
	} //* Fin if fork() / Fin CODE FILLE
	
	//* CODE TACHE COURANTE :
	//* On enregistre le pid de la tâche fille qu'on vient de créer, pour pouvoir 
	//* L'arrêter le cas échéant, et gérer les zombies
	tachesFilles.push_back(noFille);
	/** Synchronisation d'entrée des voitures 
	 * Le temps de prendre le ticket,
	 * Remonter la fenêtre de la voiture ou
	 * Refermer la portière (en fonction de sa longueur de bras !)
	 * De démarrer et d'évacuer !
	 * Cette synchro sert donc à éviter les collisions
	 */
	sleep(1);
	
	return noFille;
}

/**
 * Mode d'emploi : cf. entree.h
 */
unsigned char EntreeAttenteFinGarage(TypeBarriere barriere, TypeUsager usager, time_t arrivee, unsigned int numVoiture) {
#ifdef MAP
	std::ofstream f("entree_EntreeAttenteFinGarage.log", ios_base::app);
#endif
	//* Lancement du voiturier
	pid_t pidGarage = GarerVoiture(barriere);
	//* Si le lancement a échoué, on arrête tout :
	if (pidGarage == -1) {
#ifdef MAP
		f << "Erreur, GarerVoiture() a renvoyé '-1' en tant que PID pour la tâche fille." << std::endl;
#endif
		return 0;
	}
#ifdef MAP
	f << "EntreeAttenteFinGarage: Début d'une EntreeAttenteFinGarage sur le pid " << pidGarage << std::endl;
#endif
	//* Sinon, on stocke le pid pour un arrêt subit possible
	pidAttenteFinGarage = pidGarage;
	//* On enregistre le handler de fin sur SIGUSR2 :
	struct sigaction action;
	action.sa_handler = FinAttenteFinGarage;
	action.sa_flags = SA_RESTART;
	sigaction(SIGUSR2, &action, NULL);
	
#ifdef MAP
	f << "EntreeAttenteFinGarage : Début d'attente de la fin de la tâche fille GarerVoiture() ayant le pid " << pidGarage << std::endl;
#endif
	
	//* On attend la fin de la tâche fille GarerVoiture() :
	int st = -1;
	waitpid(pidGarage, &st, 0);
#ifdef MAP
		f << "EntreeAttenteFinGarage (pid=" << pidGarage << ") : Status reçu=" << st << std::endl;
#endif
	//* Transforming status to the place number :
	unsigned char place = (st & 0xFF00)>>8;//* Equivalent de WEXITSTATUS(st)
	//* Mais au moins ici on voit l'opération effectuée (mise à zéro du dernier octet + décalage vers la droite de 8 bits = récupération du premier octet)
	
#ifdef MAP
	f << "EntreeAttenteFinGarage : fin de la tâche fille " << pidGarage << " avec le status " << st << "correspondant à la place " << (int)place <<", lancement de l'affichage" << std::endl;
#endif
	
	//* Une fois celle-ci terminée, on affiche la place où elle s'est garée :
	AfficherPlace(place, usager, numVoiture, arrivee);
	
#ifdef MAP
	f.close();
#endif
	//* Et on retourne la place où elle s'est garée à l'appelant :
	return place;
}

/**
 * Mode d'emploi : cf. entree.h
 */
void Entree(int porte_num) {
#ifdef MAP
	//cout << "Pouet";
	std::stringstream fname_tmp;
	fname_tmp << "entree" << porte_num << ".log";
	std::string fname = fname_tmp.str();
	std::ofstream f(fname.c_str());
	f << "Lancement de Entrée avec porte_num = ";
	f << porte_num << std::endl;
#endif
	//* Enregistrement du numéro de porte de manière à ce que tout le monde puisse y accéder (notamment les ss-fonctions)
	PorteNum = porte_num;

	//*Association du signal SIGUSR2 à  la fin du programme
	//* Effectué avant même l'initialisation mais FinProgramme() prend des précautions !
	struct sigaction action;
	action.sa_handler = FinProgramme;
	action.sa_flags = SA_RESTART;
	sigaction(SIGUSR2, &action, NULL);

#ifdef MAP
	f << "DEBUT récupération des mémoires partagées" << std::endl;
#endif
	int shmIdRequetes = shmget(CLEF_REQUETES, sizeof (requete) * NB_PORTES, 0666 | 0);
	requete *shmPtRequetes = (requete*) shmat(shmIdRequetes, NULL, 0);
	int shmIdCompteur = shmget(CLEF_COMPTEUR, sizeof (compteur_t), 0666 | 0);
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
	for(;read(canalDesc, &tuture, sizeof (voiture)) > 0;) {//* Phase moteur
		//* Note importante : 
		//* si read() renvoit 0 => EOF() => plus d'écrivains => on arrête d'essayer de lire, fin de phase moteur
		//* si read() renvoit -1 => appel système en échec => cas non géré spécifiquement (cf. sujet !), ici on finit la phase moteur également
		DessinerVoitureBarriere(barriere, tuture.type);
#ifdef MAP
		f << "Entrée=" << PorteNum << " : Valeur lue sur le canal : voiture id=" << tuture.plaque << std::endl;
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