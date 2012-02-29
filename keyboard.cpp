#include "keyboard.h"

using namespace std;

static void FinProgramme(int signum) {
	exit(EXIT_CODE);
}

void keyboard() {
	//Association du signal SIGUSR2 à la fin du programme
	struct sigaction action;
	action.sa_handler = FinProgramme;
	sigaction(SIGUSR2, &action, NULL);
	for (;;) {
		Menu();
	}
}

void pousserVoitureVersEntree(TypeUsager usager, unsigned int valeur) {
	static unsigned int voituresId = 0;
#ifdef MAP
	std::ofstream f("debug_kb_canalw.log");
	f << "pousserVoitureVersEntree() lancée avec : valeur=" << valeur << std::endl;
#endif
	const char *cname;

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
#ifdef MAP
	f << "Debut d'ouverture du canal" << std::endl;
#endif
	int desc = open(cname, O_WRONLY);
#ifdef MAP
	f << "Fin d'ouverture du canal" << std::endl;
#endif
	if (desc == -1) {//* L'ouverture du canal a échoué, on laisse tomber
		return;
	}
#ifdef MAP
	f << "Debut d'écriture sur le canal" << std::endl;
#endif
	voiture tuture;
	tuture.id = ++voituresId;
	tuture.type = usager;
	write(desc, &tuture, sizeof (voiture));
#ifdef MAP
	f << "Fin d'écriture sur le canal" << std::endl;
#endif
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
	TypeUsager usager = AUCUN;
	switch (code) {
		case 'E':
#ifdef MAP
			f << "Exiting keyboard !" << endl;
#endif
			exit(EXIT_CODE);
			break;

		case 'P':
			usager = PROF;
		case 'A':
			if(usager == AUCUN) {//* Sert à ce que, quand c'est "P" la valeur ne soit pas écrasée
				usager = AUTRE;
			}
			//* valeur = n° de la porte (0,1,2)
#ifdef MAP
			f << "Commande reconnue, lancement de pousserVoitureVersEntree" << std::endl;
#endif
			pousserVoitureVersEntree(usager, valeur);
			break;
		case 'S':
			break;
	}
#ifdef MAP
	f.close();
#endif
}
