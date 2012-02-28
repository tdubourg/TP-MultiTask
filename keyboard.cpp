#include "keyboard.h"

using namespace std;

void keyboard() {
    for(;;) {
	Menu();
    }
}

void pousserVoitureVersEntree(char code, unsigned int valeur) {
#ifdef MAP
	    std::ofstream f("debug_kb_canalw.log");
	    f << "pousserVoitureVersEntree() lancée avec : code=" << code << ", valeur=" << valeur << std::endl;
#endif
    char *cname;

    switch (valeur) {
	case ENTREE_GB:
	    cname = CANAL_KEY_ENTREE_GB;
	    break;
	case ENTREE_P:
	    cname = CANAL_KEY_ENTREE_BP_P;
	    break;
	case ENTREE_A:
	    cname = CANAL_KEY_ENTREE_BP_A;
	    break;
    }

    // @TODO : Improve that (opening)
    int desc = open(cname, O_WRONLY);
    if (desc == -1) {//* L'ouverture du canal a échoué, on laisse tomber
	return;
    }
    write(desc, &valeur, sizeof (unsigned int));
    close(desc);
#ifdef MAP
    f.close();
#endif
}

void Commande(char code, unsigned int valeur) {
#ifdef MAP
	    std::ofstream f("debug_kb1.log");
	    f << "Commande() lancée avec : code=" << code << ", valeur=" << valeur << std::endl;
#endif
    switch (code) {
	case 'E':
	    exit(EXIT_CODE);
	    break;

	case 'P':
	case 'A':
	case 'S':
	    //* valeur = n° de la porte (0,1,2)
	    char* cname;
#ifdef MAP
	    f << "Commande reconnue, lancement de pousserVoitureVersEntree" << std::endl;
#endif
	    pousserVoitureVersEntree(code, valeur);
	    break;
    }
#ifdef MAP
    f.close();
#endif
}
