#include "entree.h"
#include "main.h"

void entree(int porte_num, void* shmPtRequetes) {
    char *cname;
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

	requete req;
	req.arrivee = clock(); // @TODO : Check if that's correct
	req.type = (char) valeur;
	shmPtRequetes[porte_num] = req;
    }
#ifdef MAP
    f.close();
#endif
    close(desc);
}