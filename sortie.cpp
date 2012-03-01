#include "sortie.h"

using namespace std;

#ifdef MAP
std::ofstream f ("debug_sortie.log", ios_base::app);
#endif

static int canalKeySortie = -1;
static vector<int> noSorties;

static void FinProgramme (int signum){
#ifdef MAP
	f << "fin programme" << endl;
#endif
	vector<int>::iterator it;
	for(it = noSorties.begin (); it != noSorties.end (); ++it){
#ifdef MAP
		f << "envoie signal de kill" << endl;
#endif
		kill ((*it), SIGUSR2);
		int st = -1;
		do{
			waitpid ((*it), &st, 0);
		}while(st < 0);
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
	int noAffSortie;

	for(;;){
		for(; read (canalKeySortie, &place, sizeof(unsigned int)) <= 0;); //Bloc vide //On lis dans le canal tant qu'il y a des éléments à lire, sinon, on attend qu'il y en ai de nouveau	
		if((noAffSortie = fork ()) == 0){
			//Code du fils affichageSortie
			affichageSortie (place);
			exit (0);
		}
#ifdef MAP
		f << "filles de sortie :(affichage sortie) :" << noAffSortie << endl;
#endif
		if(noAffSortie > 0){
			noSorties.push_back (noAffSortie);
		}
	}

	//--------------------------------Destruction------------------------------------
}
