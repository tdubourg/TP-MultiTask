/*************************************************************************
                           Menu.h  -  description
                             -------------------
    début                : Mercredi 01 février 2012
    copyright            : (C) 2012 par Mathieu Maranzana
    e-mail               : Mathieu.Maranzana@insa-lyon.fr
*************************************************************************/

//---------- Interface du module <Menu> (fichier Menu.h) -----------------
#if ! defined MENU_H
#define MENU_H

//------------------------------------------------------------------------
// Rôle du module <Menu>
//    Ce module est en charge de la gestion du menu (interface homme - machine)
//    de l'application multitâche gérant le parking.
//    Les commandes autorisées sont :
//      1. e|E : déclenche la fin de l'application multitâche
//      2. p|P : déclenche l'arrivée d'un usager prof
//      3. a|A : déclenche l'arrivée d'un usager autre
//      4. s|S : déclenche la sortie d'un véhicule du parking
//    Ce module se charge aussi de la mise à jour de l'écran du TP
//    (zone <Commande> et <Message>).
//------------------------------------------------------------------------

////////////////////////////////////////////////////////////////// INCLUDE
//--------------------------------------------------- Interfaces utilisées

//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

/////////////////////////////////////////////////////////////////// PUBLIC
//---------------------------------------------------- Fonctions publiques
void Menu ( void );
// Mode d'emploi :
//    - les commandes autorisées sont :
//      1. e|E : déclenche la fin de l'application multitâche
//      2. p|P : saisie de l'arrivée d'un usager prof
//               un usager prof peut se présenter à la porte Blaise Pascal
//               (file PROFS) ou à la porte Gaston Berger (pas de file
//               particulière)
//               le choix de cette porte suit immédiatement le déclenchement
//               de l'arrivée d'un usager prof
//               en cas d'erreur de saisie du numéro de la porte, on réitère
//               la demande
//      3. a|A : saisie de l'arrivée d'un usager autre
//               un usager autre peut se présenter à la porte Blaise Pascal
//               (file AUTRES) ou à la porte Gaston Berger (pas de file
//               particulière)
//               le choix de cette porte suit immédiatement le déclenchement
//               de l'arrivée d'un usager autre
//               en cas d'erreur de saisie du numéro de la porte, on réitère
//               la demande
//      4. s|S : déclenchement de la sortie d'un véhicule du parking
//               ce déclenchement est suivi de la saisie du numéro de la place
//               de parking ([1,8], limite graphique) (choix de la voiture qui
//               quittera le parking)
//               bien évidemment, il est conseillé de choisir une place
//               actuellement occupée par un véhicule
//      toutes les autres commandes génèrent une erreur et déclenchent une
//      nouvelle saisie
//      pour les quatre commandes licites, le menu appelle la procédure
//      <Commande> en respectant la convention suivante pour les paramètres
//      (=> interfaçage possible avec le menu)
//        1. e|E : code = 'E' et valeur = 0
//        2. p|P : code = 'P' et valeur = numéro de la porte
//        3. a|A : code = 'A' et valeur = numéro de la porte
//        4. s|S : code = 'S' et valeur = numéro de la place
//      => le prototype imposé de <Commande> est donc le suivant :
//            void Commande ( char code, unsigned int valeur );
//
// Contrat : aucun
//


#endif // MENU_H
