/*************************************************************************
                           Outils.h  -  description
                             -------------------
    début                : Mercredi 01 février 2012
    copyright            : (C) 2012 par Mathieu Maranzana
    e-mail               : Mathieu.Maranzana@insa-lyon.fr
*************************************************************************/

//---------- Interface des fonctions Outils (fichier Outils.h) -----------
#if ! defined OUTILS_H
#define OUTILS_H

//------------------------------------------------------------------------
// Rôle du module <Outils>
//    Ce module prend en charge la partie graphique de l'application
//    multitâche en gérant complètement le clavier et l'écran.
//------------------------------------------------------------------------

////////////////////////////////////////////////////////////////// INCLUDE
//--------------------------------------------------- Interfaces utilisées
#include <cstdlib>
using namespace std;

//------------------------------------------------------------- Constantes
// constantes de l'application multitâche
//  1. nombre de barrières manipulées
//     a. porte Blaise Pascal côté usager prof (entrée)
//     b. porte Blaise Pascal côté usager autre (entrée)
//     c. porte Gaston Berger côté usager prof ou autre (entrée)
//     d. porte Gaston Berger sortie usager prof ou autre (sortie)
const unsigned int NB_BARRIERES_ENTREE = 3;
const unsigned int NB_BARRIERES_SORTIE = 1;
const unsigned int NB_BARRIERES = NB_BARRIERES_ENTREE + NB_BARRIERES_SORTIE;

//  2. nombre de places disponibles dans le parking (contraintes graphiques)
const unsigned int NB_PLACES = 8;

//------------------------------------------------------------------ Types
// définition des modes vidéos supportés lors des différents affichages
// (les deux modes ne sont pas systématiquement disponibles)
enum ModeVideo { NORMALE, INVERSE };

// définition des noms symboliques pour les <NB_BARRIERES> barrières de
// l'application multitâche
enum TypeBarriere {
  AUCUNE,               // 0 : pas de porte du parking
  PROF_BLAISE_PASCAL,   // 1 : entrée du parking pour les profs
  AUTRE_BLAISE_PASCAL,  // 2 : entrée du parking pour les autres
  ENTREE_GASTON_BERGER, // 3 : entrée du parking pour les profs ou autres
  SORTIE_GASTON_BERGER  // 4 : sortie du parking
};

// définition des différentes catégories d'usagers gérées par l'application
enum TypeUsager { AUCUN, PROF, AUTRE };

// définition des types de terminaux supportés pour l'exécution de
// l'application multitâche
//    1. XTERM : terminal X par défaut
//    2. VT220 : terminal pour les connexions réseau (via ssh)
enum TypeTerminal { XTERM, VT220 };

// définition des zones particulières de l'écran (affichage et effacement)
enum TypeZone {
  HEURE,      // zone d'affichage de l'heure (1 ligne)
  ETAT_P1,    // zone des caractéristiques (type, num, arrivée) de la place #1
  ETAT_P2,    // zone des caractéristiques (type, num, arrivée) de la place #2
  ETAT_P3,    // zone des caractéristiques (type, num, arrivée) de la place #3
  ETAT_P4,    // zone des caractéristiques (type, num, arrivée) de la place #4
  ETAT_P5,    // zone des caractéristiques (type, num, arrivée) de la place #5
  ETAT_P6,    // zone des caractéristiques (type, num, arrivée) de la place #6
  ETAT_P7,    // zone des caractéristiques (type, num, arrivée) de la place #7
  ETAT_P8,    // zone des caractéristiques (type, num, arrivée) de la place #8
  REQUETE_R1, // zone d'affichage requête (type, demande) Entrée B-P Prof
  REQUETE_R2, // zone d'affichage requête (type, demande) Entrée B-P Autre
  REQUETE_R3, // zone d'affichage requête (type, demande) Entrée G-B
  MESSAGE,    // zone d'affichage des messages d'erreurs (1 ligne, 78 car.)
  MENU        // zone d'affichage des différents menus pour la simulation
};


/////////////////////////////////////////////////////////////////// PUBLIC
//---------------------------------------------------- Fonctions publiques
void Afficher ( TypeZone zone, int valeur, ModeVideo mode = NORMALE );
// Mode d'emploi :
//    <zone>   : zone de l'écran qui va subir l'affichage
//               (MESSAGE, MENU, ETAT_Pi, REQUETE_Rx...)
//    <valeur> : contient la valeur entière à afficher dans la zone adéquate
//    <mode>   : contient le mode vidéo d'affichage de la valeur entière
//               (par défaut, le mode d'affichage est normal)
//    - affiche l'entier (<valeur>) selon le mode choisi (<mode>) dans
//      la bonne zone de l'écran (<zone>)
//
// Contrat :
//    - l'affichage de la valeur entière ne doit pas dépasser la taille de
//      la zone utilisée pour cet affichage
//    - le comportement est indéterminé en cas de non-respect de la contrainte
//


void Afficher ( TypeZone zone, const char message[], ModeVideo mode = NORMALE );
// Mode d'emploi :
//    <zone>    : zone de l'écran qui va subir l'affichage
//                (MESSAGE, MENU, ETAT_Pi, REQUETE_Rx...)
//    <message> : contient le message sous forme d'une chaîne de caractères
//                en convention C/C++
//    <mode>    : contient le mode vidéo d'affichage du message à l'écran
//                (par défaut, le mode d'affichage est normal)
//    - affiche le message (<message>) selon le mode choisi (<mode>) dans
//      la bonne zone de l'écran (<zone>)
//
// Contrat :
//    - le message doit être une chaîne de caractères au sens du C/C++ et ne
//      pas dépasser la taille de la zone utilisée pour l'affichage
//    - le comportement est indéterminé en cas de non-respect de ces contraintes
//


void AfficherPlace ( unsigned int numPlace, TypeUsager usager,
                     unsigned int numVoiture, time_t arrivee );
// Mode d'emploi :
//    <numPlace>  : numéro de la place de parking utilisée pour garer la voiture
//    <usager>    : type de l'usager dont la voiture vient de se garer
//                  (indispensable pour la porte Gaston Berger avec une entrée
//                  unique)
//        * PROF  : pour un usager prof
//        * AUTRE : pour un usager autre
//    <numVoiture>: numéro de la voiture de l'usager
//    <arrivee>   : instant d'arrivée de la voiture dans le parking
//    (nombre de secondes depuis le 1er janvier 1970 00:00:00 UTC (Epoch))
//    - affiche les caractéristiques de la voiture stationnée (fin de la tâche
//      fille lancée par la fonction <GarerVoiture>)
//        * type    : à partir du paramètre <type>
//        * num     : à partir du paramètre <numVoiture>
//        * arrivée : à partir du paramètre <arrivée> en utilisant tous les
//                    chiffres significatifs disponibles
//    - l'affichage doit rester actif aussi longtemps que la voiture est
//      présente dans le parking
//
// Contrat : aucun
//


void AfficherRequete ( TypeBarriere barriere, TypeUsager usager,
                       time_t demande );
// Mode d'emploi :
//    <barriere> : barrière d'arrivée pour l'usager
//        * PROF_BLAISE_PASCAL   : porte Blaise Pascal pour un usager prof
//        * AUTRE_BLAISE_PASCAL  : porte Blaise Pascal pour un usager autre
//        * ENTREE_GASTON_BERGER : porte Gaston Berger pour un usager prof
//                                 ou autre
//    <usager>   : type de l'usager qui vient d'arriver à la barrière
//                 (indispensable pour la porte Gaston Berger avec une entrée
//                 unique)
//        * PROF  : pour un usager prof
//        * AUTRE : pour un usager autre
//    <demande> : instant d'arrivée pour l'usager à la barrière
//    (nombre de secondes depuis le 1er janvier 1970 00:00:00 UTC (Epoch))
//    - affiche la requête d'entrée dans le parking pour l'usager de type
//      <usager> arrivé à la barrière identifiée par <barriere>
//    - compte tenu de la taille restreinte de la zone d'affichage, la date
//      de demande d'entrée dans le parking (<demande>) ne comporte que 5
//      chiffres significatifs (les cinq derniers) (représentation possible
//      de plus de 27h de stationnement, ce qui est suffisant pour les
//      besoins du TP)
//
// Contrat : aucun
//


void AfficherSortie ( TypeUsager usager, unsigned int numVoiture,
                      time_t arrivee, time_t depart );
// Mode d'emploi :
//    <usager>     : type de l'usager (PROF, AUTRE) quittant le parking
//    <numVoiture> : numéro de la voiture de l'usager
//    <arrivee>    : instant d'arrivée de la voiture dans le parking
//    (nombre de secondes depuis le 1er janvier 1970 00:00:00 UTC (Epoch))
//    <depart>     : instant de départ de la voiture du parking
//    (nombre de secondes depuis le 1er janvier 1970 00:00:00 UTC (Epoch))
//    - affiche les caractéristiques type, numéro, arrivée, départ, durée
//      et coût du stationnement pour la voiture qui vient de quitter le
//      parking au niveau de la sous-zone "Sortie" de la zone "Etat" de l'écran
//    - la durée et le coût du stationnement sont calculés par cette procédure
//    - cet affichage reste actif aussi longtemps qu'une nouvelle voiture
//      ne quitte pas le parking
//
// Contrat : aucun
//


void DessinerVoitureBarriere ( TypeBarriere barriere, TypeUsager usager );
// Mode d'emploi :
//    <barriere> : barrière d'arrivée pour l'usager
//        * PROF_BLAISE_PASCAL   : porte Blaise Pascal pour un usager prof
//        * AUTRE_BLAISE_PASCAL  : porte Blaise Pascal pour un usager autre
//        * ENTREE_GASTON_BERGER : porte Gaston Berger pour un usager prof
//                                 ou autre
//    <usager>   : type de l'usager qui vient d'arriver à la barrière
//                 (indispensable pour la porte Gaston Berger avec une entrée
//                 unique)
//        * PROF  : pour un usager prof
//        * AUTRE : pour un usager autre
//    - vérifie la "disponibilité" des barrières (=> évite les collisions)
//    - dessine la voiture, avec le dessin adéquat 'P' ou 'A' suivant le
//      type de l'usager (<usager>), au niveau de la barrière définie par le
//      paramètre <barriere>
//    - une temporisation de <TEMPO> secondes marque l'arrivée d'un nouvel
//      usager... c'est le temps que met l'usager pour descendre sa vitre
//      et faire sa demande de ticket...
//
// Contrat : aucun
//


void Effacer ( TypeZone zone );
// Mode d'emploi :
//    <zone> : zone de l'écran qui va subir l'effacement
//             (MESSAGE, MENU, ETAT_Pi, REQUETE_Rx...)
//    - efface la zone correspondante à l'écran
//
// Contrat : aucun
//


pid_t GarerVoiture ( TypeBarriere barriere );
// Mode d'emploi :
//    <barriere>   : barrière de départ pour le garage de la voiture
//        * PROF_BLAISE_PASCAL   : porte Blaise Pascal pour un usager prof
//        * AUTRE_BLAISE_PASCAL  : porte Blaise Pascal pour un usager autre
//        * ENTREE_GASTON_BERGER : porte Gaston Berger pour un usager prof
//                                 ou autre
//    - Sélectionne une place de parking pour la voiture (si elle existe)
//    - Si une place de parking libre existe, lance une tâche fille pour
//      gérer le garage de la voiture dans la place de parking
//      => L'appelant n'est pas bloqué pendant cette opération et peut
//      continuer normalement ses activités (par exemple, lancement d'un
//      autre garage de voiture)
//    - La synchronisation de fin avec la tâche fille peut se faire via le
//      signal <SIGCHLD>, automatiquement déclenché vers le père à la mort d'un
//      fils
//    - La valeur de retour de la tâche fille est le numéro de la place
//      que vient d'occuper la voiture, codé sur un octet [0,255]
//    - La tâche fille de garage d'une voiture est susceptible de réagir
//      au signal <SIGUSR2> en cas de fin de l'application (utilisation de
//      la touche 'e' ou 'E')
//      A la réception de ce signal, la tâche fille de garage déclenche
//      sa fin propre
//    - Renvoie le PID de la tâche fille gérant le garage d'une voiture dans
//      le parking, si elle est correctement lancée
//    - Renvoie -1 en cas de problème
//
// Contrat : aucun
//


void InitialiserApplication ( TypeTerminal terminal );
// Mode d'emploi :
//    <terminal> : indique le type de terminal utilisé pour l'exécution
//                 de l'application multitâche
//      * XTERM : terminal de type XTERM (terminal X par défaut)
//      * VT220 : terminal de type VT220 (connexion réseau via ssh)
//    - modifie la gestion de la ligne de transmission et prépare l'écran
//      pour les besoins du TP :
//         1. suppression de l'écho
//         2. retour immédiat après lecture d'un caractère (lecture au vol)
//         3. curseur en mode invisible
//         4. effacement de l'écran (curseur en (0,0) en haut et à gauche
//    - initialise les paramètres annexes (générateur aléatoire)
//      indispensable au bon fonctionnement du TP
//    - initialise les attributs graphiques de l'écran
//    - met en place les ressources systèmes pour la gestion graphique
//        * un sémaphore général avec 2 sémaphores élémentaires :
//           . sem 0 : mutex de gestion des conflits d'accès à l'écran
//           . sem 1 : mutex de gestion des conflits d'accès à la mémoire
//                     partagée utilisée pour définir les positions des voitures
//        * une zone de mémoire partagée pour la mémorisation des positions
//          des voitures dans le parking
// ATTENTION: en cas de "plantage" de l'application multitâche (sortie de
//            l'application sans passer par <TerminerApplication>), les
//            ressources systèmes (sémaphore et mémoire partagée) ne seront
//            pas rendues au système
//         => une destruction manuelle est OBLIGATOIRE
//    - trace le parking et son environnement => prépare l'écran pour le TP
//
// Contrat : aucun
//


pid_t SortirVoiture ( unsigned int numPlace );
// Mode d'emploi :
//    <numPlace> : numéro de la place de parking concerné par le départ de la
//                 voiture
//    - vérifie l'existence d'une voiture pour la place de parking mentionnée
//    - si une voiture est présente, lance une tâche fille pour gérer la
//      sortie de la voiture du parking => l'appelant n'est pas bloqué pendant
//      cette opération et peut continuer normalement ses activités
//      (par exemple, lancement d'une autre sortie de voiture)
//    - la synchronisation de fin avec la tâche fille peut se faire via le
//      signal <SIGCHLD> automatiquement envoyé vers le père à la mort d'un fils
//    - la valeur de retour de la tâche fille est le numéro de la place de
//      parking qui vient d'être libérée
//    - la tâche fille de gestion de la sortie d'une voiture du parking est
//      susceptible de réagir au signal <SIGUSR2> en cas de fin de
//      l'application (utilisation de la touche 'e' ou 'E')
//      à la réception de ce signal, la tâche fille de gestion de la sortie
//      d'une voiture du parking déclenche sa fin propre
//    - renvoie le PID de la tâche fille gérant la sortie d'une voiture du
//      parking, si elle est correctement lancée
//    - renvoie -1 en cas de problème
//
// Contrat : aucun
//


void TerminerApplication ( bool efface = true );
// Mode d'emploi :
//    <efface> : demande d'effacement (ou de conservation) de l'écran en
//               fin d'exécution
//    - si <efface> = <true>, alors l'écran est effacé en fin d'exécution
//      sinon, le dessin est conservé (très utile en phase de mise au point)
//    - restaure l'état initial de la ligne de transmission et de l'écran
//         1. traitement normal des lectures au clavier ("bufférisées")
//         2. curseur en mode visible
//         3. effacement de l'écran (curseur en (0,0) en haut et à gauche)
//    - restitue les ressources systèmes (sémaphore, mémoire partagée)
//      utilisées pour la gestion de l'écran (gestion des E/S du parking)
//
// Contrat : aucun
//


#endif // OUTILS_H

