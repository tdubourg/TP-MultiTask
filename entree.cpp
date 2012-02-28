#include "entree.h"

void entree(int porte_num) {
    int shmIdRequetes = shmget(CLEF_REQUETES, sizeof (requete) * NB_PORTES, 0666 | 0);
    requete *shmPtRequetes = (requete*) shmat(shmIdRequetes, NULL, 0);
    int shmIdCompteur = shmget(CLEF_COMPTEUR, sizeof (int), 0666 | 0);
    int * shmPtCompteur = (int *) shmat(shmIdCompteur, NULL, 0);

    const char *cname;
    switch (porte_num) {
	case ENTREE_P:
	    cname = CANAL_KEY_ENTREE_BP_P;
	    break;
	case ENTREE_A:
	    cname = CANAL_KEY_ENTREE_BP_A;
	    break;
	case ENTREE_GB:
	    cname = CANAL_KEY_ENTREE_GB;
	    break;
    }
    int desc = open(cname, O_RDONLY);
    if (desc == -1) {//* L'ouverture du canal a échoué, on laisse tomber
	return;
    }
    int valeur;

#ifdef MAP
    std::stringstream fname_tmp;
    fname_tmp << "entree" << porte_num << ".log";
    std::string fname = fname_tmp.str();
    std::ofstream f(fname.c_str());
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

	} else {
	    requete req;
	    req.arrivee = (int) time(NULL);
	    req.type = (char) valeur;
	    shmPtRequetes[porte_num] = req;
	}
    }
#ifdef MAP
    f.close();
#endif
    close(desc);
}