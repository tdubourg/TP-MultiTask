/* 
 * File:   main.cpp
 * Author: fdevriese
 *
 * Created on 13 février 2012, 15:08
 */

#include "main.h"
using namespace std;

int main (int argc, char** argv){

    //Création de l'écran et de ses protections
    //InitialiserApplication (XTERM);

    bool error = false; //Booléen permettant de vérifier si une erreur s'est produite lors de l'initialisation de l'application

    pid_t noKeyboard;
    pid_t noEntreeGB;
    pid_t noEntreeBPP;
    pid_t noEntreeBPA;
    pid_t noSortie;
    pid_t noHeure;

    //-----------------------------------------initialisation---------------------------------

    int shmIdCompteur;
    int shmIdRequetes;

    unsigned int* shmPtCompteur;
    requete* shmPtRequetes;

    sem_t* semPtEntreeGB; //sémaphore de synchronisation pour l'entrée Gaston Berger
    sem_t* semPtEntreeBPP; //sémaphore de synchronisation pour l'entrée Blaise Pascal (Prof)
    sem_t* semPtEntreeBPA; //sémaphore de synchronisation pour l'entrée Blaise Pascal (Autre)
    sem_t* semPtShmCompteur; //sémaphore de protection de la mémoire partagée "compteur"
    sem_t* semPtShmRequete; //sémaphore de protection de la mémoire partagée "requete"

    //masquages des signaux
    struct sigaction action;
    action.sa_handler = SIG_IGN;
    action.sa_flags = 0;
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGUSR1, &action, NULL);
    sigaction (SIGUSR2, &action, NULL);

    //Création des canaux
#ifdef MAP
	cout << "Beginning creating shared memories" << endl;
#endif
    if(mkfifo(CANAL_KEY_ENTREE_BP_A, 0666) == -1) //Canal reliant Keyboard et Entree blaise pascal autres
    {
        error = true;
#ifdef MAP
        cout << "lol";
#endif
    }
    if(!error && mkfifo (CANAL_KEY_ENTREE_BP_P, 0666) == -1) //Canal reliant Keyboard et Entree blaise pascal profs
    {
        error = true;
    }
    if(!error && mkfifo (CANAL_KEY_ENTREE_GB, 0666) == -1) //Canal reliant Keyboard et Entree gaston berger
    {
        error = true;
    }
    if(!error && mkfifo (CANAL_KEY_SORTIE, 0666) == -1) //Canal reliant Keyboard et Sortie
    {
        error = true;
    }
#ifdef MAP
	cout << "End creating shared memories" << endl;
#endif

    //Création des mémoires partagées
    if(!error && (shmIdCompteur = shmget (CLEF_COMPTEUR, sizeof(unsigned int), 0666 | IPC_CREAT)) < 0){
        error = true;
    }
    if(!error && (shmIdRequetes = shmget (CLEF_REQUETES, sizeof(requete) * NB_PORTES, 0666 | IPC_CREAT)) < 0){
        error = true;
    }
    if(!error && (shmPtCompteur = (unsigned int*) shmat (shmIdCompteur, NULL, 0)) == NULL){
        error = true;
    }
	else if(!error) {
        *shmPtCompteur = CAPACITE_PARKING;
    }
    /*/if (!error && (shmPtRequetes = (requete*) shmat(shmIdRequetes, NULL, 0)) == NULL) {
        error = true;
    }/* abandopnné else { //* Initialisation du tableau à NULL pour qu'on puisse savoir s'il y a déjà qqchose dedans ou pas quand on le manipule par la suite
            shmPtRequetes = (requete**)shmPtRequetes;
            for(int i = 0; i < NB_PORTES; i++) {
                shmPtRequetes[i] = NULL;
            }
        }*/


    //Création des sémaphores
    if(!error && (semPtEntreeGB = sem_open (SEM_ENTREE_GB, O_CREAT, 0666, 0)) == SEM_FAILED){
        error = true;
    }
    if(!error && (semPtEntreeBPA = sem_open (SEM_ENTREE_BP_A, O_CREAT, 0666, 0)) == SEM_FAILED){
        error = true;
    }
    if(!error && (semPtEntreeBPP = sem_open (SEM_ENTREE_BP_P, O_CREAT, 0666, 0)) == SEM_FAILED){
        error = true;
    }
    if(!error && (semPtShmCompteur = sem_open (SEM_SHM_COMPTEUR, O_CREAT, 0666, 1)) == SEM_FAILED){
        error = true;
    }
    if(!error && (semPtShmRequete = sem_open (SEM_SHM_REQUETE, O_CREAT, 0666, 1)) == SEM_FAILED){
        error = true;
    }



    if((noKeyboard = fork ()) == 0){
        //Code du fils Keyboard
        keyboard ();
    }
    else if(noKeyboard == -1){
        error = true;
    }
    else if((noHeure = fork ()) == 0){
        //Code de l'heure
        ActiverHeure ();
    }
    else if(noHeure == -1){
        error = true;
    }
    else if((noEntreeGB = fork ()) == 0){
        //Code du fils entree Gaston Berger
        entree (ENTREE_GB);
    }
    else if(noEntreeGB == -1){
        error = true;
    }
    else if((noEntreeBPP = fork ()) == 0){
        //Code du fils entree Prof (Blaise Pascal)
        entree (ENTREE_P);
    }
    else if(noEntreeBPP == -1){
        error = true;
    }
    else if((noEntreeBPA = fork ()) == 0){
        //Code du fils entree Autres (Blaise Pascal)
        entree (ENTREE_A);
    }
    else if(noEntreeBPA == -1){
        error = true;
    }
    else if((noSortie = fork ()) == 0){
        //Code du fils Sortie
        Sortie ();
    }
    else if(noSortie == -1){
        error = true;
    }
    else{
        //Code de la mère

        //---------------------------------------------Moteur-------------------------------------------
        int st = -1;
        do{
            waitpid (noKeyboard, &st, 0); //Attend la fin de la tache fille Keyboard
        }while(st != 0);


        //----------------------------------------------Destruction-------------------------------------

        if(error) //S'il ya eu une erreur pendant l'initialisation, on demande à Keyboard de se détruire
        {
            kill (noKeyboard, SIGUSR2);
        }

        /** Envoi du signal de fin d'application aux taches filles et attente de la fin de celles ci
         * afin de détruire les canaux (car il faut qu'il n'y ait plus ni écrivains ni lecteurs du le canal
         * avant de le fermer
         */
        kill (noEntreeBPA, SIGUSR2);
        do{
            waitpid (noEntreeBPA, &st, 0);
        }while(st != 0);

        kill (noEntreeBPP, SIGUSR2);
        do{
            waitpid (noEntreeBPP, &st, 0);
        }while(st != 0);

        kill (noEntreeGB, SIGUSR2);
        do{
            waitpid (noEntreeGB, &st, 0);
        }while(st != 0);

        kill (noHeure, SIGUSR2);
        do{
            waitpid (noHeure, &st, 0);
        }while(st != 0);

        kill (noSortie, SIGUSR2);
        do{
            waitpid (noSortie, &st, 0);
        }while(st != 0);

        //Destruction des canaux, il est necessaire d'attendre qu'il n'y ai plus de lecteurs ni d'écrivains. 
        unlink (CANAL_KEY_ENTREE_BP_A);
        unlink (CANAL_KEY_ENTREE_BP_P);
        unlink (CANAL_KEY_ENTREE_GB);
        unlink (CANAL_KEY_SORTIE);

        //Destruction des mémoires partagées 
        shmdt (shmPtCompteur);
        shmctl (shmIdCompteur, IPC_RMID, 0);
        shmctl (shmIdRequetes, IPC_RMID, 0);

        //Destruction des sémaphores
        sem_unlink (SEM_ENTREE_BP_A);
        sem_unlink (SEM_ENTREE_BP_P);
        sem_unlink (SEM_ENTREE_GB);
        sem_unlink (SEM_SHM_COMPTEUR);
        sem_unlink (SEM_SHM_REQUETE);

        //Suppression de l'écran et de ses protections
        TerminerApplication ();
    }

    if(error){
        return 1;
    }
    else{
        return 0;
    }
}

