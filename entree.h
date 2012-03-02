/*************************************************************************
                           entree  -  description
                             -------------------
    début                : 16/02/2012
    copyright            : (C) 2012-2042 par tdubourg
    e-mail               : theo.dubourg@insa-lyon.fr
*************************************************************************/

//---------- Interface du module <entree> (fichier entree.h) ---------

#ifndef ENTREE_H
#define	ENTREE_H


/////////////////////////////////////////////////////////////////// PUBLIC
//---------------------------------------------------- Fonctions publiques
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <signal.h>
#include <vector>
#include <sys/wait.h>

/*----------------------------------------- Includes personels ---------------------------------------------- */
#include "Config.h"
#include "Outils.h"

//------------------------------------------------------------------ Types
typedef std::vector<pid_t> pidvect;

#ifdef MAP
#include <fstream>
#endif


void Entree(int porte_num);
pid_t Garage(requete req, TypeBarriere barriere, voiture tuture, sem_t* semPtShmParking, requete* shmPtParking);
unsigned char EntreeAttenteFinGarage(TypeBarriere barriere, TypeUsager usager, time_t arrivee, unsigned int numVoiture);

#endif	/* ENTREE_H */

