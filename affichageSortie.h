/*************************************************************************
                           affichageSortie.h  -  description
                             -------------------
    début                : 27 février 2012
    copyright            : (C) 2012 par Florent Devriese
    e-mail               : florent.devriese@insa-lyon.fr
*************************************************************************/

//---------- Interface de la tâche <affichageSortie> (fichier affichageSortie.h) -------------

//------------------------------------------------------------------------
// Rôle de la tâche <affichageSortie>
//    Cette tâche est chargée d'afficher toutes les information liées à la sortie
//    d 'un véhicule grace à des appels provenant de Outils.
//    Elle se charge également de détruire toutes sa tache fille (envoi de SIGUSR2) 
//    lors de la reception de la fin de programme. 
//------------------------------------------------------------------------

#ifndef AFFICHAGESORTIE_H
#define	AFFICHAGESORTIE_H

//////////////////////////////////////////////INCLUDE
/*-------------------------------------------- Includes systemes ----------------------------------------------- */

#include <signal.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

/*----------------------------------------- Includes personels ---------------------------------------------- */

#include "Outils.h"
#include "Config.h"

void affichageSortie(unsigned int place);
// Mode d'emploi :
//    - cette fonction lance une tâche fille AfficherSortie() chargée de dessiner 
//      la voiture sortant du parking. La tâche <affichageSortie> attend la fin de
//      la tâche fille et efface la zone de texte correspondante à la place du 
//      parking venant de se libérer. Elle affiche également les information de la
//      sortie du véhicule (prix, durée...). 
//	Si un/des véhicule(s) sont en attente à une porte, elle se charge ensuite de débloquer
//      la tache entrée correspondante et d'éffacer la zone de texte correspondant à
//      la requête. 
//    - la fin de la tâche <affichageSortie> est déclenchée à la réception du
//      signal <SIGUSR2>
// Contrat : La place doit être comprise entre 1 et 8 (capacité du parking)
//


#endif	/* AFFICHAGESORTIE_H */

