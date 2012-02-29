#include "sortie.h"

using namespace std;


static int noAffSortie;

static void FinProgramme(int signum) {
	kill(noAffSortie, SIGUSR2);
	exit(0);
}

void Sortie() {

	//--------------------------------Initialisation------------------------------------

	bool error = false;

	//Association du signal SIGUSR2 à  la fin du programme
	struct sigaction action;
	action.sa_handler = FinProgramme;
	sigaction(SIGUSR2, &action, NULL);

	//Ouverture du canal de communication en lecture
	int canalKeySortie = open(CANAL_KEY_SORTIE, O_RDONLY);
	if (canalKeySortie == -1) {
		error = true;
	}

	//------------------------------------Moteur--------------------------------------- 
	unsigned int place;
	//int noAffSortie;

	for (;;) {
		read(canalKeySortie, &place, sizeof (unsigned int)); //On lis dans le canal tant qu'il y a des éléments à lire, sinon, on attend qu'il y en ai de nouveau

		if ((noAffSortie = fork()) == 0) {
			//Code du fils affichageSortie
			affichageSortie(place);
			exit(0);
		}

	}

	//--------------------------------Destruction------------------------------------
}
