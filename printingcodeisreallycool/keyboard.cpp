/*************************************************************************
						   entree  -  description
							 -------------------
	début                : 16/02/2012
	copyright            : (C) 2012-2042 par tdubourg
	e-mail               : theo.dubourg@insa-lyon.fr
 *************************************************************************/

//---------- Réalisation de la tâche <Entree> (fichier entree.cpp) ---

/////////////////////////////////////////////////////////////////  INCLUDE
//-------------------------------------------------------- Include système

//------------------------------------------------------ Include personnel
#include "keyboard.h"

///////////////////////////////////////////////////////////////////  PRIVE
//------------------------------------------------------------- Constantes

//------------------------------------------------------------------ Types

//---------------------------------------------------- Variables statiques
static int descGB = -1;
static int descBPP = -1;
static int descBPA = -1;
static int descS = -1;

//------------------------------------------------------ Fonctions privées

/**
 * Mode d'emploi / description : 
 * FinProgramme : fonction de terminaison de la tâche courante 
 * Mode d'emploi : 
 * 
 * Cette fonction se charge de la fermeture de toutes les ressources utilisées 
 * par la tâche courante.
 * Une fois la phase de destruction de la tâche courante effectuée, cette fonction
 * termine la tâche courante.
 * 
 * Contrat : aucun
 */
static void FinProgramme(int useless) {
	if (descGB != -1) {
		close(descGB);
	}
	if (descBPP != -1) {
		close(descBPP);
	}
	if (descBPA != -1) {
		close(descBPA);
	}
	if (descS != -1) {
		close(descS);
	}
	exit(EXIT_CODE);
} // Fin FinProgramme


//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques

/**
 * Mode d'emploi : cf. entree.h
 */

void Keyboard() {

	/*********************** INITIALISATION **************************/
	//* Note : Dans cette tâche, pas de démasquage de SIGUSR2, c'est keyBoard() qui sera à l'origine de la fin du programme ! (détection de la touche E|e)

	//* Ouverture de tous les canaux de communication vers les autres tâches :
	descGB = open(CANAL_KEY_ENTREE_GB, O_WRONLY);
	descBPP = open(CANAL_KEY_ENTREE_BP_P, O_WRONLY);
	descBPA = open(CANAL_KEY_ENTREE_BP_A, O_WRONLY);
	descS = open(CANAL_KEY_SORTIE, O_WRONLY);


	/*********** FIN DE L'INITIALISATION ************/

	/************ PHASE MOTEUR ***************/
	for (;;) {
		Menu();
	}
	/*************** FIN PHASE MOTEUR *******/

	/************** PHASE DESTRUCTION **********/
	//* If, for any reason, we get here, make sure everything closes properly :
	FinProgramme(0);
} // Fin Keyboard

/**
 * Mode d'emploi : cf. entree.h
 */
void pousserVoitureVersSortie(place_num_t valeur) {


	//* Envoi du numéro de place à libérer à la sortie : 
	write(descS, &valeur, sizeof (place_num_t));


} // Fin pousserVoitureVersSortie


/**
 * Mode d'emploi : cf. entree.h
 */
void pousserVoitureVersEntree(TypeUsager usager, TypeBarriere barriere) {
	//* Cette variable est uniquement statique à cette fonction car elle n'a pas besoin
	//* D'être exposée à d'autres fonction, son utilisation est INTERNE
	//* Elle gère les plaques minéralogiques !
	static plaque_t NextVoitureId = 1;

	//* Décision conditionnelle de sur quel canal on va écrire en fonction du paramétrage d'appel
	int canalDesc;

	if (usager == PROF) {
		if (barriere == PROF_BLAISE_PASCAL) {
			canalDesc = descBPP;
		} else {
			canalDesc = descGB;
		}
	} else {
		if (barriere == AUTRE_BLAISE_PASCAL) {
			canalDesc = descBPA;
		} else {
			canalDesc = descGB;
		}
	}

	//* Initialisation d'une voiture qu'on va envoyer dans la file d'attente
	voiture tuture;
	tuture.type = usager;
	//* Gestion de la plaque minéralogique
	if (NextVoitureId > MAX_PLAQUE) {
		NextVoitureId = 1;
	}
	tuture.plaque = NextVoitureId++; //* @warning : NextVoitureId is incremented here
	
	//* Ecriture des informations dans le canal de communication :
	write(canalDesc, &tuture, sizeof (voiture));


} // Fin pousserVoitureVersEntree


/**
 * Mode d'emploi : cf. entree.h
 */
void Commande(char code, unsigned int valeur) {

	//* Initialisation des variables :
	TypeUsager usager = AUCUN;
	TypeBarriere barriere = AUCUNE;
	switch (code) {//* En fonction du choix du menu
		case 'E':

			exit(EXIT_CODE);
			break;

		case 'P':
			//* En fonction du choix du ss-menu : 
			usager = PROF;
			if (valeur == MENU_CHOICE_PROF_BP) {
				barriere = PROF_BLAISE_PASCAL;
			} else {
				barriere = ENTREE_GASTON_BERGER;
			}
			//* break VOLONTAIREMENT non mis ici, les deux cas sont similaires, évitons de répéter le code
		case 'A':
			//* En fonction du choix du ss-menu : 
			if (usager == AUCUN) {//* Sert à ce que, quand c'est "P" la valeur ne soit pas écrasée
				usager = AUTRE;
				if (valeur == MENU_CHOICE_AUTRE_BP) {
					barriere = AUTRE_BLAISE_PASCAL;
				} else {
					barriere = ENTREE_GASTON_BERGER;
				}
			}
			//* valeur = n° de la porte (0,1,2)

			//* Lancement de la communication vers l'entrée :
			pousserVoitureVersEntree(usager, barriere);
			break;
			
		case 'S':
			//* Lancement de la communication vers la sortie :
			pousserVoitureVersSortie(valeur);
			break;
	}

} // Fin Commande
