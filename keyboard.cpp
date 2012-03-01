#include "keyboard.h"

static int descGB;
static int descBPP;
static int descBPA;
static int descS;
static unsigned short int NextVoitureId = 1;

using namespace std;

static void FinProgramme(int signum) {
	if(descGB != -1) {
		close(descGB);
	}
	if(descBPP != -1) {
		close(descBPP);
	}
	if(descBPA != -1) {
		close(descBPA);
	}
	if(descS != -1) {
		close(descS);
	}
	exit(EXIT_CODE);
}


void keyboard() {
#ifdef MAP
	std::ofstream f("keyboard.log", ios_base::app);
	f << "Début de keyboard" << std::endl;
#endif
	//Association du signal SIGUSR2 à la fin du programme
	struct sigaction action;
	action.sa_handler = FinProgramme;
	sigaction(SIGUSR2, &action, NULL);
#ifdef MAP
	f << "Debut d'ouverture des canaux" << std::endl;
#endif
	descGB = open(CANAL_KEY_ENTREE_GB, O_WRONLY);
	descBPP = open(CANAL_KEY_ENTREE_BP_P, O_WRONLY);
	descBPA = open(CANAL_KEY_ENTREE_BP_A, O_WRONLY);
	descS = open(CANAL_KEY_SORTIE, O_WRONLY);
#ifdef MAP
	f << "Fin d'ouverture des canaux" << std::endl;
#endif
#ifdef MAP
	f << "Debut d'écriture sur le canal" << std::endl;
#endif
	for (;;) {
		Menu();
	}
	//* If, for any reason, we get here, make sure everything closes properly :
	FinProgramme(0);
}

void pousserVoitureVersSortie(unsigned int valeur) {
#ifdef MAP
	std::ofstream f("debug_kb_pousserSortie.log");
	f << "pousserVoitureVersSortie() lancée avec : valeur=" << valeur << std::endl;
#endif
#ifdef MAP
	f << "Debut d'écriture sur le canal" << std::endl;
#endif
	write(descS, &valeur, sizeof (unsigned int));
#ifdef MAP
	f << "Fin d'écriture sur le canal" << std::endl;
#endif
#ifdef MAP
	f.close();
#endif
}

void pousserVoitureVersEntree(TypeUsager usager, TypeBarriere valeur) {
	static unsigned int voituresId = 0;
#ifdef MAP
	std::ofstream f("debug_kb_canalw.log");
	f << "pousserVoitureVersEntree() lancée avec : valeur=" << valeur << std::endl;
#endif
	int canalDesc;

	if(usager == PROF) {
		if(valeur == PROF_BLAISE_PASCAL) {
			canalDesc = descBPP;
		} else {
			canalDesc = descGB;
		}
	} else {
		if(valeur == AUTRE_BLAISE_PASCAL) {
			canalDesc = descBPA;
		} else {
			canalDesc = descGB;
		}
	}

	voiture tuture;
	tuture.id = ++voituresId;
	tuture.type = usager;
	tuture.plaque = NextVoitureId++;//* @warning : NextVoitureId is incremented here
	write(canalDesc, &tuture, sizeof (voiture));
#ifdef MAP
	f << "Fin d'écriture sur le canal" << std::endl;
#endif
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
	TypeBarriere barriere = AUCUNE;
	switch (code) {
		case 'E':
#ifdef MAP
			f << "Exiting keyboard !" << endl;
#endif
			exit(EXIT_CODE);
			break;

		case 'P':
			usager = PROF;
			if(valeur == MENU_CHOICE_PROF_BP) {
				barriere = PROF_BLAISE_PASCAL;
			} else {
				barriere = ENTREE_GASTON_BERGER;
			}
		case 'A':
			if (usager == AUCUN) {//* Sert à ce que, quand c'est "P" la valeur ne soit pas écrasée
				usager = AUTRE;
				if(valeur == MENU_CHOICE_AUTRE_BP) {
					barriere = AUTRE_BLAISE_PASCAL;
				} else {
					barriere = ENTREE_GASTON_BERGER;
				}
			}
			//* valeur = n° de la porte (0,1,2)
#ifdef MAP
			f << "Commande reconnue, lancement de pousserVoitureVersEntree" << std::endl;
#endif
			pousserVoitureVersEntree(usager, barriere);
			break;
		case 'S':
			pousserVoitureVersSortie(valeur);
			break;
	}
#ifdef MAP
	f.close();
#endif
}
