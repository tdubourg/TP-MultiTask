/*************************************************************************
                           sortie.h  -  description
                             -------------------
    début                : 27 février 2012
    copyright            : (C) 2012 par Florent Devriese
    e-mail               : florent.devriese@insa-lyon.fr
*************************************************************************/

#ifndef SORTIE_H
#define	SORTIE_H

//////////////////////////////////////////////INCLUDE
/*-------------------------------------------- Includes systemes ----------------------------------------------- */

#include <signal.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>

/*----------------------------------------- Includes personels ---------------------------------------------- */

#include "Outils.h"
#include "Config.h"
#include "affichageSortie.h"

void Sortie();
// Mode d'emploi :
//    - cette fonction lance une tâche fille affichageSortie() chargée
//      d'afficher les différents composants lors de la sortie d'une
//      voiture. 
//    - la fin de la tâche <Sortie> est déclenchée à la réception du
//      signal <SIGUSR2>
// Contrat : aucun
//

#endif	/* SORTIE_H */

