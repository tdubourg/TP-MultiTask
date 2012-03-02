
//////////////////////////////////////////////INCLUDE
/*----------------------------------------- Includes personels ---------------------------------------------- */
#include "affichageSortie.h"

using namespace std;

//////////////////////////////////////////////PRIVE
/*----------------------------------------- Variables statiques ---------------------------------------------- */
static int noAff = -1;
static int* shmPtCompteur;
static requete* shmPtRequetes;
static requete * shmPtParking;
static sem_t* semPtShmCompteur;
static sem_t* semPtShmRequete;
static sem_t* semPtShmParking;
static sem_t* semPtEntree_GB;
static sem_t* semPtEntree_BP_A;
static sem_t* semPtEntree_BP_P;

/*----------------------------------------- Fonctions privées ---------------------------------------------- */
static void FinProgramme ( int num ) {
    //Algorithme : aucun
    //
#ifdef MAP
    std::ofstream f ( "affichageSortie.log", ios_base::app );
    f << "AFFICHAGESORTIE : on recoie le signal ..." << std::endl;
#endif
    if ( noAff != -1 ) {
#ifdef MAP
        f << "AFFICHAGESORTIE : on envoie le signal a la fifille" << std::endl;
#endif
        kill ( noAff, SIGUSR2 );
        int st = -1;
        waitpid ( noAff, &st, 0 );
    }

    //Detachement des mémoires partagées 
    shmdt ( shmPtCompteur );
    shmdt ( shmPtRequetes );
    shmdt ( shmPtParking );

    //Fermeture des semaphore
    sem_close ( semPtEntree_GB );
    sem_close ( semPtEntree_BP_A );
    sem_close ( semPtEntree_BP_P );
    sem_close ( semPtShmCompteur );
    sem_close ( semPtShmParking );
    sem_close ( semPtShmRequete );

    if ( num != -1 ) {
        num = 0;
    }

    exit ( num );
}//------Fin de FinProgramme


//////////////////////////////////////////////PUBLIC

/*----------------------------------------- Fonctions publiques ---------------------------------------------- */

void affichageSortie ( unsigned int place ) {
    //Algorithme : aucun
    //
#ifdef MAP
    std::ofstream f ( "affichageSortie.log", ios_base::app );
    f << "AFFICHAGESORTIE : Début de affichageSortie(" << place << ")" << std::endl;
#endif
    //--------------------------------Initialisation------------------------------------

    bool error = false;

    //*Association du signal SIGUSR2 à  la fin du programme
    struct sigaction action;
    action.sa_handler = FinProgramme;
    sigaction ( SIGUSR2, &action, NULL );

    //*Ouverture des sémaphore de protection  
    semPtShmCompteur = sem_open ( SEM_SHM_COMPTEUR, 0, 0666, 1 );
    if ( semPtShmCompteur == SEM_FAILED ) {
        error = true;
    }
    semPtShmRequete = sem_open ( SEM_SHM_REQUETE, 0, 0666, 1 );
    if ( semPtShmRequete == SEM_FAILED ) {
        error = true;
    }
    semPtShmParking = sem_open ( SEM_SHM_PARKING, 0, 0666, 1 );
    if ( semPtShmParking == SEM_FAILED ) {
        error = true;
    }
    semPtEntree_GB = sem_open ( SEM_ENTREE_GB, 0, 0666, 1 );
    if ( semPtEntree_GB == SEM_FAILED ) {
        error = true;
    }
    semPtEntree_BP_A = sem_open ( SEM_ENTREE_BP_A, 0, 0666, 1 );
    if ( semPtEntree_BP_A == SEM_FAILED ) {
        error = true;
    }
    semPtEntree_BP_P = sem_open ( SEM_ENTREE_BP_P, 0, 0666, 1 );
    if ( semPtEntree_BP_P == SEM_FAILED ) {
        error = true;
    }

    //*Ouverture des mémoires partagées 
    int shmIdCompteur = shmget ( CLEF_COMPTEUR, sizeof (int ), 0666 | 0 );
    if ( shmIdCompteur < 0 ) {
        error = true;
    }
    int shmIdRequetes = shmget ( CLEF_REQUETES, sizeof (requete ) * NB_PORTES, 0666 | 0 );
    if ( shmIdRequetes < 0 ) {
        error = true;
    }
    int shmIdParking = shmget ( CLEF_PARKING, sizeof (requete ) * CAPACITE_PARKING, 0666 );
    if ( shmIdParking < 0 ) {
        error = true;
    }

    shmPtCompteur = ( int* ) shmat ( shmIdCompteur, NULL, 0 );
    if ( shmPtCompteur == NULL ) {
        error = true;
    }
    shmPtRequetes = ( requete* ) shmat ( shmIdRequetes, NULL, 0 );
    if ( shmPtRequetes == NULL ) {
        error = true;
    }
    shmPtParking = ( requete* ) shmat ( shmIdParking, NULL, 0 );
    if ( shmPtParking == NULL ) {
        error = true;
    }

    //------------------------------------Moteur--------------------------------------- 

    if ( error ) {
        Afficher ( MESSAGE, "Erreur de création de tache, quittez le programme." );
        FinProgramme ( -1 );
    }

    noAff = SortirVoiture ( place );
#ifdef MAP
    f << "AFFICHAGESORTIE : lancement d'un sortir voiture avec le pid : " << noAff << endl;
#endif

    int st = -1;
    do {
        waitpid ( noAff, &st, 0 );
#ifdef MAP
        f << "waitpid(" << noAff << ") status récupéré : " << st << endl;
#endif		
    } while ( st < 0 );

#ifdef MAP
    f << "AFFICHAGESORTIE : Demande de lock sur le semaphore ShmCompteur" << std::endl;
#endif

    sem_wait ( semPtShmCompteur ); //*On prend possession de la mémoire partagée servant à compter le nombre de place dans le parking, sinon, on attend qu'elle soit disponible

#ifdef MAP
    f << "AFFICHAGESORTI : Demande de lock sur le semaphore ShmCompteur RÉUSSIE" << std::endl;
#endif

    bool isFull = !( *shmPtCompteur ); //* /!\ On le fait avant de libérer de la place /!\ 

#ifdef MAP
    f << "AFFICHAGESORTIE : Valeur compteur avant incrément : " << *shmPtCompteur << std::endl;
#endif

    *shmPtCompteur = ( *shmPtCompteur ) + 1;

#ifdef MAP
    f << "AFFICHAGESORTIE : Relâchement du lock sur le semaphore ShmCompteur après incrément. Valeur actuelle :" << *shmPtCompteur << std::endl;
#endif
    sem_post ( semPtShmCompteur ); //*On restitue l'accès à la mémoire partagée

#ifdef MAP
    f << "AFFICHAGESORTIE : Affichage de la sortie effectuée + Effacement des places de parking." << std::endl;
#endif

    switch ( place )//*Effacement des places correspondantes à la sortie
    {
        case 1:
            Effacer ( ETAT_P1 );
            break;
        case 2:
            Effacer ( ETAT_P2 );
            break;
        case 3:
            Effacer ( ETAT_P3 );
            break;
        case 4:
            Effacer ( ETAT_P4 );
            break;
        case 5:
            Effacer ( ETAT_P5 );
            break;
        case 6:
            Effacer ( ETAT_P6 );
            break;
        case 7:
            Effacer ( ETAT_P7 );
            break;
        case 8:
            Effacer ( ETAT_P8 );
            break;
    }

#ifdef MAP
    f << "AFFICHAGESORTIE : Effacement effectué." << std::endl;
#endif

    sem_wait ( semPtShmParking );

#ifdef MAP
    f << "AFFICHAGESORTIE : Lancement de l'affichage des infos relatives à la sortie toute récente de tuture." << std::endl;
#endif

    AfficherSortie ( shmPtParking[place].type, shmPtParking[place].plaque, shmPtParking[place].arrivee, time ( NULL ) );

#ifdef MAP
    f << "AFFICHAGESORTIE : Affichage effectué/" << std::endl;
#endif

    sem_post ( semPtShmParking );

    if ( isFull ) {

#ifdef MAP
        f << "AFFICHAGESORTIE : Parking was full just before this car exit, so let's check if they are requests to be satisfied." << std::endl;
#endif
        sem_t* semPtToUnlock;
        //* Parcours du tableau des requêtes :
        sem_wait ( semPtShmRequete );
        if ( shmPtRequetes[ENTREE_P].type != AUCUN ) //* S'il y a un prof à l'entree prof...
        {
            if ( shmPtRequetes[ENTREE_GB].type != PROF ) //*... Et s'il n'y a pas d'autre prof a l'entree de tous, le prof rentre
            {
#ifdef MAP
                f << "AFFICHAGESORTIE : Nous allons debloquer l entree PROF" << std::endl;
#endif
                semPtToUnlock = semPtEntree_BP_P;
                Effacer ( REQUETE_R1 );
                shmPtRequetes[ENTREE_P].type = AUCUN;
                shmPtRequetes[ENTREE_P].arrivee = -1;
            }
            else //*... Mais s'il y a un prof a l'entree de tous, le premier arrive doit rentrer
            {
                if ( shmPtRequetes[ENTREE_P].arrivee < shmPtRequetes[ENTREE_GB].arrivee ) //* si le prof a l'entree Prof est arrive avant l'autre, il rentre
                {
#ifdef MAP
                    f << "AFFICHAGESORTIE : Nous allons debloquer l entree PROF" << std::endl;
#endif
                    semPtToUnlock = semPtEntree_BP_P;
                    Effacer ( REQUETE_R1 );
                    shmPtRequetes[ENTREE_P].type = AUCUN;
                    shmPtRequetes[ENTREE_P].arrivee = -1;
                }
                else //*Sinon, l'autre prof rentre
                {
#ifdef MAP
                    f << "AFFICHAGESORTIE : Nous allons debloquer l entree GB" << std::endl;
#endif
                    semPtToUnlock = semPtEntree_GB;
                    Effacer ( REQUETE_R3 );
                    shmPtRequetes[ENTREE_GB].type = AUCUN;
                    shmPtRequetes[ENTREE_GB].arrivee = -1;
                }
            }
        }
        else //*...Sinon s'il n'y a pas de prof à l'entree prof ...
        {
            if ( shmPtRequetes[ENTREE_GB].type == PROF ) //*... Si il y a un prof a l'entree de tous, il rentre
            {
#ifdef MAP
                f << "AFFICHAGESORTIE : Nous allons debloquer l entree GB" << std::endl;
#endif
                semPtToUnlock = semPtEntree_GB;
                Effacer ( REQUETE_R3 );
                shmPtRequetes[ENTREE_GB].type = AUCUN;
                shmPtRequetes[ENTREE_GB].arrivee = -1;
            }
            else if ( shmPtRequetes[ENTREE_GB].type == AUTRE )//* Si il y a un autre a l'entree de tous ...
            {
                if ( shmPtRequetes[ENTREE_A].type != AUCUN ) //*... Si il y a un autre a l'entree des autres , le premier arrive doit rentrer
                {
                    if ( shmPtRequetes[ENTREE_A].arrivee < shmPtRequetes[ENTREE_GB].arrivee ) //* si l'autre a l'entree autre est arrivee avant l'autre a l'entree de tous, il rentre
                    {
#ifdef MAP
                        f << "AFFICHAGESORTIE : Nous allons debloquer l entree AUTRE" << std::endl;
#endif
                        semPtToUnlock = semPtEntree_BP_A;
                        Effacer ( REQUETE_R2 );
                        shmPtRequetes[ENTREE_A].type = AUCUN;
                        shmPtRequetes[ENTREE_A].arrivee = -1;
                    }
                    else //*Sinon, c'est l'autre de l'entree de tous qui rentre
                    {
#ifdef MAP
                        f << "AFFICHAGESORTIE : Nous allons debloquer l entree GB" << std::endl;
#endif
                        semPtToUnlock = semPtEntree_GB;
                        Effacer ( REQUETE_R3 );
                        shmPtRequetes[ENTREE_GB].type = AUCUN;
                        shmPtRequetes[ENTREE_GB].arrivee = -1;
                    }
                }
                else //*Sinon si il n'y a personne à l'entree des autres, l'autre a la rentree de tous rentre
                {
#ifdef MAP
                    f << "AFFICHAGESORTIE : Nous allons debloquer l entree GB" << std::endl;
#endif
                    semPtToUnlock = semPtEntree_GB;
                    Effacer ( REQUETE_R3 );
                    shmPtRequetes[ENTREE_GB].type = AUCUN;
                    shmPtRequetes[ENTREE_GB].arrivee = -1;
                }
            }
            else //*Sinon, le dernier cas possible etant qu'il y ai seulement un autre a l'entree autre, on le fait rentrer
            {
#ifdef MAP
                f << "AFFICHAGESORTIE : Nous allons debloquer l entree AUTRE" << std::endl;
#endif
                semPtToUnlock = semPtEntree_BP_A;
                Effacer ( REQUETE_R2 );
                shmPtRequetes[ENTREE_A].type = AUCUN;
                shmPtRequetes[ENTREE_A].arrivee = -1;
            }
        }

        //* Débloquage du sema sur les requêtes
        sem_post ( semPtShmRequete );
        //* Débloquage de la porte qui va bien :
        sem_post ( semPtToUnlock );
    }
#ifdef MAP
    f.close ( );
#endif
    //Si, pour une raison quelconque on arrive ici, on arrete proprement
    FinProgramme ( 0 );
}//-----------Fin de affichageSortie
