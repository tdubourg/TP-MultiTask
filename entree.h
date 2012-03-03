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



/////////////////////////////////////////////////////////////////  INCLUDE
//--------------------------------------------------- Interfaces utilisées
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



//////////////////////////////////////////////////////////////////  PUBLIC
//---------------------------------------------------- Fonctions publiques


void Entree(int porte_num);
/** Mode d'emploi :
 *      Cette fonction gère l'ensemble du module <Entree>
 *      Pour avoir plus de détails sur l'algorithme du module <Entree> : cf. le commentaire en début de entree.h
 *      <porte_num> : numéro de la porte que l'on va gérer
 *      
 * 
 * Contrat : 
 *      <porte_num> doit avoir une valeur parmi [1, NB_BARRIERES_ENTREE]
 *      le comportement est indéterminé en cas de non-respect de ces contraintes
 */

pid_t Garage(requete req, TypeBarriere barriere, voiture tuture, sem_t* semPtShmParking, requete* shmPtParking);
/** Mode d'emploi :
 *      Cette fonction est appelée par Entree() 
 *      <req> : Type d'usager qui vient d'arriver à une barrière
 *      <barriere> : Barrière à laquelle la voiture vient d'arriver
 *      <tuture> : Voiture à garer
 *      <semPtShmParking> : Variable attachée au sémaphore de synchro sur la mémoire partagée
 *              relative au stockage des places de parking.
 *      <shmPtParking> : Variable attachée à la mémoire partagée relative au stockage 
 *              des places de parking.
 *      
 *      Cette fonction se charge de toute la partie relative au garage de la voiture
 *      arrivante (une fois qu'on a déjà validé qu'il y a de la place pour garer
 *      cette voiture). Elle crée une sous-tâche (tâche fille) qui exécutera 
 *      entreeAttenteFinGarage() et mettra à jour l'état des places de parking
 *      en mémoire partagée.
 *      
 *		<retour> : Le retour de cette fonction est le pid de la tâche fille créée.
 *      
 * 
 * Contrat : 
 *      <req> : Doit être une requête non-vide (attributs initialisés, usager (type) != AUCUN)
 *      <barriere> : différent de AUCUNE
 *      <tuture> : Voiture non-vide (attributs initialisés, usager (type) != AUCUN)
 *      <semPtShmParking> : Le pointeur doit être non NULL et initialisé / ouvert correctement.
 *      <shmPtParking> : Le pointeur doit être non NULL et initialisé / ouvert correctement.
 *      le comportement est indéterminé en cas de non-respect de ces contraintes
 */

unsigned char EntreeAttenteFinGarage(TypeBarriere barriere, TypeUsager usager, time_t arrivee, unsigned int numVoiture);
/** Mode d'emploi :
 *      Cette fonction est appelée par Garage() 
 *      <barriere> : Barrière à laquelle la voiture vient d'arriver
 *		<usager> : type de l'usager qui vient d'arriver
 *		<arrivee> : Heure d'arrivée de l'usager que l'on va garer
 *		<numVoiture> : Numéro d'immatriculation / minéralogique de la voiture que l'on va garer
 *      
 *      Cette fonction se charge de la partie "Garage pure", CàD le mouvement
 *		du "voiturier" (qui est lancé par cette fonction).
 *		Cette fonction met également à jour l'Affichage de la zone de droite concernant
 *		l'état courant du parking, mais ne s'occupe pas de la mise à jour de la mémoire
 *		partagée.
 *		/!\ Note importante : Cette fonction est "bloquante". Elle se termine lorsque le "garage"
 *		est totalement terminé.
 *      
 *		<retour> : Le retour de cette fonction est le pid de la tâche fille créée.
 *			<retour> vaut 0 si une erreur est survenue
 *      
 * 
 * Contrat : 
 *      <barriere> : différent de AUCUNE
 *      <usager> doit être différent de AUCUN
 *		<numVoiture> : doit être compris dans [1, 999]
 *      le comportement est indéterminé en cas de non-respect de ces contraintes
 */
#endif	/* ENTREE_H */

