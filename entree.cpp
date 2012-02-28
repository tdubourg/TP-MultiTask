#include "entree.h"

void entree(int porte_num) {
#ifdef MAP
	std::stringstream fname_tmp;
	fname_tmp << "entree" << porte_num << ".log";
	std::string fname = fname_tmp.str();
	std::ofstream f(fname.c_str());
	f << "Lancement de Entrée avec porte_num = " << porte_num;
#endif
	
	int shmIdRequetes = shmget(CLEF_REQUETES, sizeof (requete) * NB_PORTES, 0666 | 0);
	requete *shmPtRequetes = (requete*) shmat(shmIdRequetes, NULL, 0);
	int shmIdCompteur = shmget(CLEF_COMPTEUR, sizeof (int), 0666 | 0);
	int * shmPtCompteur = (int *) shmat(shmIdCompteur, NULL, 0);
	sem_t* semPtShmCompteur;
	sem_t* semPtEntree;

	//* On essaie d'ouvrir le sema, si ça marche pas, on réessaie ! Car de toutes façons, le programme ne peut pas fonctionner
	//* Sans ce sémaphore, attendons donc que qq'un l'initialise !
	while ((semPtShmCompteur = sem_open(SEM_SHM_COMPTEUR, 0, 0666, 0)) != SEM_FAILED);

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
	while ((semPtEntree = sem_open(sem_key, 0, 0666, 0)) != SEM_FAILED);


	int desc = open(cname, O_RDONLY);
	if (desc == -1) {//* L'ouverture du canal a échoué, on laisse tomber
		return;
	}
	int valeur;

#ifdef MAP
		f << "Entrée : Debut de lecture du canal" << std::endl;
#endif
		
	while (read(desc, &valeur, sizeof (unsigned int))) {
#ifdef MAP
		f << "Valeur lue sur le canal :" << valeur << std::endl;
#endif
		if ((*shmPtCompteur) > 0) {
			//* Y'a de la place, on se gare :
			//* On décrémente le compteur avant :
			//* Avec gestion du sémaphore
			sem_wait(semPtShmCompteur);
			*shmPtCompteur -= 1; //* On décrémente le nb de places dispo !
			sem_post(semPtShmCompteur);
			GarerVoiture(barriere);
		} else {//* Le parking est plein :
			//* On formule une requête d'entrée :
			requete req;
			req.arrivee = (int) time(NULL);
			req.type = (char) valeur;
			shmPtRequetes[porte_num] = req;
			//* Puis on attends l'autorisation de faire entrer la voiture :
			sem_wait(semPtEntree);
		}
	}
#ifdef MAP
	f.close();
#endif
	close(desc);
}