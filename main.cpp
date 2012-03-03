/*************************************************************************
                           Main.cpp  -  description
                             -------------------
    début                : 27 février 2012
    copyright            : (C) 2012 par Florent Devriese
    e-mail               : florent.devriese@insa-lyon.fr
 *************************************************************************/
//---------- Réalisation de la tâche <Main> (fichier Main.cpp) -------------

//////////////////////////////////////////////INCLUDE
/*----------------------------------------- Includes personels ---------------------------------------------- */
#include "main.h"

using namespace std;

//////////////////////////////////////////////PUBLIC
/*----------------------------------------- Fonctions publiques ---------------------------------------------- */

int main ( int argc, char** argv ) {
    //Algorithme : aucun
    //

#ifdef MAP
    std::ofstream f ( "debug_main.log" );
    f << "Lancement du main" << endl;
#endif
    //*Création de l'écran et de ses protections
    InitialiserApplication ( XTERM );

    bool error = false; //*Booléen permettant de vérifier si une erreur s'est produite lors de l'initialisation de l'application

    pid_t noKeyboard;
    pid_t noEntreeGB;
    pid_t noEntreeBPP;
    pid_t noEntreeBPA;
    pid_t noSortie;
    pid_t noHeure;

    //-----------------------------------------initialisation---------------------------------

    int shmIdCompteur;
    int shmIdRequetes;
    int shmIdParking;

    unsigned int* shmPtCompteur;
    requete* shmPtRequetes;

    sem_t* semPtEntreeGB; //*sémaphore de synchronisation pour l'entrée Gaston Berger
    sem_t* semPtEntreeBPP; //*sémaphore de synchronisation pour l'entrée Blaise Pascal (Prof)
    sem_t* semPtEntreeBPA; //*sémaphore de synchronisation pour l'entrée Blaise Pascal (Autre)
    sem_t* semPtShmCompteur; //*sémaphore de protection de la mémoire partagée "compteur"
    sem_t* semPtShmRequete; //*sémaphore de protection de la mémoire partagée "requete"
    sem_t* semPtShmParking; //*sémaphore de protection de la mémoire partagée "parking"

    //*masquages des signaux
    struct sigaction action;
    action.sa_handler = SIG_IGN;
    action.sa_flags = SA_RESTART;
    sigaction ( SIGINT, &action, NULL );
    sigaction ( SIGUSR1, &action, NULL );
    sigaction ( SIGUSR2, &action, NULL );
    sigaction ( SIGHUP, &action, NULL );
    sigaction ( SIGQUIT, &action, NULL );
    sigaction ( SIGABRT, &action, NULL );
    sigaction ( SIGFPE, &action, NULL );
    sigaction ( SIGPIPE, &action, NULL );
    sigaction ( SIGALRM, &action, NULL );
    sigaction ( SIGTERM, &action, NULL );       

    //*Activation de l'heure
    if ( ( noHeure = ActiverHeure ( ) ) == -1 ) {
#ifdef MAP
        f << "fail activer heure" << endl;
#endif
        error = true;
    }

    //*Création des canaux
#ifdef MAP
    f << "Beginning creating shared memories" << endl;
#endif
    if ( mkfifo ( CANAL_KEY_ENTREE_BP_A, 0666 ) == -1 ) //*Canal reliant Keyboard et Entree blaise pascal autres
    {
        error = true;
#ifdef MAP
        f << "fail creation canal";
#endif
    }
    if ( !error && mkfifo ( CANAL_KEY_ENTREE_BP_P, 0666 ) == -1 ) //*Canal reliant Keyboard et Entree blaise pascal profs
    {
        error = true;
    }
    if ( !error && mkfifo ( CANAL_KEY_ENTREE_GB, 0666 ) == -1 ) //*Canal reliant Keyboard et Entree gaston berger
    {
        error = true;
    }
    if ( !error && mkfifo ( CANAL_KEY_SORTIE, 0666 ) == -1 ) //*Canal reliant Keyboard et Sortie
    {
        error = true;
    }
#ifdef MAP
    f << "End creating shared memories" << endl;
#endif

    //*Création des mémoires partagées
    if ( !error && ( shmIdCompteur = shmget ( CLEF_COMPTEUR, sizeof (compteur_t ), 0666 | IPC_CREAT ) ) < 0 ) {
        error = true;
#ifdef MAP
        f << "fail creation memoire partagee" << endl;
#endif
    }
    if ( !error && ( shmIdRequetes = shmget ( CLEF_REQUETES, sizeof (requete ) * NB_PORTES, 0666 | IPC_CREAT ) ) < 0 ) {
        error = true;
    }
    if ( !error && ( shmIdParking = shmget ( CLEF_PARKING, sizeof (requete ) * CAPACITE_PARKING, 0666 | IPC_CREAT ) ) < 0 ) {
        error = true;
    }
    if ( !error && ( shmPtCompteur = ( unsigned int* ) shmat ( shmIdCompteur, NULL, 0 ) ) == NULL ) {
        error = true;
    }
    else if ( !error ) {
#ifdef MAP
        f << "Initialisation du Compteur du parking à " << CAPACITE_PARKING << " places" << std::endl;
#endif
        *shmPtCompteur = CAPACITE_PARKING;
    }

#ifdef MAP
    else {
        f << "La réservation de shmPtCompteur a planté" << std::endl;
    }
#endif

    if ( !error && ( shmPtRequetes = ( requete* ) shmat ( shmIdRequetes, NULL, 0 ) ) == NULL ) {
        error = true;
    }
    else if ( !error ) {
        //* On initialise la mémoire avec des valeur correspondant au vide
        requete rVide;
        rVide.arrivee = -1;
        rVide.type = AUCUN;
        shmPtRequetes[0] = rVide;
        shmPtRequetes[1] = rVide;
        shmPtRequetes[2] = rVide;
    }


    //*Création des sémaphores
    if ( !error && ( semPtEntreeGB = sem_open ( SEM_ENTREE_GB, O_CREAT, 0666, 0 ) ) == SEM_FAILED ) {
        error = true;
#ifdef MAP
        f << "fail creation semaphore" << endl;
#endif
    }
    if ( !error && ( semPtEntreeBPA = sem_open ( SEM_ENTREE_BP_A, O_CREAT, 0666, 0 ) ) == SEM_FAILED ) {
        error = true;
    }
    if ( !error && ( semPtEntreeBPP = sem_open ( SEM_ENTREE_BP_P, O_CREAT, 0666, 0 ) ) == SEM_FAILED ) {
        error = true;
    }
    if ( !error && ( semPtShmCompteur = sem_open ( SEM_SHM_COMPTEUR, O_CREAT, 0666, 1 ) ) == SEM_FAILED ) {
        error = true;
    }
    if ( !error && ( semPtShmRequete = sem_open ( SEM_SHM_REQUETE, O_CREAT, 0666, 1 ) ) == SEM_FAILED ) {
        error = true;
    }
    if ( !error && ( semPtShmParking = sem_open ( SEM_SHM_PARKING, O_CREAT, 0666, 1 ) ) == SEM_FAILED ) {
        error = true;
    }

    if ( !error && ( noKeyboard = fork ( ) ) == 0 ) {
        //*Code du fils Keyboard
        Keyboard ( );
    }
    else if ( !error && noKeyboard == -1 ) {
        error = true;
    }
    else if ( !error && ( noEntreeGB = fork ( ) ) == 0 ) {
        //*Code du fils entree Gaston Berger
        Entree ( ENTREE_GB );
    }
    else if ( !error && noEntreeGB == -1 ) {
        error = true;
    }
    else if ( !error && ( noEntreeBPP = fork ( ) ) == 0 ) {
        //*Code du fils entree Prof (Blaise Pascal)
        Entree ( ENTREE_P );
    }
    else if ( !error && noEntreeBPP == -1 ) {
        error = true;
    }
    else if ( !error && ( noEntreeBPA = fork ( ) ) == 0 ) {
        //*Code du fils entree Autres (Blaise Pascal)
        Entree ( ENTREE_A );
    }
    else if ( !error && noEntreeBPA == -1 ) {
        error = true;
    }
    else if ( !error && ( noSortie = fork ( ) ) == 0 ) {
        //*Code du fils Sortie
        Sortie ( );
    }
    else if ( !error && noSortie == -1 ) {
        error = true;
    }
    else {
#ifdef MAP
        f << "Pid créés : " << std::endl;
        f << "Keyboard : " << noKeyboard << std::endl;
        f << "Sortie : " << noSortie << std::endl;
        f << "Entrée BPA : " << noEntreeBPA << std::endl;
        f << "Entrée BPP : " << noEntreeBPP << std::endl;
        f << "Entrée GB : " << noEntreeGB << std::endl;
#endif
        //*Code de la mère

        //---------------------------------------------Moteur-------------------------------------------
        int st = -1;
        waitpid ( noKeyboard, &st, 0 ); //Attend la fin de la tache fille Keyboard
#ifdef MAP
        f << "Keyboard has exited" << endl;
#endif


        //----------------------------------------------Destruction-------------------------------------

        if ( error ) //*S'il ya eu une erreur pendant l'initialisation, on demande à Keyboard de se détruire
        {
            kill ( noKeyboard, SIGUSR2 );
        }

        // Envoi du signal de fin d'application aux taches filles et attente de la fin de celles ci
        // afin de détruire les canaux (car il faut qu'il n'y ait plus ni écrivains ni lecteurs du le canal
        // avant de le fermer

        kill ( noEntreeBPA, SIGUSR2 );
        waitpid ( noEntreeBPA, &st, 0 );
#ifdef MAP
        f << "BPA has exited" << endl;
#endif
        kill ( noEntreeBPP, SIGUSR2 );
        waitpid ( noEntreeBPP, &st, 0 );

        kill ( noEntreeGB, SIGUSR2 );
        waitpid ( noEntreeGB, &st, 0 );
#ifdef MAP
        f << "GB has exited" << endl;
#endif

        kill ( noHeure, SIGUSR2 );
        waitpid ( noHeure, &st, 0 );

#ifdef MAP
        f << "Heure has exited" << endl;
#endif
        kill ( noSortie, SIGUSR2 );
        waitpid ( noSortie, &st, 0 );

#ifdef MAP
        f << "Sortie has exited" << endl;
#endif

        //*Destruction des canaux, il est necessaire d'attendre qu'il n'y ai plus de lecteurs ni d'écrivains. 
        unlink ( CANAL_KEY_ENTREE_BP_A );
        unlink ( CANAL_KEY_ENTREE_BP_P );
        unlink ( CANAL_KEY_ENTREE_GB );
        unlink ( CANAL_KEY_SORTIE );

#ifdef MAP
        f << "Canaux détruits" << endl;
#endif
        //*Destruction des mémoires partagées 
        shmdt ( shmPtCompteur );
        shmctl ( shmIdCompteur, IPC_RMID, 0 );
        shmctl ( shmIdRequetes, IPC_RMID, 0 );
        shmctl ( shmIdParking, IPC_RMID, 0 );

#ifdef MAP
        f << "Memoire partagee detruites" << endl;
#endif

        //*Destruction des sémaphores
        sem_unlink ( SEM_ENTREE_BP_A );
        sem_unlink ( SEM_ENTREE_BP_P );
        sem_unlink ( SEM_ENTREE_GB );
        sem_unlink ( SEM_SHM_COMPTEUR );
        sem_unlink ( SEM_SHM_REQUETE );
        sem_unlink ( SEM_SHM_PARKING );

#ifdef MAP
        f << "Semaphores détruits" << endl;
#endif
        //*Suppression de l'écran et de ses protections
        TerminerApplication ( );
    }

    if ( error ) {
        return -1;
    }
    else {
        return 0;
    }
#ifdef MAP
    f.close ( );
#endif
}//------------Fin de Main

