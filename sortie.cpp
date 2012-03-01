#include "sortie.h"

using namespace std;

#ifdef MAP
std::ofstream f ("debug_main.log");
#endif

static int noAffSortie = -1;
static int canalKeySortie = -1;

static void FinProgramme (int signum){
	if(noAffSortie != -1){
		kill (noAffSortie, SIGUSR2);
	}
	if(canalKeySortie != -1){
		close (canalKeySortie);
	}
	exit (EXIT_CODE);
}

void Sortie (){

	//--------------------------------Initialisation------------------------------------

	bool error = false;

	//Association du signal SIGUSR2 à  la fin du programme
	struct sigaction action;
	action.sa_handler = FinProgramme;
	sigaction (SIGUSR2, &action, NULL);

	//Ouverture du canal de communication en lecture
	canalKeySortie = open (CANAL_KEY_SORTIE, O_RDONLY);
	if(canalKeySortie == -1){
		error = true;
	}

	//------------------------------------Moteur--------------------------------------- 
	unsigned int place;

	for(;;){
		for(;read (canalKeySortie, &place, sizeof(unsigned int)) == 0;);//Bloc vide //On lis dans le canal tant qu'il y a des éléments à lire, sinon, on attend qu'il y en ai de nouveau	
			if((noAffSortie = fork ()) == 0){
				//Code du fils affichageSortie
				affichageSortie (place);
				exit (0);
			}
	}

	//--------------------------------Destruction------------------------------------
}
