/*************************************************************************
                           Heure.h  -  description
                             -------------------
    début                : Mercredi 01 février 2012
    copyright            : (C) 2012 par Mathieu Maranzana
    e-mail               : Mathieu.Maranzana@insa-lyon.fr
*************************************************************************/

//---------- Interface de la tâche <Heure> (fichier Heure.h) -------------
#if ! defined HEURE_H
#define HEURE_H

//------------------------------------------------------------------------
// Rôle de la tâche <Heure>
//    Cette tâche est chargée de l'affichage périodique (toutes les
//    <PERIODE> secondes (on pose <PERIODE> = 1 seconde)) de l'heure
//    courante dans la zone adéquate de l'écran du TP (en haut, à droite).
//    La fin de la tâche <Heure> est déclenchée à la réception du
//    signal <SIGUSR2>.
//------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques
pid_t ActiverHeure ( void );
// Mode d'emploi :
//    - cette fonction lance une tâche fille <Heure> chargée de
//      l'affichage périodique (toutes les <PERIODE> secondes) de l'heure
//      courante dans la zone <HEURE> de l'écran du TP
//    - la fin de la tâche <Heure> est déclenchée à la réception du
//      signal <SIGUSR2>
//    - renvoie le PID de la tâche <Heure>, si sa création se passe bien
//    - renvoie -1 en cas d'échec à la création de la tâche <Heure>
//
// Contrat : aucun
//


#endif // HEURE_H
