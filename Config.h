/* 
 * File:   Config.h
 * Author: fhyren
 *
 * Created on February 27, 2012, 10:50 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

#define CANAL_KEY_ENTREE_GB "key_entree_GB.fifo"
#define ENTREE_GB 2
#define CANAL_KEY_ENTREE_BP_P "key_entree_BPP.fifo"
#define ENTREE_P 0
#define CANAL_KEY_ENTREE_BP_A "key_entree_BPA.fifo"
#define ENTREE_A 1
#define CANAL_KEY_SORTIE "key_sortie.fifo"
        
#define SEM_ENTREE_GB "/entree_GB"
#define SEM_ENTREE_BP_P "/entree_BPP"
#define SEM_ENTREE_BP_A "/entree_BPA"
#define SEM_SHM_COMPTEUR "/compteur"
#define SEM_SHM_REQUETE "/requete"
#define SEM_SHM_PARKING "/parking"
#define NB_PORTES 3

#define MENU_CHOICE_PROF_BP 1
#define MENU_CHOICE_PROF_GB 2
#define MENU_CHOICE_AUTRE_BP 1
#define MENU_CHOICE_AUTRE_GB 2

#define CLEF_COMPTEUR 11111
#define CLEF_REQUETES 22222
#define CLEF_PARKING 33333

#define CAPACITE_PARKING 8

#define MAX_PLAQUE 999

#define EXIT_CODE 0


#include "Outils.h"


typedef struct r
{
    TypeUsager type;
    int arrivee;
    short int plaque;
} requete ;


typedef struct v {
    unsigned int id;
    TypeUsager type;
    short int plaque;
} voiture;

#define MAP

#ifdef MAP
//* Debugging purposes :
#include <iostream>
#include <fstream>
#include <sstream>
#include <errno.h>
#endif

#endif	/* CONFIG_H */

