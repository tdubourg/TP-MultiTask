/*************************************************************************
                           keyboard  -  description
                             -------------------
    début                : 16/02/2012
    copyright            : (C) 2012-2042 par tdubourg
    e-mail               : theo.dubourg@insa-lyon.fr
 *************************************************************************/

//---------- Interface du module <keyboard> (fichier keyboard.h) ---------

#ifndef KEYBOARD_H
#define	KEYBOARD_H

/**
 * Rôle du module <keyboard>
 *      Le module keyboard sert à faire l'interface entre l'utilisateur et l'application.
 *      Ce module utilise notamment la fonction Menu() du module <Outils>
 *      Le module keyboard, après avoir récupéré une saisie de l'utilisateur, se charge de
 *      faire transiter les informations jusqu'au bon destinataire :
 *              * Sortie, si la commande était une demande de sortie du parking
 *              * (une des) Entrée si la commande était une demande d'entrée dans le parking
 *      C'est notamment ce module qui s'occupe de l'assignation des numéros de plaque des voitures.
 *      Ce module est disponible à tout moment afin de répondre à une sollicitation de l'utilisateur.
 *      Sa phase moteur est ainsi en permanence active.
 *      C'est ce module qui, sur ordre de l'utilisateur, provoquera la fermeture de l'application
 *      par sa terminaison (que la tâche Mère (main) attend).
 *
 */

////////////////////////////////////////////////////////////////// INCLUDE
//--------------------------------------------------- Interfaces utilisées

/*-------------------------------------------- Includes systemes ----------------------------------------------- */

#include <stdlib.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

using namespace std;


/*----------------------------------------- Includes personels ---------------------------------------------- */

#include "Menu.h"
#include "Config.h"


/////////////////////////////////////////////////////////////////// PUBLIC
//---------------------------------------------------- Fonctions publiques
void Keyboard ( );
/** Mode d'emploi :
 *      Cette fonction gère l'ensemble du module keyboard
 *      Pour avoir plus de détails sur l'algorithme du module <Keyboard> : cf. le commentaire en début de keyboard.h
 *      En pratique, Keyboard() n'assurera que le lancement de Menu(), fonction qui lancera à son tour Commande() 
 *      afin d'effectuer les traitements spécifiques à chaque demande de l'utilisateur
 *
 * Contrat : Si Keyboard() rencontre un souci lors de son initialisation, la tâche quittera
 * après avoir affiché un message dans la zone d'affichage des messages de l'application.
 */

void Commande ( char code, unsigned int valeur );
/** Mode d'emploi :
 *      Cette fonction est appelée par Menu() (cf. module <Outils>)
 *      <code> : Choix tapé par l'utilisateur pour le Menu
 *      <valeur> : Choix tapé par l'utilisateur pour le sous-menu
 *      
 *      Cette fonction se charge, à partir des paramètres passés par Menu()
 *      de lancer, les cas échéants, pousserVoitureVersSortie() ou 
 *      pousserVoitureVersEntree() avec le bon paramètrage.
 *      
 * 
 * Contrat : 
 *      <code> doit avoir une valeur parmi 'P', 'A' ou 'S'
 *      <valeur> doit avoir une valeur parmi [1,2] si code ='P' ou ='A' et parmi [1,8] si code ='S'
 *      le comportement est indéterminé en cas de non-respect de ces contraintes
 */

void pousserVoitureVersSortie ( place_num_t valeur );
/** Mode d'emploi :
 *      Cette fonction est appelée par Menu() (cf. module <Outils>)
 *      <valeur> : Place de parking à libérer (en faisant sortir la voiture !)
 *      
 *      Cette fonction se charge de faire parvenir à la sortie une requête de sortie
 *      d'un véhicule à une place donnée
 *      
 * 
 * Contrat : 
 *      <valeur> doit avoir une valeur parmi [1,8]
 *      le comportement est indéterminé en cas de non-respect de ces contraintes
 */

void pousserVoitureVersEntree ( TypeUsager usager, TypeBarriere barriere );
/** Mode d'emploi :
 *      Cette fonction est appelée par Menu() (cf. module <Outils>)
 *      <usager> : Type d'usager qui vient d'arriver à une barrière
 *      <barriere> : Barrière à laquelle la voiture vient d'arriver
 *      
 *      Cette fonction se charge de faire parvenir à la bonne tâche entrée
 *      une requête d'arrivée d'un usager de type <usager>
 *      
 * 
 * Contrat : 
 *      <usager> doit être différent de AUCUN
 *      <barriere> doit être différent de AUCUNE
 *      le comportement est indéterminé en cas de non-respect de ces contraintes
 */

#endif	/* KEYBOARD_H */

