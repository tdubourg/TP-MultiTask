/*************************************************************************
                           Main.h  -  description
                             -------------------
    début                : 27 février 2012
    copyright            : (C) 2012 par Florent Devriese
    e-mail               : florent.devriese@insa-lyon.fr
*************************************************************************/

//---------- Interface de la tâche <Main> (fichier Main.h) -------------
//------------------------------Mere------------------------------------

//------------------------------------------------------------------------
// Rôle de la tâche <Main>
//    Cette tâche est chargée de la création de toutes
//    les ressources necessaires au bon fonctionnement
//    du programme, ainsi que des taches filles chargées
//    de les utiliser.
//    Elle se charge également de détruire toutes les ressources
//    et taches filles (envoi de SIGUSR2) lors de la reception
//    de la fin de programme. 
//------------------------------------------------------------------------

#ifndef MAIN_H
#define	MAIN_H

//////////////////////////////////////////////INCLUDE
/*-------------------------------------------- Includes systemes ----------------------------------------------- */

#include <cstdlib>
#include <time.h>
#include <unistd.h> 
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>

/*----------------------------------------- Includes personels ---------------------------------------------- */

#include "Heure.h"
#include "keyboard.h"
#include "entree.h"
#include "sortie.h"
#include "Config.h"

int main(int argc, char** argv);
// Mode d'emploi : Retour -1 en cas de soucis, 0 en cas d'execution normale
// Contrat : Les paramètres du main seront ignorés


#endif	/* MAIN_H */

