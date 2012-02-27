#include "sortie.h"

using namespace std;


static void FinProgramme(int signum)
{
    
}

void Sortie()
{
	
	struct sigaction action;
	action.sa_handler = FinProgramme;
	sigaction(SIGUSR2, &action, NULL);
	
	
}