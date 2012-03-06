
//////////////////////////////////////////////INCLUDE
/*----------------------------------------- Includes personels ---------------------------------------------- */
#include "affichageSortie.h"

using namespace std;

//////////////////////////////////////////////PRIVE
/*----------------------------------------- Variables statiques ---------------------------------------------- */
static int noAff = -1;
static compteur_t* shmPtCompteur;
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

    if ( noAff != -1 ) {

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

void affichageSortie ( place_num_t place ) {
    //Algorithme : aucun
    //

    //--------------------------------Initialisation------------------------------------

    bool error = false;

    //*Association du signal SIGUSR2 à  la fin du programme
    struct sigaction action;
    action.sa_handler = FinProgramme;
    action.sa_flags = 0;
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
    int shmIdCompteur = shmget ( CLEF_COMPTEUR, sizeof (compteur_t ), 0666 | 0 );
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

    shmPtCompteur = ( compteur_t* ) shmat ( shmIdCompteur, NULL, 0 );
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


    int st = -1;
        waitpid ( noAff, &st, 0 );




    sem_wait ( semPtShmCompteur ); //*On prend possession de la mémoire partagée servant à compter le nombre de place dans le parking, sinon, on attend qu'elle soit disponible



    bool isFull = !( *shmPtCompteur ); //* /!\ On le fait avant de libérer de la place /!\ 



    *shmPtCompteur = ( *shmPtCompteur ) + 1;


    sem_post ( semPtShmCompteur ); //*On restitue l'accès à la mémoire partagée



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



    sem_wait ( semPtShmParking );



    AfficherSortie ( shmPtParking[place-1].type, shmPtParking[place-1].plaque, shmPtParking[place-1].arrivee, time ( NULL ) );//* Note : place = [1,8] mais tableau = [0,7]



    sem_post ( semPtShmParking );

    if ( isFull ) {


        sem_t* semPtToUnlock;
        //* Parcours du tableau des requêtes :
        sem_wait ( semPtShmRequete );
        if ( shmPtRequetes[ENTREE_P].type != AUCUN ) //* S'il y a un prof à l'entree prof...
        {
            if ( shmPtRequetes[ENTREE_GB].type != PROF ) //*... Et s'il n'y a pas d'autre prof a l'entree de tous, le prof rentre
            {

                semPtToUnlock = semPtEntree_BP_P;
                Effacer ( REQUETE_R1 );
                shmPtRequetes[ENTREE_P].type = AUCUN;
                shmPtRequetes[ENTREE_P].arrivee = -1;
            }
            else //*... Mais s'il y a un prof a l'entree de tous, le premier arrive doit rentrer
            {
                if ( shmPtRequetes[ENTREE_P].arrivee < shmPtRequetes[ENTREE_GB].arrivee ) //* si le prof a l'entree Prof est arrive avant l'autre, il rentre
                {

                    semPtToUnlock = semPtEntree_BP_P;
                    Effacer ( REQUETE_R1 );
                    shmPtRequetes[ENTREE_P].type = AUCUN;
                    shmPtRequetes[ENTREE_P].arrivee = -1;
                }
                else //*Sinon, l'autre prof rentre
                {

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

                        semPtToUnlock = semPtEntree_BP_A;
                        Effacer ( REQUETE_R2 );
                        shmPtRequetes[ENTREE_A].type = AUCUN;
                        shmPtRequetes[ENTREE_A].arrivee = -1;
                    }
                    else //*Sinon, c'est l'autre de l'entree de tous qui rentre
                    {

                        semPtToUnlock = semPtEntree_GB;
                        Effacer ( REQUETE_R3 );
                        shmPtRequetes[ENTREE_GB].type = AUCUN;
                        shmPtRequetes[ENTREE_GB].arrivee = -1;
                    }
                }
                else //*Sinon si il n'y a personne à l'entree des autres, l'autre a la rentree de tous rentre
                {

                    semPtToUnlock = semPtEntree_GB;
                    Effacer ( REQUETE_R3 );
                    shmPtRequetes[ENTREE_GB].type = AUCUN;
                    shmPtRequetes[ENTREE_GB].arrivee = -1;
                }
            }
            else //*Sinon, le dernier cas possible etant qu'il y ai seulement un autre a l'entree autre, on le fait rentrer
            {

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

    //Si on arrive ici, on arrete proprement
    FinProgramme ( 0 );
}//-----------Fin de affichageSortie
