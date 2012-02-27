#include "sortie.h"

using namespace std;


static void FinProgramme(int signum)
{
    
}

void Sortie()
{	
    
    //--------------------------------Initialisation------------------------------------
    
    bool error = false;
    
    //Association du signal SIGUSR2 à  la fin du programme
    struct sigaction action;
    action.sa_handler = FinProgramme;
    sigaction(SIGUSR2, &action, NULL);
    
    //Ouverture du canal de communication 
    if (open (CANAL_KEY_SORTIE, O_RDONLY) == -1)
    {
	error = true;
    }
    
    //------------------------------------Moteur---------------------------------------
    
    //--------------------------------Destruction------------------------------------
}