/*************************************************************************
                           Config.h  -  description
                             -------------------
    début                : 27 février 2012
    copyright            : (C) 2012 par Florent Devriese & Théo Dubourg
    e-mail               : florent.devriese@insa-lyon.fr & theo.dubourg@insa-lyon.fr
 *************************************************************************/

//---------- Interface du fichier de configuration Config.h -----------
// ------- Note : Cette interface n'a pas de fichier de réalisation (.cpp) lié ---

#ifndef CONFIG_H
#define	CONFIG_H


/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include "Outils.h"

//------------------------------------------------------------- Constantes
//------------------------------------------------------------- Canaux
#define CANAL_KEY_ENTREE_GB "key_entree_GB.fifo"
#define ENTREE_GB 2
#define CANAL_KEY_ENTREE_BP_P "key_entree_BPP.fifo"
#define ENTREE_P 0
#define CANAL_KEY_ENTREE_BP_A "key_entree_BPA.fifo"
#define ENTREE_A 1
#define CANAL_KEY_SORTIE "key_sortie.fifo"

//------------------------------------------------------------- Sémaphores
#define SEM_ENTREE_GB "/entree_GB"
#define SEM_ENTREE_BP_P "/entree_BPP"
#define SEM_ENTREE_BP_A "/entree_BPA"
#define SEM_SHM_COMPTEUR "/compteur"
#define SEM_SHM_REQUETE "/requete"
#define SEM_SHM_PARKING "/parking"
#define NB_PORTES 3

//------------------------------------------------------------- Choix du menu
#define MENU_CHOICE_PROF_BP 1
#define MENU_CHOICE_PROF_GB 2
#define MENU_CHOICE_AUTRE_BP 1
#define MENU_CHOICE_AUTRE_GB 2

//------------------------------------------------------------- Mémoires partagées
#define CLEF_COMPTEUR 11111
#define CLEF_REQUETES 22222
#define CLEF_PARKING 33333

//------------------------------------------------------------- Constantes divers
#define CAPACITE_PARKING 8

#define MAX_PLAQUE 999

#define EXIT_CODE 0


//------------------------------------------------------------------ Types

/** Notes : les trois types suivants peuvent paraître peut utiles
 * Mais étant donné qu'ils sont utilisés pr gérer des informations au travers de 
 * différentes tâches dans différents fichiers
 * la taille de stockage des valeurs peut-être critique
 * en définissant un type, on s'assure que si on change la taille de stockage 
 * d'un valeur en mémoire, un jour
 * on aura un changement qui se répercutera sur tout le code de l'application via l'utilisation des
 * sizeof() sur le type custom défini ici.
 * On évite ainsi une source d'erreur difficile à détecter et qui peut n'apparaître que 
 * très rarement !
 */

//* Type d'un compteur de places libres dans le parking
typedef unsigned short int compteur_t;

//* Type d'un numéro de place dans le parking
typedef unsigned short int place_num_t;

//* Type d'un numéro minéralogique
typedef unsigned int plaque_t;

/** Type <requete> : Stockage d'informations relatives à un usager
 * ayant un numéro minéralogique
 * à un instant donné
 * Le type <requete> pourra être utilisé à n'importe quelles fins ayant besoin de 
 * regrouper ces informations
 */
typedef struct r
{
    TypeUsager type;
    time_t arrivee;
    short int plaque;
} requete;

/** Type voiture :
 * regroupe des informations sur un usager ayant une plaque minéralogique
 */
typedef struct v
{
    TypeUsager type;
    plaque_t plaque;
} voiture;

#ifdef MAP
//* Debugging purposes :
#include <iostream>
#include <fstream>
#include <sstream>
#include <errno.h>
#endif

#endif	/* CONFIG_H */

